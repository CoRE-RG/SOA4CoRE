//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "soqosmw/applications/someipapp/base/ISomeIpServiceApp.h"
#include <soqosmw/discovery/someipservicediscovery/SomeIpSD.h>
#include <soqosmw/servicemanager/someipservicemanager/SomeIpLocalServiceManager.h>

#include <inet/networklayer/common/L3AddressResolver.h>
#include <inet/networklayer/contract/ipv4/IPv4Address.h>
#include <list>

namespace SOQoSMW {

Define_Module(SomeIpSD);

void SomeIpSD::initialize(int stage) {
    inet::UDPBasicApp::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        if (!par("localAddress")) {
            throw cRuntimeError("Please define a local ip address");
        }
    }
}

void SomeIpSD::handleMessageWhenUp(cMessage *msg) {
    if (SomeIpSDHeader *someIpSDHeader = dynamic_cast<SomeIpSDHeader*>(msg)) {
        if(inet::UDPDataIndication *udpDataIndication = dynamic_cast<inet::UDPDataIndication*>(someIpSDHeader->getControlInfo())) {
            if (udpDataIndication->getSrcAddr() != inet::L3AddressResolver().resolve(par("localAddress"))) {
                processSomeIpSDHeader(someIpSDHeader);
            }
        }
    }
    delete msg;
}

void SomeIpSD::find(uint16_t serviceID, uint16_t instanceID) {
    Enter_Method("SomeIpSD::find");
    SomeIpSDHeader *someIpSDHeader = new SomeIpSDHeader("SOME/IP SD - FIND");

    ServiceEntry *findEntry = new ServiceEntry("ServiceEntry");
    findEntry->setType(SOQoSMW::SomeIpSDEntryType::FIND);
    findEntry->setIndex1stOptions(0x00);
    findEntry->setIndex2ndOptions(0x00);
    findEntry->setNum1stAnd2ndOptions(0x00);
    findEntry->setServiceID(serviceID);
    findEntry->setInstanceID(instanceID);
    findEntry->setMajorVersion_TTL(0x01000003);
    findEntry->setMinorVersion(0xFFFFFFFF);
    someIpSDHeader->encapEntry(findEntry);

    socket.sendTo(someIpSDHeader, inet::IPv4Address(BROADCASTADDRESS), destPort);
}

void SomeIpSD::offer(uint16_t serviceID, uint16_t instanceID, inet::L3Address remoteAddress, inet::L3Address publisherIP, uint16_t publisherPort) {
    Enter_Method("SomeIpSD::offer");
    SomeIpSDHeader *someIpSDHeader = new SomeIpSDHeader("SOME/IP SD - OFFER");

    ServiceEntry *offerEntry = new ServiceEntry("ServiceEntry");
    offerEntry->setType(SOQoSMW::SomeIpSDEntryType::OFFER);
    offerEntry->setIndex1stOptions(0x00);
    offerEntry->setIndex2ndOptions(0x00);
    offerEntry->setNum1stAnd2ndOptions(0x01);
    offerEntry->setServiceID(serviceID);
    offerEntry->setInstanceID(instanceID);
    offerEntry->setMajorVersion_TTL(0x01000003);
    offerEntry->setMinorVersion(0xFFFFFFFF);
    someIpSDHeader->encapEntry(offerEntry);

    IPv4EndpointOption *ipv4EndpointOption = new IPv4EndpointOption("IPv4EndpointOption of Publisher");
    ipv4EndpointOption->setIpv4Address(publisherIP.toIPv4());
    ipv4EndpointOption->setL4Protocol(IPProtocolId::IP_PROT_UDP);
    ipv4EndpointOption->setPort(publisherPort);
    someIpSDHeader->encapOption(ipv4EndpointOption);

    socket.sendTo(someIpSDHeader, remoteAddress, destPort);
}

