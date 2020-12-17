#include "ExchangeEventDataReaderListenerImpl.h"
#include "QuoteDataReaderListenerImpl.h"
#include "StockQuoterTypeSupportImpl.h"
#include "ace/streams.h"
#include "dds/DCPS/Marked_Default_Qos.h"
#include "dds/DCPS/Service_Participant.h"
#include "dds/DCPS/StaticIncludes.h"
#include "dds/DCPS/SubscriberImpl.h"
#include "orbsvcs/Time_Utilities.h"

// constants for Stock Quoter domain Id, types, and topic
DDS::DomainId_t QUOTER_DOMAIN_ID = 1066;
const char *QUOTER_QUOTE_TYPE = "Quote Type";
const char *QUOTER_QUOTE_TOPIC = "Stock Quotes";
const char *QUOTER_EXCHANGE_EVENT_TYPE = "Exchange Event Type";
const char *QUOTER_EXCHANGE_EVENT_TOPIC = "Stock Exchange Events";

int main(int argc, char *argv[]) {
    DDS::DomainParticipantFactory_var dpf = DDS::DomainParticipantFactory::_nil();
    DDS::DomainParticipant_var participant = DDS::DomainParticipant::_nil();

    try {
        // create participant
        dpf = TheParticipantFactoryWithArgs(argc, argv);
        participant = dpf->create_participant(QUOTER_DOMAIN_ID, PARTICIPANT_QOS_DEFAULT, DDS::DomainParticipantListener::_nil(), ::OpenDDS::DCPS::DEFAULT_STATUS_MASK);

        if (CORBA::is_nil(participant.in())) {
            cerr << "create_participant failed." << endl;
            ACE_OS::exit(1);
        }

        // create subscriber
        DDS::Subscriber_var subscriber = participant->create_subscriber(SUBSCRIBER_QOS_DEFAULT, DDS::SubscriberListener::_nil(), ::OpenDDS::DCPS::DEFAULT_STATUS_MASK);

        if (CORBA::is_nil(subscriber.in())) {
            cerr << "create_subscriber failed." << endl;
            ACE_OS::exit(1);
        }

        // register Quote type
        StockQuoter::QuoteTypeSupport_var quote_servant = new StockQuoter::QuoteTypeSupportImpl();

        if (DDS::RETCODE_OK != quote_servant->register_type(participant.in(), QUOTER_QUOTE_TYPE)) {
            cerr << "register_type for " << QUOTER_QUOTE_TYPE << " failed." << endl;
            ACE_OS::exit(1);
        }

        // register ExchangeEvent type
        StockQuoter::ExchangeEventTypeSupport_var exchange_event_servant = new StockQuoter::ExchangeEventTypeSupportImpl();

        if (DDS::RETCODE_OK != exchange_event_servant->register_type(participant.in(), QUOTER_EXCHANGE_EVENT_TYPE)) {
            cerr << "register_type for " << QUOTER_EXCHANGE_EVENT_TYPE << " failed." << endl;
            ACE_OS::exit(1);
        }

        // create topic and qos
        DDS::TopicQos default_topic_qos;
        participant->get_default_topic_qos(default_topic_qos);

        DDS::Topic_var quote_topic = participant->create_topic(QUOTER_QUOTE_TOPIC, QUOTER_QUOTE_TYPE, default_topic_qos, DDS::TopicListener::_nil(), ::OpenDDS::DCPS::DEFAULT_STATUS_MASK);

        if (CORBA::is_nil(quote_topic.in())) {
            cerr << "create_topic for " << QUOTER_QUOTE_TOPIC << " failed." << endl;
            ACE_OS::exit(1);
        }

        DDS::Topic_var exchange_event_topic = participant->create_topic(QUOTER_EXCHANGE_EVENT_TOPIC, QUOTER_EXCHANGE_EVENT_TYPE, default_topic_qos, DDS::TopicListener::_nil(), ::OpenDDS::DCPS::DEFAULT_STATUS_MASK);

        if (CORBA::is_nil(exchange_event_topic.in())) {
            cerr << "create_topic for " << QUOTER_EXCHANGE_EVENT_TOPIC << " failed." << endl;
            ACE_OS::exit(1);
        }

        // create data readers
        // QuoteDataReaderListenerImpl quote_listener_servant;
        // DDS::DataReaderListener_var quote_listener = new QuoteDataReaderListenerImpl;
        DDS::DataReaderListener_var quote_listener(new QuoteDataReaderListenerImpl);

        if (CORBA::is_nil(quote_listener.in())) {
            cerr << "quote_listener is nil." << endl;
            ACE_OS::exit(1);
        }

        // ExchangeEventDataReaderListenerImpl exchange_event_listener_servant;
        DDS::DataReaderListener_var exchange_event_listener(new ExchangeEventDataReaderListenerImpl);
        ExchangeEventDataReaderListenerImpl *exchange_event_listener_servant = dynamic_cast<ExchangeEventDataReaderListenerImpl *>(exchange_event_listener.in());

        if (CORBA::is_nil(exchange_event_listener.in())) {
            cerr << "exchange_event_listener is nil." << endl;
            ACE_OS::exit(1);
        }

        DDS::DataReaderQos dr_default_qos;

        subscriber->get_default_datareader_qos(dr_default_qos);
        DDS::DataReader_var quote_dr = subscriber->create_datareader(quote_topic.in(), dr_default_qos, quote_listener.in(), ::OpenDDS::DCPS::DEFAULT_STATUS_MASK);
        DDS::DataReader_var exchange_event_dr = subscriber->create_datareader(exchange_event_topic.in(), dr_default_qos, exchange_event_listener.in(), ::OpenDDS::DCPS::DEFAULT_STATUS_MASK);

        // wait event
        cout << "Subscriber waiting for events" << endl;

        while (!exchange_event_listener_servant->is_exchange_closed_received()) {
            ACE_OS::sleep(1);
        }

        cout << "received CLOSED event from publisher "
             << " ... exit..." << endl;

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
    