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

#include <soqosmw/applications/publisherapp/someippublisherapp/SomeIpPublisher.h>
#include "soqosmw/messages/someip/SomeIpHeader_m.h"

namespace SOQoSMW {

Define_Module(SomeIpPublisher);

void SomeIpPublisher::initialize(int stage) {
    SomeIpAppBase::initialize(stage);
    if (stage == inet::INITSTAGE_LOCAL) {
        _publishServiceID = par("publishServiceID").intValue();
        _instanceID = par("instanceID").intValue();
        cModule* module = getParentModule()->getSubmodule("lsm");
        if((_someipLSM = dynamic_cast<SomeIpLocalServiceManager*>(module))) {
            _someipLSM->registerPublisherService(this);
        } else {
            throw cRuntimeError("No SOME/IP local service manager found.");
        }
        SomeIpAppBase::scheduleSelfMsg(omnetpp::SimTime(1,omnetpp::SIMTIME_MS));
    }
}

void SomeIpPublisher::handleMessageWhenUp(cMessage *msg) {
    if (!_destinations.empty()) {
        std::string headerName = "SOME/IP - RESPONSE - " + std::to_string(getPublishServiceID());
        SomeIpHeader* someipHeader = new SomeIpHeader(headerName.c_str());
        someipHeader->setMessageID(0x00000042);
        someipHeader->setLength(someipHeader->getByteLength()-UNCOVEREDBYTESBYLENGTH);
        someipHeader->setRequestID(0x00000042);
        someipHeader->setProtocolVersion(SOQoSMW::ProtocolVersion::PV_1);
        someipHeader->setInterfaceVersion(SOQoSMW::InterfaceVersion::IV_1);
        someipHeader->setMessageType(SOQoSMW::MessageType::RESPONSE);
        someipHeader->setReturnCode(SOQoSMW::ReturnCode::E_OK);
        for (std::pair<inet::L3Address,uint16_t> destination : _destinations) {
            socket.sendTo(someipHeader, destination.first, destination.second);
        }
    }
    delete msg;
    SomeIpAppBase::scheduleSelfMsg(omnetpp::SimTime(1,omnetpp::SIMTIME_MS));
}

void SomeIpPublisher::processPacket(cPacket *packet) {

}

inet::L3Address SomeIpPublisher::getIpAddress(inet::L3Address::AddressType addressType) {
    switch (addressType) {
        case L3Address::IPv4:
            return inet::IPv4Address(par("localAddress").stringValue());
            break;
        default:
            throw cRuntimeError("Unknown addresstype");
            break;
    }
}

int SomeIpPublisher::getPort() {
    return localPort;
}

uint16_t SomeIpPublisher::getPublishServiceID() {
    return _publishServiceID;
}

uint16_t SomeIpPublisher::getInstanceID() {
    return _instanceID;
}

void SomeIpPublisher::addSomeipSubscriberDestinationInformartion(inet::L3Address ipAddress, uint16_t port) {
    _destinations.push_back(std::make_pair(ipAddress,port));
}

bool SomeIpPublisher::operator==(SomeIpPublisher* other) {
    return this->_publishServiceID == other->getPublishServiceID()
            && this->getIpAddress(L3Address::IPv4) == other->getIpAddress(L3Address::IPv4)
            && this->getPort() == other->getPort();
}

bool SomeIpPublisher::operator!=(SomeIpPublisher* other) {
    return !(this == other);
}

} /* end namespace SOQoSMW */