void SomeIpSD::subscribeEventgroup(uint16_t serviceID, uint16_t instanceID, inet::L3Address remoteAddress, inet::L3Address subscriberIP, uint16_t subscriberPort) {
    Enter_Method("SomeIpSD::subscribeEventgroup");
    SomeIpSDHeader *someIpSDHeader = new SomeIpSDHeader("SOME/IP SD - SUBSCRIBEEVENTGROUP");

    EventgroupEntry *subscribeEventgroupEntry = new EventgroupEntry("EventgroupEntry");
    subscribeEventgroupEntry->setType(SOQoSMW::SomeIpSDEntryType::SUBSCRIBEVENTGROUP);
    subscribeEventgroupEntry->setIndex1stOptions(0);
    subscribeEventgroupEntry->setIndex2ndOptions(0);
    subscribeEventgroupEntry->setNum1stAnd2ndOptions(0x01);
    subscribeEventgroupEntry->setServiceID(serviceID);
    subscribeEventgroupEntry->setInstanceID(instanceID);
    subscribeEventgroupEntry->setMajorVersion_TTL(0x01000003);
    someIpSDHeader->encapEntry(subscribeEventgroupEntry);

    IPv4EndpointOption *ipv4EndpointOption = new IPv4EndpointOption("IPv4EndpointOption of Subscriber");
    ipv4EndpointOption->setIpv4Address(subscriberIP.toIPv4());
    ipv4EndpointOption->setL4Protocol(IPProtocolId::IP_PROT_UDP);
    ipv4EndpointOption->setPort(subscriberPort);
    someIpSDHeader->encapOption(ipv4EndpointOption);

    socket.sendTo(someIpSDHeader, remoteAddress, destPort);
}

void SomeIpSD::subscribeEventgroupAck(uint16_t serviceID, uint16_t instanceID, inet::L3Address publisherIP, uint16_t publisherPort, inet::L3Address remoteAddress) {
    Enter_Method("SomeIpSD::subscribeEventgroupAck");
    SomeIpSDHeader *someIpSDHeader = new SomeIpSDHeader("SOME/IP SD - SUBSCRIBEEVENTGROUPACK");

    EventgroupEntry *subscribeEventgroupAckEntry = new EventgroupEntry("EventgroupEntry");
    subscribeEventgroupAckEntry->setType(SOQoSMW::SomeIpSDEntryType::SUBSCRIBEVENTGROUPACK);
    subscribeEventgroupAckEntry->setIndex1stOptions(0);
    subscribeEventgroupAckEntry->setIndex2ndOptions(0);
    subscribeEventgroupAckEntry->setNum1stAnd2ndOptions(0x01);
    subscribeEventgroupAckEntry->setServiceID(serviceID);
    subscribeEventgroupAckEntry->setInstanceID(instanceID);
    subscribeEventgroupAckEntry->setMajorVersion_TTL(0x01000003);
    someIpSDHeader->encapEntry(subscribeEventgroupAckEntry);

    IPv4EndpointOption *ipv4EndpointOption = new IPv4EndpointOption("IPv4EndpointOption of Publisher");
    ipv4EndpointOption->setIpv4Address(publisherIP.toIPv4());
    ipv4EndpointOption->setL4Protocol(IPProtocolId::IP_PROT_UDP);
    ipv4EndpointOption->setPort(publisherPort);
    someIpSDHeader->encapOption(ipv4EndpointOption);

    socket.sendTo(someIpSDHeader, remoteAddress, destPort);
}

void SomeIpSD::processSomeIpSDHeader(SomeIpSDHeader* someIpSDHeader) {
    std::list<SomeIpSDEntry*> entries = someIpSDHeader->getEncapEntries();
    for (std::list<SomeIpSDEntry*>::const_iterator it = entries.begin(); it != entries.end(); ++it) {
        switch ((*it)->getType()) {
            case SOQoSMW::SomeIpSDEntryType::FIND:
                EV << "FIND ARRIVED" << endl;
                processFindEntry(*it, someIpSDHeader);
                break;
            case SOQoSMW::SomeIpSDEntryType::OFFER:
                EV << "OFFER ARRIVED" << endl;
                processOfferEntry(*it, someIpSDHeader);
                break;
            case SOQoSMW::SomeIpSDEntryType::SUBSCRIBEVENTGROUP:
                EV << "SUBSCRIBEVENTGROUP ARRIVED" << endl;
                processSubscribeEventGroupEntry(*it, someIpSDHeader);
                break;
            case SOQoSMW::SomeIpSDEntryType::SUBSCRIBEVENTGROUPACK:
                EV << "SUBSCRIBEVENTGROUPACK ARRIVED" << endl;
                processSubscribeEventGroupAckEntry(*it, someIpSDHeader);
                break;
            default:
                EV << "Unknown type" << std::endl;
        }
    }
}


