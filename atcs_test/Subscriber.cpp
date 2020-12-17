#include <ace/Log_Msg.h>
#include <dds/DCPS/Marked_Default_Qos.h>
#include <dds/DCPS/Service_Participant.h>
#include <dds/DCPS/StaticIncludes.h>
#include <dds/DCPS/WaitSet.h>
#include <dds/DdsDcpsInfrastructureC.h>
#include <dds/DdsDcpsSubscriptionC.h>
#ifdef ACE_AS_STATIC_LIBS
#include <dds/DCPS/RTPS/RtpsDiscovery.h>
#include <dds/DCPS/transport/rtps_udp/RtpsUdp.h>
#endif

#include <iostream>

#include "DataReaderListenerImpl.h"
#include "OnboardATCSTypeSupportImpl.h"

class ATCSSubscriber {
   private:
    DDS::DomainParticipantFactory_var dpf;
    DDS::DomainParticipant_var participant;
    OnboardATCS::TrainMessageTypeSupport_var ts;
    CORBA::String_var type_name;
    DDS::Topic_var topic;
    DDS::Subscriber_var subscriber;
    DDS::DataReaderListener_var listener;
    DDS::DataReaderQos reader_qos;
    DDS::DataReader_var reader;
    OnboardATCS::TrainMessageDataReader_var reader_i;
    DDS::StatusCondition_var condition;
    DDS::WaitSet_var ws;
    DDS::DomainId_t domain_id;

   public:
    ATCSSubscriber(int argc, ACE_TCHAR *argv[], DDS::DomainId_t id) {
        dpf = TheParticipantFactoryWithArgs(argc, argv);
        domain_id = id;
    }

    // Create DomainParticipant
    int createDomainParticipant() {
        participant =
            dpf->create_participant(domain_id,
                                    PARTICIPANT_QOS_DEFAULT,
                                    0,
                                    OpenDDS::DCPS::DEFAULT_STATUS_MASK);

        if (!participant) {
            ACE_ERROR_RETURN((LM_ERROR,
                              ACE_TEXT("ERROR: %N:%l: main() -")
                                  ACE_TEXT(" create_participant failed!\n")),
                             1);
        }
    }

    // Register TypeSupport
    int registerTypeSupport() {
        ts = new OnboardATCS::TrainMessageTypeSupportImpl;

        if (ts->register_type(participant, "") != DDS::RETCODE_OK) {
            ACE_ERROR_RETURN((LM_ERROR,
                              ACE_TEXT("ERROR: %N:%l: main() -")
                                  ACE_TEXT(" register_type failed!\n")),
                             1);
        }
    }

    // Create Topic
    int createTopic(CORBA::String_var sTopic) {
        type_name = ts->get_type_name();
        topic = participant->create_topic(sTopic,
                                          type_name,
                                          TOPIC_QOS_DEFAULT,
                                          0,
                                          OpenDDS::DCPS::DEFAULT_STATUS_MASK);

        if (!topic) {
            ACE_ERROR_RETURN((LM_ERROR,
                              ACE_TEXT("ERROR: %N:%l: main() -")
                                  ACE_TEXT(" create_topic failed!\n")),
                             1);
        }
    }

    // Create Subscriber
    int createSubscriber() {
        subscriber = participant->create_subscriber(SUBSCRIBER_QOS_DEFAULT,
                                                    0,
                                                    OpenDDS::DCPS::DEFAULT_STATUS_MASK);

        if (!subscriber) {
            ACE_ERROR_RETURN((LM_ERROR,
                              ACE_TEXT("ERROR: %N:%l: main() -")
                                  ACE_TEXT(" create_subscriber failed!\n")),
                             1);
        }
    }

    // Create DataReader
    int createDataReader() {
        listener = new DataReaderListenerImpl;
        subscriber->get_default_datareader_qos(reader_qos);
        reader_qos.reliability.kind = DDS::RELIABLE_RELIABILITY_QOS;

        reader = subscriber->create_datareader(topic,
                                               reader_qos,
                                               listener,
                                               OpenDDS::DCPS::DEFAULT_STATUS_MASK);

        if (!reader) {
            ACE_ERROR_RETURN((LM_ERROR,
                              ACE_TEXT("ERROR: %N:%l: main() -")
                                  ACE_TEXT(" create_datareader failed!\n")),
                             1);
        }

        reader_i = OnboardATCS::TrainMessageDataReader::_narrow(reader);

        if (!reader_i) {
            ACE_ERROR_RETURN((LM_ERROR,
                              ACE_TEXT("ERROR: %N:%l: main() -")
                                  ACE_TEXT(" _narrow failed!\n")),
                             1);
        }
    }

    // Block until Publisher completes
    int readData() {
        condition = reader->get_statuscondition();
        condition->set_enabled_statuses(DDS::SUBSCRIPTION_MATCHED_STATUS);

        ws = new DDS::WaitSet;
        ws->attach_condition(condition);

        while (true) {
            DDS::SubscriptionMatchedStatus matches;
            if (reader->get_subscription_matched_status(matches) != DDS::RETCODE_OK) {
                ACE_ERROR_RETURN((LM_ERROR,
                                  ACE_TEXT("ERROR: %N:%l: main() -")
                                      ACE_TEXT(" get_subscription_matched_status failed!\n")),
                                 1);
            }

            if (matches.current_count == 0 && matches.total_count > 0) {
                break;
            }

            // DDS::ConditionSeq conditions;
            // DDS::Duration_t timeout = {160, 0};
            // if (ws->wait(conditions, timeout) != DDS::RETCODE_OK) {
            //     ACE_ERROR_RETURN((LM_ERROR,
            //                       ACE_TEXT("ERROR: %N:%l: main() -")
            //                           ACE_TEXT(" wait failed!\n")),
            //                      1);
            // }
        }

        ws->detach_condition(condition);
    }

    // Clean-up!
    ~ATCSSubscriber() {
        participant->delete_contained_entities();
        dpf->delete_participant(participant);

        TheServiceParticipant->shutdown();
    }
};