#include <ace/Log_Msg.h>
#include <dds/DCPS/Marked_Default_Qos.h>
#include <dds/DCPS/Service_Participant.h>
#include <dds/DCPS/StaticIncludes.h>
#include <dds/DCPS/WaitSet.h>
#include <dds/DdsDcpsInfrastructureC.h>
#include <dds/DdsDcpsPublicationC.h>

#ifdef ACE_AS_STATIC_LIBS
#include <dds/DCPS/RTPS/RtpsDiscovery.h>
#include <dds/DCPS/transport/rtps_udp/RtpsUdp.h>
#endif

#include "OnboardATCSTypeSupportImpl.h"
#include <iostream>

class ATCSPublisher {
   private:
    DDS::DomainParticipantFactory_var dpf;
    DDS::DomainParticipant_var participant;
    OnboardATCS::TrainMessageTypeSupport_var ts;
    CORBA::String_var type_name;
    DDS::Topic_var topic;
    DDS::Publisher_var publisher;
    DDS::DataWriter_var writer;
    OnboardATCS::TrainMessageDataWriter_var message_writer;
    DDS::StatusCondition_var condition;
    DDS::WaitSet_var ws;
    DDS::ConditionSeq conditions;
    short seq;
    DDS::DomainId_t domain_id;
    

   public:
    ATCSPublisher(int argc, ACE_TCHAR *argv[], DDS::DomainId_t id) {
        dpf = TheParticipantFactoryWithArgs(argc, argv);
        seq = 0;
        domain_id = id;
    }

    // create domain participant
    int createDomainParticipant() {
        participant = dpf->create_participant(42,
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
        topic =
            participant->create_topic(sTopic,
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

    // Create Publisher
    int createPublisher() {
        publisher =
            participant->create_publisher(PUBLISHER_QOS_DEFAULT,
                                          0,
                                          OpenDDS::DCPS::DEFAULT_STATUS_MASK);

        if (!publisher) {
            ACE_ERROR_RETURN((LM_ERROR,
                              ACE_TEXT("ERROR: %N:%l: main() -")
                                  ACE_TEXT(" create_publisher failed!\n")),
                             1);
        }
    }

    // Create DataWriter
    int createDataWriter() {
        writer =
            publisher->create_datawriter(topic,
                                         DATAWRITER_QOS_DEFAULT,
                                         0,
                                         OpenDDS::DCPS::DEFAULT_STATUS_MASK);

        if (!writer) {
            ACE_ERROR_RETURN((LM_ERROR,
                              ACE_TEXT("ERROR: %N:%l: main() -")
                                  ACE_TEXT(" create_datawriter failed!\n")),
                             1);
        }

        message_writer =
            OnboardATCS::TrainMessageDataWriter::_narrow(writer);

        if (!message_writer) {
            ACE_ERROR_RETURN((LM_ERROR,
                              ACE_TEXT("ERROR: %N:%l: main() -")
                                  ACE_TEXT(" _narrow failed!\n")),
                             1);
        }
    }

    // Block until Subscriber is available
    int waitSubscriber() {
        condition = writer->get_statuscondition();
        condition->set_enabled_statuses(DDS::PUBLICATION_MATCHED_STATUS);

        ws = new DDS::WaitSet;
        ws->attach_condition(condition);

        while (true) {
            DDS::PublicationMatchedStatus matches;
            if (writer->get_publication_matched_status(matches) != ::DDS::RETCODE_OK) {
                ACE_ERROR_RETURN((LM_ERROR,
                                  ACE_TEXT("ERROR: %N:%l: main() -")
                                      ACE_TEXT(" get_publication_matched_status failed!\n")),
                                 1);
            }

            if (matches.current_count >= 1) {
                break;
            }

            DDS::ConditionSeq conditions;
            DDS::Duration_t timeout = {60, 0};
            if (ws->wait(conditions, timeout) != DDS::RETCODE_OK) {
                ACE_ERROR_RETURN((LM_ERROR,
                                  ACE_TEXT("ERROR: %N:%l: main() -")
                                      ACE_TEXT(" wait failed!\n")),
                                 1);
            }
        }

        ws->detach_condition(condition);
    }

    int msgSend() {
        OnboardATCS::TrainMessage msg;
        // for debugging
        msg.msg_id = 73;
        msg.seq = seq;
        
        // msg.temp = something;

        // for ATCS msg
        msg.nid_train_id = 99;
        msg.v_train = 15;
        msg.n_iter_i = 2;
        int seq_length = msg.n_iter_i;
        msg.d_dist_from_i.length(seq_length);

        for (int i = 0; i < seq_length; i++) {
            msg.d_dist_from_i[i] = i + seq;
        }
        
        seq++;

        // message sent
        DDS::ReturnCode_t error = message_writer->write(msg, DDS::HANDLE_NIL);

        if (error != DDS::RETCODE_OK) {
            ACE_ERROR((LM_ERROR,
                       ACE_TEXT("ERROR: %N:%l: main() -")
                           ACE_TEXT(" write returned %d!\n"),
                       error));
        }

        // Wait for samples to be acknowledged
        DDS::Duration_t timeout = {10, 0};
        if (message_writer->wait_for_acknowledgments(timeout) != DDS::RETCODE_OK) {
            ACE_ERROR_RETURN((LM_ERROR,
                              ACE_TEXT("ERROR: %N:%l: main() -")
                                  ACE_TEXT(" wait_for_acknowledgments failed!\n")),
                             1);
        }
    }

    // Clean-up!
    ~ATCSPublisher() {
        participant->delete_contained_entities();
        dpf->delete_participant(participant);

        TheServiceParticipant->shutdown();
    }
};