void SomeIpSD::processFindEntry(SomeIpSDEntry* findEntry, SomeIpSDHeader* someIpSDHeader) {
    std::list<ISomeIpServiceApp*> publisherList = _someIpLSM->lookLocalForPublisherService(findEntry->getServiceID());
    if (!publisherList.empty()) {
        inet::UDPDataIndication *udpDataIndication = dynamic_cast<inet::UDPDataIndication*>(someIpSDHeader->getControlInfo());
        for (ISomeIpServiceApp *someIpPublisher : publisherList) {
            offer(findEntry->getServiceID(), findEntry->getInstanceID(), udpDataIndication->getSrcAddr(),
                    someIpPublisher->getIpAddress(inet::L3Address::AddressType::IPv4), someIpPublisher->getPort());
        }
    }
}

void SomeIpSD::processOfferEntry(SomeIpSDEntry* offerEntry, SomeIpSDHeader* someIpSDHeader) {
    int num2ndOption = offerEntry->getNum1stAnd2ndOptions() & 0x0F;
    if (num2ndOption > 0) {
        IPv4EndpointOption* ipv4EndpointOption = dynamic_cast<IPv4EndpointOption*>(someIpSDHeader->decapOption());
        _someIpLSM->addRemotePublisher(offerEntry->getServiceID(), ipv4EndpointOption->getIpv4Address(), ipv4EndpointOption->getPort());
        delete ipv4EndpointOption;
    }
}

void SomeIpSD::processSubscribeEventGroupEntry(SomeIpSDEntry* subscribeEventGroupEntry, SomeIpSDHeader* someIpSDHeader) {
    int num2ndOption = subscribeEventGroupEntry->getNum1stAnd2ndOptions() & 0x0F;
    if (num2ndOption > 0) {
        IPv4EndpointOption* ipv4EndpointOption = dynamic_cast<IPv4EndpointOption*>(someIpSDHeader->decapOption());
        _someIpLSM->publishToSubscriber(subscribeEventGroupEntry->getServiceID(), ipv4EndpointOption->getIpv4Address(), ipv4EndpointOption->getPort());
        delete ipv4EndpointOption;
    }
}

void SomeIpSD::processSubscribeEventGroupAckEntry(SomeIpSDEntry *subscribeEventGroupAckEntry, SomeIpSDHeader* someIpSDHeader) {
    int num2ndOption = subscribeEventGroupAckEntry->getNum1stAnd2ndOptions() & 0x0F;
    if (num2ndOption > 0) {
        IPv4EndpointOption* ipv4EndpointOption = dynamic_cast<IPv4EndpointOption*>(someIpSDHeader->decapOption());
        _someIpLSM->acknowledgeService(subscribeEventGroupAckEntry->getServiceID(), ipv4EndpointOption->getIpv4Address(), ipv4EndpointOption->getPort());
        delete ipv4EndpointOption;
    }
}

void SomeIpSD::subscribeService(uint16_t serviceID, uint16_t instanceID, inet::L3Address publisherIP, inet::L3Address subscriberIP, uint16_t subscriberPort) {
    subscribeEventgroup(serviceID, instanceID, publisherIP, subscriberIP, subscriberPort);
}

void SomeIpSD::findService(uint16_t serviceID, uint16_t instanceID) {
    find(serviceID, instanceID);
}

void SomeIpSD::acknowledgeSubscription(uint16_t serviceID, uint16_t instanceID, inet::L3Address publisherIP, uint16_t publisherPort, inet::L3Address remoteAddress) {
    subscribeEventgroupAck(serviceID, instanceID, publisherIP, publisherPort, remoteAddress);
}

} /* end namespace SOQoSMW */
