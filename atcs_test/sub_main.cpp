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

#include <iostream>

#include "DataReaderListenerImpl.h"
#include "OnboardATCSTypeSupportImpl.h"
#include "Subscriber.cpp"

int ACE_TMAIN(int argc, ACE_TCHAR *argv[]) {
    std::cout << "subscriber test start" << std::endl;

    auto temp = new ATCSSubscriber(argc, argv, 42);

    temp->createDomainParticipant();
    temp->registerTypeSupport();
    temp->createTopic("atcs test topic");
    temp->createSubscriber();
    temp->createDataReader();
    temp->readData();

    std::cout << "subscriber test end" << std::endl;
}
