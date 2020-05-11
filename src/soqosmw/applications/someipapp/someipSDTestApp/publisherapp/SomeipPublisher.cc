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

#include "soqosmw/applications/someipapp/someipSDTestApp/publisherapp/SomeipPublisher.h"
#include "soqosmw/messages/someip/SomeIpHeader_m.h"

namespace SOQoSMW {

Define_Module(SomeipPublisher);

void SomeipPublisher::initialize(int stage) {
    SomeipAppBase::initialize(stage);
    if (stage == inet::INITSTAGE_LOCAL) {
        _publishServiceID = par("publishServiceID").intValue();
        _instanceID = par("instanceID").intValue();
        cModule* module = getParentModule()->getSubmodule("udpApp", SOMEIPLOCALSERVICEMANAGERIDX);
        if(_someipLSM = dynamic_cast<SomeipLocalServiceManager*>(module)) {
            _someipLSM->registerPublisherService(this);
        } else {
            throw cRuntimeError("Submodule at index %d is no Someip LSM app."
                    "Please place the SomeipLocalServiceManager at index %d", SOMEIPLOCALSERVICEMANAGERIDX);
        }
    }
}

void SomeipPublisher::handleMessageWhenUp(cMessage *msg) {
    SomeIpHeader* someipHeader = new SomeIpHeader("SOME/IP - RESPONSE");
    someipHeader->setMessageID(0x00000042);
    someipHeader->setLength(someipHeader->getByteLength()-UNCOVEREDBYTESBYLENGTH);
    someipHeader->setRequestID(0x00000042);
    someipHeader->setProtocolVersion(SOQoSMW::ProtocolVersion::PV_1);
    someipHeader->setInterfaceVersion(SOQoSMW::InterfaceVersion::IV_1);
    someipHeader->setMessageType(SOQoSMW::MessageType::RESPONSE);
    someipHeader->setReturnCode(SOQoSMW::ReturnCode::E_OK);
    socket.sendTo(someipHeader, _subscriberIpAddress, _subscriberPort);
    delete msg;
    SomeipAppBase::scheduleSelfMsg(omnetpp::SimTime(1,omnetpp::SIMTIME_MS));
}

void SomeipPublisher::processPacket(cPacket *packet) {

}

inet::L3Address SomeipPublisher::getIpAddress(inet::L3Address::AddressType addressType) {
    switch (addressType) {
        case L3Address::IPv4:
            return inet::IPv4Address(par("localAddress").stringValue());
            break;
        default:
            throw cRuntimeError("Unknown addresstype");
            break;
    }
}

int SomeipPublisher::getPort() {
    return localPort;
}

uint16_t SomeipPublisher::getPublishServiceID() {
    return _publishServiceID;
}

uint16_t SomeipPublisher::getInstanceID() {
    return _instanceID;
}

void SomeipPublisher::setSubscriberIpAddress(L3Address subscriberIpAddress) {
    _subscriberIpAddress = subscriberIpAddress;
}

void SomeipPublisher::setSubscriberPort(uint16_t port) {
    _subscriberPort = port;
}

void SomeipPublisher::startPublish() {
    Enter_Method("SomeipPublisher::startPublish");
    SomeipAppBase::scheduleSelfMsg(omnetpp::SimTime(1,omnetpp::SIMTIME_MS));
}

} /* end namespace SOQoSMW */
