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

#include "soqosmw/applications/someipapp/base/SomeipSD.h"
#include "soqosmw/applications/someipapp/dummySD/publisherapp/SomeipPublisher.h"
#include "soqosmw/applications/someipapp/dummySD/subscriberapp/SomeipSubscriber.h"
#include "soqosmw/messages/someip/Entry_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/networklayer/contract/ipv4/IPv4Address.h"
#include <vector>

namespace SOQoSMW {

Define_Module(SomeipSD);

void SomeipSD::initialize(int stage) {
    SomeipAppBase::initialize(stage);
}

void SomeipSD::handleMessageWhenUp(cMessage *msg) {
    SomeIpSDHeader *someipSDHeader = dynamic_cast<SomeIpSDHeader*>(msg);
    if (!someipSDHeader) {
        delete msg;
    } else {
        processPacket(someipSDHeader);
        delete someipSDHeader;
    }

}

void SomeipSD::find(uint16_t serviceID, uint16_t instanceID) {
    Enter_Method("SomeipSD::find");
    SomeIpSDHeader *someipSDHeader = new SomeIpSDHeader("SOME/IP SD - FIND");
    std::vector<Entry> entryList;

    ServiceEntry findEntry;
    findEntry.setType(SOQoSMW::EntryType::FIND);
    findEntry.setIndex1stOptions(0);
    findEntry.setIndex2ndOptions(0);
    findEntry.setNum1stAnd2ndOptions(0);
    findEntry.setServiceID(serviceID);
    findEntry.setInstanceID(instanceID);
    findEntry.setMajorVersion_TTL(0x01000003);
    findEntry.setMinorVersion(0xFFFFFFFF);

    entryList.push_back(findEntry);
    someipSDHeader->setEntries(entryList);
    someipSDHeader->setEntriesLength(SERVICEENTRYLENGTH);

    someipSDHeader->setByteLength(
            someipSDHeader->getByteLength() // SOME/IP Header length - 16 Byte (byteLength untouched before!)
            +MIN_SOMEIPSDHEADERBYTESIZE     // Minimal SOME/IP SD HEADER length - 12 Byte
            +SERVICEENTRYLENGTH);           // Service entry added before
    someipSDHeader->setLength(someipSDHeader->getByteLength()-UNCOVEREDBYTESBYLENGTH);

    socket.setBroadcast(true);
    socket.sendTo(someipSDHeader, inet::IPv4Address(BROADCASTADDRESS), destPort);
}

void SomeipSD::registerPublisher(SomeipPublisher* someipPublisher) {
    _someipPublisher = someipPublisher;
}

void SomeipSD::registerSubscriber(SomeipSubscriber* someipSubscriber) {
    _someipSubscriber = someipSubscriber;
}

void SomeipSD::processPacket(SomeIpSDHeader* someipSDHeader) {
    std::vector<Entry> entries = someipSDHeader->getEntries();
    for (auto it = entries.begin(); it != entries.end(); it++) {
        switch (it.base()->getType()) {
            case SOQoSMW::EntryType::FIND:
                EV << "FIND ARRIVED" << endl;
                break;
            case SOQoSMW::EntryType::OFFER:
                EV << "OFFER ARRIVED" << endl;
                break;
            case SOQoSMW::EntryType::SUBSCRIBEVENTGROUP:
                EV << "SUBSCRIBEVENTGROUP ARRIVED" << endl;
                break;
            case SOQoSMW::EntryType::SUBSCRIBEVENTGROUPACK:
                EV << "SUBSCRIBEVENTGROUPACK ARRIVED" << endl;
                break;
            default:
                EV << "Unknown type" << std::endl;
        }
    }
}

} /* end namespace SOQoSMW */
