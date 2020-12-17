/*
 *
 *
 * Distributed under the OpenDDS License.
 * See: http://www.opendds.org/license.html
 */

#include "DataReaderListenerImpl.h"

#include <ace/Log_Msg.h>
#include <ace/OS_NS_stdlib.h>

#include <iostream>

#include "OnboardATCSTypeSupportC.h"
#include "OnboardATCSTypeSupportImpl.h"

void DataReaderListenerImpl::on_requested_deadline_missed(
    DDS::DataReader_ptr /*reader*/,
    const DDS::RequestedDeadlineMissedStatus& /*status*/) {
}

void DataReaderListenerImpl::on_requested_incompatible_qos(
    DDS::DataReader_ptr /*reader*/,
    const DDS::RequestedIncompatibleQosStatus& /*status*/) {
}

void DataReaderListenerImpl::on_sample_rejected(
    DDS::DataReader_ptr /*reader*/,
    const DDS::SampleRejectedStatus& /*status*/) {
}

void DataReaderListenerImpl::on_liveliness_changed(
    DDS::DataReader_ptr /*reader*/,
    const DDS::LivelinessChangedStatus& /*status*/) {
}

void DataReaderListenerImpl::on_data_available(DDS::DataReader_ptr reader) {
    OnboardATCS::TrainMessageDataReader_var reader_i =
        OnboardATCS::TrainMessageDataReader::_narrow(reader);

    if (!reader_i) {
        ACE_ERROR((LM_ERROR,
                   ACE_TEXT("ERROR: %N:%l: on_data_available() -")
                       ACE_TEXT(" _narrow failed!\n")));
        ACE_OS::exit(1);
    }

    OnboardATCS::TrainMessage message;
    DDS::SampleInfo info;

    DDS::ReturnCode_t error = reader_i->take_next_sample(message, info);

    if (error == DDS::RETCODE_OK) {
        // std::cout << "SampleInfo.sample_rank = " << info.sample_rank << std::endl;
        // std::cout << "SampleInfo.instance_state = " << info.instance_state << std::endl;

        if (info.valid_data) {
            std::cout << "Message: nid_train_id    = " << message.nid_train_id << std::endl
                      << "         msg_id  = " << message.msg_id << std::endl
                      << "         seq      = " << message.seq << std::endl;
                      // << "         text       = " << message.temp.in() << std::endl;

            for (int i = 0; i < message.n_iter_i; i++) {
                std::cout << "d_dist_from[" << i << "]: " << message.d_dist_from_i[i] << std::endl;
            }
            
        }

    } else {
        ACE_ERROR((LM_ERROR,
                   ACE_TEXT("ERROR: %N:%l: on_data_available() -")
                       ACE_TEXT(" take_next_sample failed!\n")));
    }
}

void DataReaderListenerImpl::on_subscription_matched(
    DDS::DataReader_ptr /*reader*/,
    const DDS::SubscriptionMatchedStatus& /*status*/) {
}

void DataReaderListenerImpl::on_sample_lost(
    DDS::DataReader_ptr /*reader*/,
    const DDS::SampleLostStatus& /*status*/) {
}
