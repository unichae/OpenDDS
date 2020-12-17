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

#include "Publisher.cpp"

int ACE_TMAIN(int argc, ACE_TCHAR *argv[]) {
    std::cout << "**********publisher test start" << std::endl;

    auto temp = new ATCSPublisher(argc, argv, 42);

    temp->createDomainParticipant();
    temp->registerTypeSupport();
    temp->createTopic("atcs test topic");
    temp->createPublisher();
    temp->createDataWriter();
    temp->waitSubscriber();

    while (true) {
        temp->msgSend();
        std::cout << "***** msg sent" << std::endl;
        // sleep(1);
    }

    std::cout << "**********publisher test end" << std::endl;
}
