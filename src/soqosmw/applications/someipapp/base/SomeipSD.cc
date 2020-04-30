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

#include <soqosmw/applications/someipapp/someipSDTestApp/publisherapp/SomeipPublisher.h>
#include <soqosmw/applications/someipapp/someipSDTestApp/subscriberapp/SomeipSubscriber.h>
#include "soqosmw/applications/someipapp/base/SomeipSD.h"
#include "soqosmw/messages/someip/Entry_m.h"
#include "soqosmw/messages/someip/Option_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/networklayer/contract/ipv4/IPv4Address.h"
#include <list>

namespace SOQoSMW {

Define_Module(SomeipSD);

void SomeipSD::initialize(int stage) {
    SomeipAppBase::initialize(stage);
    _localAddress = par("localAddress").stringValue();
}

void SomeipSD::handleMessageWhenUp(cMessage *msg) {
    SomeIpSDHeader *someipSDHeader = nullptr;
    if ((someipSDHeader = dynamic_cast<SomeIpSDHeader*>(msg))) {
        processPacket(someipSDHeader);
        delete someipSDHeader;
    } else {
        delete msg;
    }

}

void SomeipSD::find(uint16_t serviceID, uint16_t instanceID) {
    Enter_Method("SomeipSD::find");
    SomeIpSDHeader *someipSDHeader = new SomeIpSDHeader("SOME/IP SD - FIND");

    ServiceEntry *findEntry = new ServiceEntry("ServiceEntry");
    findEntry->setType(SOQoSMW::EntryType::FIND);
    findEntry->setIndex1stOptions(0x00);
    findEntry->setIndex2ndOptions(0x00);
    findEntry->setNum1stAnd2ndOptions(0x10);
    findEntry->setServiceID(serviceID);
    findEntry->setInstanceID(instanceID);
    findEntry->setMajorVersion_TTL(0x01000003);
    findEntry->setMinorVersion(0xFFFFFFFF);
    someipSDHeader->encapEntry(findEntry);
    someipSDHeader->setEntriesLength(findEntry->getByteLength());

    IPv4SDEndpointOption *ipv4SDEndpointOption = new IPv4SDEndpointOption("IPv4SDEndpointOption");
    ipv4SDEndpointOption->setIpv4Address(inet::IPv4Address(_localAddress));
    ipv4SDEndpointOption->setL4Protocol(IPProtocolId::IP_PROT_UDP);
    ipv4SDEndpointOption->setPort(localPort);
    someipSDHeader->encapOption(ipv4SDEndpointOption);
    someipSDHeader->setOptionsLength(ipv4SDEndpointOption->getByteLength());

    someipSDHeader->setLength(someipSDHeader->getByteLength()-UNCOVEREDBYTESBYLENGTH);

    socket.sendTo(someipSDHeader, inet::IPv4Address(BROADCASTADDRESS), destPort);
}

void SomeipSD::offer(uint16_t serviceID, uint16_t instanceID, L3Address remoteAddress, int remotePort) {
    Enter_Method("SomeipSD::offer");
    SomeIpSDHeader *someipSDHeader = new SomeIpSDHeader("SOME/IP SD - OFFER");

    ServiceEntry *offerEntry = new ServiceEntry("ServiceEntry");
    offerEntry->setType(SOQoSMW::EntryType::OFFER);
    offerEntry->setIndex1stOptions(0x00);
    offerEntry->setIndex2ndOptions(0x00);
    offerEntry->setNum1stAnd2ndOptions(0x10);
    offerEntry->setServiceID(serviceID);
    offerEntry->setInstanceID(instanceID);
    offerEntry->setMajorVersion_TTL(0x01000003);
    offerEntry->setMinorVersion(0xFFFFFFFF);
    someipSDHeader->encapEntry(offerEntry);
    someipSDHeader->setEntriesLength(offerEntry->getByteLength());

    IPv4SDEndpointOption *ipv4SDEndpointOption = new IPv4SDEndpointOption("IPv4SDEndpointOption");
    ipv4SDEndpointOption->setIpv4Address(inet::IPv4Address(_localAddress));
    ipv4SDEndpointOption->setL4Protocol(IPProtocolId::IP_PROT_UDP);
    ipv4SDEndpointOption->setPort(localPort);
    someipSDHeader->encapOption(ipv4SDEndpointOption);
    someipSDHeader->setOptionsLength(ipv4SDEndpointOption->getByteLength());

    someipSDHeader->setLength(someipSDHeader->getByteLength()-UNCOVEREDBYTESBYLENGTH);

    socket.sendTo(someipSDHeader, remoteAddress, remotePort);
}

void SomeipSD::subscribeEventgroup(uint16_t serviceID, uint16_t instanceID, L3Address remoteAddress, int remotePort) {
    Enter_Method("SomeipSD::subscribeEventgroup");
    SomeIpSDHeader *someipSDHeader = new SomeIpSDHeader("SOME/IP SD - SUBSCRIBEEVENTGROUP");

    EventgroupEntry *subscribeEventgroupEntry = new EventgroupEntry("EventgroupEntry");
    subscribeEventgroupEntry->setType(SOQoSMW::EntryType::SUBSCRIBEVENTGROUP);
    subscribeEventgroupEntry->setIndex1stOptions(0);
    subscribeEventgroupEntry->setIndex2ndOptions(1);
    subscribeEventgroupEntry->setNum1stAnd2ndOptions(0x11);
    subscribeEventgroupEntry->setServiceID(serviceID);
    subscribeEventgroupEntry->setInstanceID(instanceID);
    subscribeEventgroupEntry->setMajorVersion_TTL(0x01000003);
    someipSDHeader->encapEntry(subscribeEventgroupEntry);
    someipSDHeader->setEntriesLength(subscribeEventgroupEntry->getByteLength());

    IPv4EndpointOption *ipv4EndpointOption = new IPv4EndpointOption("IPv4EndpointOption");
    ipv4EndpointOption->setIpv4Address(_someipSubscriber->getIpAddress(inet::L3Address::IPv4).toIPv4());
    ipv4EndpointOption->setL4Protocol(IPProtocolId::IP_PROT_UDP);
    ipv4EndpointOption->setPort(_someipSubscriber->getPort());
    someipSDHeader->encapOption(ipv4EndpointOption);
    someipSDHeader->setOptionsLength(ipv4EndpointOption->getByteLength());

    IPv4SDEndpointOption *ipv4SDEndpointOption = new IPv4SDEndpointOption("IPv4SDEndpointOption");
    ipv4SDEndpointOption->setIpv4Address(inet::IPv4Address(_localAddress));
    ipv4SDEndpointOption->setL4Protocol(IPProtocolId::IP_PROT_UDP);
    ipv4SDEndpointOption->setPort(localPort);
    someipSDHeader->encapOption(ipv4SDEndpointOption);
    someipSDHeader->setOptionsLength(someipSDHeader->getOptionsLength() + ipv4SDEndpointOption->getByteLength());

    someipSDHeader->setLength(someipSDHeader->getByteLength()-UNCOVEREDBYTESBYLENGTH);

    socket.sendTo(someipSDHeader, remoteAddress, remotePort);
}

void SomeipSD::subscribeEventgroupAck(uint16_t serviceID, uint16_t instanceID, L3Address remoteAddress, int remotePort) {
    Enter_Method("SomeipSD::subscribeEventgroupAck");
    SomeIpSDHeader *someipSDHeader = new SomeIpSDHeader("SOME/IP SD - SUBSCRIBEEVENTGROUPACK");

    EventgroupEntry *subscribeEventgroupAckEntry = new EventgroupEntry("EventgroupEntry");
    subscribeEventgroupAckEntry->setType(SOQoSMW::EntryType::SUBSCRIBEVENTGROUPACK);
    subscribeEventgroupAckEntry->setIndex1stOptions(0);
    subscribeEventgroupAckEntry->setIndex2ndOptions(0);
    subscribeEventgroupAckEntry->setNum1stAnd2ndOptions(0);
    subscribeEventgroupAckEntry->setServiceID(serviceID);
    subscribeEventgroupAckEntry->setInstanceID(instanceID);
    subscribeEventgroupAckEntry->setMajorVersion_TTL(0x01000003);
    someipSDHeader->encapEntry(subscribeEventgroupAckEntry);
    someipSDHeader->setEntriesLength(subscribeEventgroupAckEntry->getByteLength());

    IPv4EndpointOption *ipv4EndpointOption = new IPv4EndpointOption("IPv4EndpointOption");
    ipv4EndpointOption->setIpv4Address(_someipPublisher->getIpAddress(inet::L3Address::IPv4).toIPv4());
    ipv4EndpointOption->setL4Protocol(IPProtocolId::IP_PROT_UDP);
    ipv4EndpointOption->setPort(_someipPublisher->getPort());
    someipSDHeader->encapOption(ipv4EndpointOption);
    someipSDHeader->setOptionsLength(ipv4EndpointOption->getByteLength());

    someipSDHeader->setLength(someipSDHeader->getByteLength()-UNCOVEREDBYTESBYLENGTH);

    socket.sendTo(someipSDHeader, remoteAddress, remotePort);
}

void SomeipSD::registerPublisher(SomeipPublisher* someipPublisher) {
    _someipPublisher = someipPublisher;
}

void SomeipSD::registerSubscriber(SomeipSubscriber* someipSubscriber) {
    _someipSubscriber = someipSubscriber;
}

void SomeipSD::processPacket(SomeIpSDHeader* someipSDHeader) {
    std::list<Entry*> entries = someipSDHeader->getEncapEntries();
    for (std::list<Entry*>::const_iterator it = entries.begin(); it != entries.end(); ++it) {
        switch ((*it)->getType()) {
            case SOQoSMW::EntryType::FIND:
                EV << "FIND ARRIVED" << endl;
                processFindEntry(*it, someipSDHeader);
                break;
            case SOQoSMW::EntryType::OFFER:
                EV << "OFFER ARRIVED" << endl;
                processOfferEntry(*it, someipSDHeader);
                break;
            case SOQoSMW::EntryType::SUBSCRIBEVENTGROUP:
                EV << "SUBSCRIBEVENTGROUP ARRIVED" << endl;
                processSubscribeEventGroupEntry(*it, someipSDHeader);
                break;
            case SOQoSMW::EntryType::SUBSCRIBEVENTGROUPACK:
                EV << "SUBSCRIBEVENTGROUPACK ARRIVED" << endl;
                processSubscribeEventGroupAckEntry(*it, someipSDHeader);
                break;
            default:
                EV << "Unknown type" << std::endl;
        }
    }
}


void SomeipSD::processFindEntry(Entry* findEntry, SomeIpSDHeader* someipSDHeader) {
    if (_someipPublisher && findEntry->getServiceID() == _someipPublisher->getPublishServiceID()) {
        int num1stOption = (findEntry->getNum1stAnd2ndOptions() & 0xF0) >> 4;
        if (num1stOption > 0) {
            IPv4SDEndpointOption* ipv4SDEndpointOption = dynamic_cast<IPv4SDEndpointOption*>(someipSDHeader->decapOption());
            offer(findEntry->getServiceID(), findEntry->getInstanceID(), ipv4SDEndpointOption->getIpv4Address(), ipv4SDEndpointOption->getPort());
        }
    }
}

void SomeipSD::processOfferEntry(Entry* offerEntry, SomeIpSDHeader* someipSDHeader) {
    if (offerEntry->getServiceID() == _someipSubscriber->getSubscribeServiceID()) {
        int num1stOption = (offerEntry->getNum1stAnd2ndOptions() & 0xF0) >> 4;
        if (num1stOption > 0) {
            IPv4SDEndpointOption* ipv4SDEndpointOption = dynamic_cast<IPv4SDEndpointOption*>(someipSDHeader->decapOption());
            subscribeEventgroup(offerEntry->getServiceID(), offerEntry->getInstanceID(), ipv4SDEndpointOption->getIpv4Address(), ipv4SDEndpointOption->getPort());
        }
    }
}

void SomeipSD::processSubscribeEventGroupEntry(Entry* subscribeEventGroupEntry, SomeIpSDHeader* someipSDHeader) {
    if (subscribeEventGroupEntry->getServiceID() == _someipPublisher->getPublishServiceID()) {
        int num1stOption = (subscribeEventGroupEntry->getNum1stAnd2ndOptions() & 0xF0) >> 4;
        if (num1stOption > 0) {
            IPv4EndpointOption* ipv4EndpointOption = dynamic_cast<IPv4EndpointOption*>(someipSDHeader->decapOption());
            _someipPublisher->setSubscriberIpAddress(ipv4EndpointOption->getIpv4Address());
            _someipPublisher->setSubscriberPort(ipv4EndpointOption->getPort());
        }
        int num2ndOption = subscribeEventGroupEntry->getNum1stAnd2ndOptions() & 0x0F;
        if (num2ndOption > 0) {
            IPv4SDEndpointOption* ipv4SDEndpointOption = dynamic_cast<IPv4SDEndpointOption*>(someipSDHeader->decapOption());
            subscribeEventgroupAck(subscribeEventGroupEntry->getServiceID(), subscribeEventGroupEntry->getInstanceID(), ipv4SDEndpointOption->getIpv4Address(), ipv4SDEndpointOption->getPort());
        }
        _someipPublisher->startPublish();
    }
}

void SomeipSD::processSubscribeEventGroupAckEntry(Entry *subscribeEventGroupAckEntry, SomeIpSDHeader* someipSDHeader) {
    if (subscribeEventGroupAckEntry->getServiceID() == _someipSubscriber->getSubscribeServiceID()) {
        EV << "Good Acknowledge arrived" << std::endl;
    } else {
        EV << "Bad Acknowledge arrived, service ids differ. Arrived id:" << subscribeEventGroupAckEntry->getServiceID() << " Expected id:" << _someipSubscriber->getSubscribeServiceID() << std::endl;
    }
}

} /* end namespace SOQoSMW */
