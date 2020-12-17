#include "StockQuoterTypeSupportImpl.h"
#include "ace/streams.h"
#include "dds/DCPS/Marked_Default_Qos.h"
#include "dds/DCPS/PublisherImpl.h"
#include "dds/DCPS/Service_Participant.h"
#include "dds/DCPS/StaticIncludes.h"
#include "orbsvcs/Time_Utilities.h"

// constants for Stock Quoter domain Id, types, and topic
DDS::DomainId_t QUOTER_DOMAIN_ID = 1066;
const char *QUOTER_QUOTE_TYPE = "Quote Type";
const char *QUOTER_QUOTE_TOPIC = "Stock Quotes";
const char *QUOTER_EXCHANGE_EVENT_TYPE = "Exchange Event Type";
const char *QUOTER_EXCHANGE_EVENT_TOPIC = "Stock Exchange Events";
const char *STOCK_EXCHANGE_NAME = "Test Stock Exchange";

// simple helper method
TimeBase::TimeT get_timestamp() {
    TimeBase::TimeT retval;
    ACE_hrtime_t t = ACE_OS::gethrtime();
    ORBSVCS_Time::hrtime_to_TimeT(retval, t);
    return retval;
}

int main(int argc, char *argv[]) {
    // domain participant
    DDS::DomainParticipantFactory_var dpf = DDS::DomainParticipantFactory::_nil();
    DDS::DomainParticipant_var participant = DDS::DomainParticipant::_nil();

    try {
        // initialize participant
        dpf = TheParticipantFactoryWithArgs(argc, argv);
        participant =
            dpf->create_participant(QUOTER_DOMAIN_ID, PARTICIPANT_QOS_DEFAULT,
                                    DDS::DomainParticipantListener::_nil(), ::OpenDDS::DCPS::DEFAULT_STATUS_MASK);

        if (CORBA::is_nil(participant.in())) {
            cerr << "create_participant failed." << endl;
            ACE_OS::exit(1);
        }

        // publisher
        DDS::Publisher_var pub = participant->create_publisher(PUBLISHER_QOS_DEFAULT, DDS::PublisherListener::_nil(), ::OpenDDS::DCPS::DEFAULT_STATUS_MASK);

        if (CORBA::is_nil(pub.in())) {
            cerr << "create_publisher failed." << endl;
            ACE_OS::exit(1);
        }

        // register the quote type
        StockQuoter::QuoteTypeSupport_var quote_servant =
            new StockQuoter::QuoteTypeSupportImpl();

        if (DDS::RETCODE_OK != quote_servant->register_type(participant.in(), QUOTER_QUOTE_TYPE)) {
            cerr << "register_type for " << QUOTER_QUOTE_TYPE << " failed. " << endl;
            ACE_OS::exit(1);
        }

        // register the ExchangeEvent type
        StockQuoter::ExchangeEventTypeSupport_var exchange_evt_servant = new StockQuoter::ExchangeEventTypeSupportImpl();

        if (DDS::RETCODE_OK !=
            exchange_evt_servant->register_type(
                participant.in(),
                QUOTER_EXCHANGE_EVENT_TYPE)) {
            cerr << "register_type for "
                 << QUOTER_EXCHANGE_EVENT_TYPE
                 << " failed." << endl;
            ACE_OS::exit(1);
        }

        // topic
        DDS::TopicQos default_topic_qos;
        participant->get_default_topic_qos(default_topic_qos);

        DDS::Topic_var quote_topic = participant->create_topic(QUOTER_QUOTE_TOPIC, QUOTER_QUOTE_TYPE, default_topic_qos, DDS::TopicListener::_nil(), ::OpenDDS::DCPS::DEFAULT_STATUS_MASK);

        if (CORBA::is_nil(quote_topic.in())) {
            cerr << "create topic for " << QUOTER_QUOTE_TOPIC << " failed." << endl;
            ACE_OS::exit(1);
        }

        DDS::Topic_var exchange_event_topic = participant->create_topic(QUOTER_EXCHANGE_EVENT_TOPIC, QUOTER_EXCHANGE_EVENT_TYPE, default_topic_qos, DDS::TopicListener::_nil(), ::OpenDDS::DCPS::DEFAULT_STATUS_MASK);

        if (CORBA::is_nil(exchange_event_topic.in())) {
            cerr << "create topic for " << QUOTER_EXCHANGE_EVENT_TOPIC << " failed." << endl;
            ACE_OS::exit(1);
        }

        // data writers
        DDS::DataWriterQos dw_default_qos;
        pub->get_default_datawriter_qos(dw_default_qos);

        DDS::DataWriter_var quote_base_dw = pub->create_datawriter(quote_topic.in(), dw_default_qos, DDS::DataWriterListener::_nil(), ::OpenDDS::DCPS::DEFAULT_STATUS_MASK);

        if (CORBA::is_nil(quote_base_dw.in())) {
            cerr << "create_datawriter for " << QUOTER_QUOTE_TOPIC << " failed." << endl;
            ACE_OS::exit(1);
        }

        StockQuoter::QuoteDataWriter_var quote_dw = StockQuoter::QuoteDataWriter::_narrow(quote_base_dw.in());

        if (CORBA::is_nil(quote_dw.in())) {
            cerr << "QuoteDataWriter cannot be narrowed." << endl;
            ACE_OS::exit(1);
        }

        DDS::DataWriter_var exchange_event_base_dw = pub->create_datawriter(exchange_event_topic.in(), dw_default_qos, DDS::DataWriterListener::_nil(), ::OpenDDS::DCPS::DEFAULT_STATUS_MASK);

        if (CORBA::is_nil(exchange_event_base_dw.in())) {
            cerr << "create_datawriter for " << QUOTER_EXCHANGE_EVENT_TOPIC << " failed." << endl;
            ACE_OS::exit(1);
        }

        StockQuoter::ExchangeEventDataWriter_var exchange_event_dw = StockQuoter::ExchangeEventDataWriter::_narrow(exchange_event_base_dw.in());

        if (CORBA::is_nil(exchange_event_base_dw.in())) {
            cerr << "ExchangeEventDataWriter cannot be narrowed." << endl;
            ACE_OS::exit(1);
        }

        // register exchange events for two quotes
        StockQuoter::Quote spy;
        spy.ticker = CORBA::string_dup("SPY");
        DDS::InstanceHandle_t spy_handle = quote_dw->register_instance(spy);

        StockQuoter::Quote mdy;
        mdy.ticker = CORBA::string_dup("MDY");
        DDS::InstanceHandle_t mdy_handle = quote_dw->register_instance(mdy);

        StockQuoter::ExchangeEvent exchange_event;
        exchange_event.exchange = STOCK_EXCHANGE_NAME;
        DDS::InstanceHandle_t exchange_event_handle = exchange_event_dw->register_instance(exchange_event);

        // publish TRADING_OPENED
        StockQuoter::ExchangeEvent opened;
        opened.event = StockQuoter::TRADING_OPENED;
        opened.exchange = STOCK_EXCHANGE_NAME;
        opened.timestamp = get_timestamp();
        cout << "publising TRADING_OPENED" << endl;

        DDS::ReturnCode_t ret = exchange_event_dw->write(opened, exchange_event_handle);

        if (ret != DDS::RETCODE_OK) {
            ACE_ERROR((LM_ERROR, ACE_TEXT("(%PI%t) ERROR: OPEN write returned %d. \n"), ret));
        }

        // for sleep
        ACE_Time_Value quarterSecond(0, 250000);

        // spy quote create
        StockQuoter::Quote spy_quote;
        spy_quote.ticker = CORBA::string_dup("SPY");
        spy_quote.exchange = STOCK_EXCHANGE_NAME;
        spy_quote.full_name = CORBA::string_dup("S&P Midcap Depository Receipts");
        
        // mdy quote create 
        StockQuoter::Quote mdy_quote;
        mdy_quote.ticker = CORBA::string_dup("MDY");
        mdy_quote.exchange = STOCK_EXCHANGE_NAME;
        mdy_quote.full_name = CORBA::string_dup("S&P Depository Receipts");

        // publish quote
        for (int i = 0; i < 20; i++) {
            spy_quote.value = 1200.0 + 10.0 * i;
            spy_quote.timestamp = get_timestamp();

            cout << "publising SPY quote: " << spy_quote.value << endl;

            ret = quote_dw->write(spy_quote, spy_handle);
            if (ret != DDS::RETCODE_OK) {
                ACE_ERROR((LM_ERROR, ACE_TEXT("(%PI%t) ERROR: SPY write returned %d. \n"), ret));
            }

            ACE_OS::sleep(quarterSecond);

        
            mdy_quote.value = 1400.0 + 8.0 * i;
            mdy_quote.timestamp = get_timestamp();

            cout << "publising MDY quote: " << mdy_quote.value << endl;

            ret = quote_dw->write(mdy_quote, mdy_handle);
            if (ret != DDS::RETCODE_OK) {
                ACE_ERROR((LM_ERROR, ACE_TEXT("(%PI%t) ERROR: MDY write returned %d. \n"), ret));
            }

            ACE_OS::sleep(quarterSecond);
        }

        // publish TRADING_CLOSED
        StockQuoter::ExchangeEvent closed;
        closed.event = StockQuoter::TRADING_CLOSED;
        closed.exchange = STOCK_EXCHANGE_NAME;
        closed.timestamp = get_timestamp();
        cout << "publising TRADING_CLOSED" << endl;

        ret = exchange_event_dw->write(closed, exchange_event_handle);

        if (ret != DDS::RETCODE_OK) {
            ACE_ERROR((LM_ERROR, ACE_TEXT("(%PI%t) ERROR: CLOSED write returned %d. \n"), ret));
        }

    } catch (CORBA::Exception &e) {
        cerr << e << '\n';
        ACE_OS::exit(1);
    }

    // cleanup
    try {
        if (!CORBA::is_nil(participant.in())) {
            participant->delete_contained_entities();
        }
        if (!CORBA::is_nil(dpf.in())) {
            dpf->delete_participant(participant.in());
        }
    } catch (CORBA::Exception &e) {
        cerr << "Exception caught in cleanup."
             << endl
             << e << endl;
        ACE_OS::exit(1);
    }
    TheServiceParticipant->shutdown();
    return 0;
}
