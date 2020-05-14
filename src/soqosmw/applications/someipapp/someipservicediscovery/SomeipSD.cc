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

#include "soqosmw/applications/someipapp/someipservicediscovery/SomeipSD.h"
#include "soqosmw/applications/someipapp/someipSDTestApp/publisherapp/SomeipPublisher.h"
#include "soqosmw/applications/someipapp/someipSDTestApp/subscriberapp/SomeipSubscriber.h"
#include "soqosmw/applications/someipapp/someipservicemanager/SomeipLocalServiceManager.h"
#include <inet/networklayer/common/L3AddressResolver.h>
#include <inet/networklayer/contract/ipv4/IPv4Address.h>
#include <list>

namespace SOQoSMW {

Define_Module(SomeipSD);

void SomeipSD::initialize(int stage) {
    SomeipAppBase::initialize(stage);
    _localAddress = par("localAddress").stringValue();
    cModule* module = getParentModule()->getSubmodule("udpApp", SOMEIPLOCALSERVICEMANAGERIDX);
    if((_someipLSM = dynamic_cast<SomeipLocalServiceManager*>(module))) {
    } else {
        throw cRuntimeError("Submodule at index %d is no Someip LSM app."
                "Please place the SomeipLocalServiceManager at index %d", SOMEIPLOCALSERVICEMANAGERIDX);
    }
}

void SomeipSD::handleMessageWhenUp(cMessage *msg) {
    if (SomeIpSDHeader *someipSDHeader = dynamic_cast<SomeIpSDHeader*>(msg)) {
        if(inet::UDPDataIndication *udpDataIndication = dynamic_cast<inet::UDPDataIndication*>(someipSDHeader->getControlInfo())) {
            if (udpDataIndication->getSrcAddr() != inet::IPv4Address(_localAddress)) {
                processSomeipSDHeader(someipSDHeader);
            }
        }
    }
    delete msg;
}

void SomeipSD::find(uint16_t serviceID, uint16_t instanceID) {
    Enter_Method("SomeipSD::find");
    SomeIpSDHeader *someipSDHeader = new SomeIpSDHeader("SOME/IP SD - FIND");

    ServiceEntry *findEntry = new ServiceEntry("ServiceEntry");
    findEntry->setType(SOQoSMW::SomeIpSDEntryType::FIND);
    findEntry->setIndex1stOptions(0x00);
    findEntry->setIndex2ndOptions(0x00);
    findEntry->setNum1stAnd2ndOptions(0x00);
    findEntry->setServiceID(serviceID);
    findEntry->setInstanceID(instanceID);
    findEntry->setMajorVersion_TTL(0x01000003);
    findEntry->setMinorVersion(0xFFFFFFFF);
    someipSDHeader->encapEntry(findEntry);

    socket.sendTo(someipSDHeader, inet::IPv4Address(BROADCASTADDRESS), destPort);
}

void SomeipSD::offer(uint16_t serviceID, uint16_t instanceID, inet::L3Address remoteAddress, inet::L3Address publisherIP, uint16_t publisherPort) {
    Enter_Method("SomeipSD::offer");
    SomeIpSDHeader *someipSDHeader = new SomeIpSDHeader("SOME/IP SD - OFFER");

    ServiceEntry *offerEntry = new ServiceEntry("ServiceEntry");
    offerEntry->setType(SOQoSMW::SomeIpSDEntryType::OFFER);
    offerEntry->setIndex1stOptions(0x00);
    offerEntry->setIndex2ndOptions(0x00);
    offerEntry->setNum1stAnd2ndOptions(0x01);
    offerEntry->setServiceID(serviceID);
    offerEntry->setInstanceID(instanceID);
    offerEntry->setMajorVersion_TTL(0x01000003);
    offerEntry->setMinorVersion(0xFFFFFFFF);
    someipSDHeader->encapEntry(offerEntry);

    IPv4EndpointOption *ipv4EndpointOption = new IPv4EndpointOption("IPv4EndpointOption");
    ipv4EndpointOption->setIpv4Address(publisherIP.toIPv4());
    ipv4EndpointOption->setL4Protocol(IPProtocolId::IP_PROT_UDP);
    ipv4EndpointOption->setPort(publisherPort);
    someipSDHeader->encapOption(ipv4EndpointOption);

    socket.sendTo(someipSDHeader, remoteAddress, destPort);
}

void SomeipSD::subscribeEventgroup(uint16_t serviceID, uint16_t instanceID, inet::L3Address remoteAddress, inet::L3Address subscriberIP, uint16_t subscriberPort) {
    Enter_Method("SomeipSD::subscribeEventgroup");
    SomeIpSDHeader *someipSDHeader = new SomeIpSDHeader("SOME/IP SD - SUBSCRIBEEVENTGROUP");

    EventgroupEntry *subscribeEventgroupEntry = new EventgroupEntry("EventgroupEntry");
    subscribeEventgroupEntry->setType(SOQoSMW::SomeIpSDEntryType::SUBSCRIBEVENTGROUP);
    subscribeEventgroupEntry->setIndex1stOptions(0);
    subscribeEventgroupEntry->setIndex2ndOptions(0);
    subscribeEventgroupEntry->setNum1stAnd2ndOptions(0x01);
    subscribeEventgroupEntry->setServiceID(serviceID);
    subscribeEventgroupEntry->setInstanceID(instanceID);
    subscribeEventgroupEntry->setMajorVersion_TTL(0x01000003);
    someipSDHeader->encapEntry(subscribeEventgroupEntry);

    IPv4EndpointOption *ipv4EndpointOption = new IPv4EndpointOption("IPv4EndpointOption");
    ipv4EndpointOption->setIpv4Address(subscriberIP.toIPv4());
    ipv4EndpointOption->setL4Protocol(IPProtocolId::IP_PROT_UDP);
    ipv4EndpointOption->setPort(subscriberPort);
    someipSDHeader->encapOption(ipv4EndpointOption);

    socket.sendTo(someipSDHeader, remoteAddress, destPort);
}

void SomeipSD::subscribeEventgroupAck(uint16_t serviceID, uint16_t instanceID, L3Address remoteAddress) {
    Enter_Method("SomeipSD::subscribeEventgroupAck");
    SomeIpSDHeader *someipSDHeader = new SomeIpSDHeader("SOME/IP SD - SUBSCRIBEEVENTGROUPACK");

    EventgroupEntry *subscribeEventgroupAckEntry = new EventgroupEntry("EventgroupEntry");
    subscribeEventgroupAckEntry->setType(SOQoSMW::SomeIpSDEntryType::SUBSCRIBEVENTGROUPACK);
    subscribeEventgroupAckEntry->setIndex1stOptions(0);
    subscribeEventgroupAckEntry->setIndex2ndOptions(0);
    subscribeEventgroupAckEntry->setNum1stAnd2ndOptions(0x00);
    subscribeEventgroupAckEntry->setServiceID(serviceID);
    subscribeEventgroupAckEntry->setInstanceID(instanceID);
    subscribeEventgroupAckEntry->setMajorVersion_TTL(0x01000003);
    someipSDHeader->encapEntry(subscribeEventgroupAckEntry);

    socket.sendTo(someipSDHeader, remoteAddress, destPort);
}

void SomeipSD::processSomeipSDHeader(SomeIpSDHeader* someipSDHeader) {
    std::list<SomeIpSDEntry*> entries = someipSDHeader->getEncapEntries();
    for (std::list<SomeIpSDEntry*>::const_iterator it = entries.begin(); it != entries.end(); ++it) {
        switch ((*it)->getType()) {
            case SOQoSMW::SomeIpSDEntryType::FIND:
                EV << "FIND ARRIVED" << endl;
                processFindEntry(*it, someipSDHeader);
                break;
            case SOQoSMW::SomeIpSDEntryType::OFFER:
                EV << "OFFER ARRIVED" << endl;
                processOfferEntry(*it, someipSDHeader);
                break;
            case SOQoSMW::SomeIpSDEntryType::SUBSCRIBEVENTGROUP:
                EV << "SUBSCRIBEVENTGROUP ARRIVED" << endl;
                processSubscribeEventGroupEntry(*it, someipSDHeader);
                break;
            case SOQoSMW::SomeIpSDEntryType::SUBSCRIBEVENTGROUPACK:
                EV << "SUBSCRIBEVENTGROUPACK ARRIVED" << endl;
                processSubscribeEventGroupAckEntry(*it, someipSDHeader);
                break;
            default:
                EV << "Unknown type" << std::endl;
        }
    }
}


void SomeipSD::processFindEntry(SomeIpSDEntry* findEntry, SomeIpSDHeader* someipSDHeader) {
    std::list<SomeipPublisher*> publisherList = _someipLSM->lookForPublisherService(findEntry->getServiceID());
    if (!publisherList.empty()) {
        inet::UDPDataIndication *udpDataIndication = dynamic_cast<inet::UDPDataIndication*>(someipSDHeader->getControlInfo());
        for (SomeipPublisher *publisher : publisherList) {
            offer(findEntry->getServiceID(), findEntry->getInstanceID(), udpDataIndication->getSrcAddr(),
                    publisher->getIpAddress(inet::L3Address::AddressType::IPv4), publisher->getPort());
        }
    }
}

void SomeipSD::processOfferEntry(SomeIpSDEntry* offerEntry, SomeIpSDHeader* someipSDHeader) {
    int num2ndOption = offerEntry->getNum1stAnd2ndOptions() & 0x0F;
    if (num2ndOption > 0) {
        //TODO if more than 1 publisher
        IPv4EndpointOption* ipv4EndpointOption = dynamic_cast<IPv4EndpointOption*>(someipSDHeader->decapOption());
        _someipLSM->addRemotePublisher(offerEntry->getServiceID(), ipv4EndpointOption->getIpv4Address(), ipv4EndpointOption->getPort());
        delete ipv4EndpointOption;
    }
}

void SomeipSD::processSubscribeEventGroupEntry(SomeIpSDEntry* subscribeEventGroupEntry, SomeIpSDHeader* someipSDHeader) {
    int num2ndOption = subscribeEventGroupEntry->getNum1stAnd2ndOptions() & 0x0F;
    if (num2ndOption > 0) {
        IPv4EndpointOption* ipv4EndpointOption = dynamic_cast<IPv4EndpointOption*>(someipSDHeader->decapOption());
        _someipLSM->publishToSubscriber(subscribeEventGroupEntry->getServiceID(), ipv4EndpointOption->getIpv4Address(), ipv4EndpointOption->getPort());
        delete ipv4EndpointOption;
    }
}

void SomeipSD::processSubscribeEventGroupAckEntry(SomeIpSDEntry *subscribeEventGroupAckEntry, SomeIpSDHeader* someipSDHeader) {
    _someipLSM->acknowledgeService(subscribeEventGroupAckEntry->getServiceID());
    EV << "Acknowledge for service id: " << subscribeEventGroupAckEntry->getServiceID() << " successful" << std::endl;
}

void SomeipSD::subscribeService(uint16_t serviceID, uint16_t instanceID, inet::L3Address publisherIP, inet::L3Address subscriberIP, uint16_t subscriberPort) {
    subscribeEventgroup(serviceID, instanceID, publisherIP, subscriberIP, subscriberPort);
}

void SomeipSD::findService(uint16_t serviceID, uint16_t instanceID) {
    find(serviceID, instanceID);
}

void SomeipSD::acknowledgeSubscription(uint16_t serviceID, uint16_t instanceID, inet::L3Address remoteAddress) {
    subscribeEventgroupAck(serviceID, instanceID, remoteAddress);
}

} /* end namespace SOQoSMW */
