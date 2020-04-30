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

#include <soqosmw/applications/someipapp/someipSDTestApp/subscriberapp/SomeipSubscriber.h>

namespace SOQoSMW {

Define_Module(SomeipSubscriber);

void SomeipSubscriber::initialize(int stage) {
    SomeipAppBase::initialize(stage);
    if (stage == inet::INITSTAGE_LOCAL) {
        _subscribeServiceID = par("subscribeServiceID").intValue();
        _instanceID = par("instanceID").intValue();
        cModule* module = getParentModule()->getSubmodule("udpApp", 1);
        _someipSD = dynamic_cast<SomeipSD*>(module);
        _someipSD->registerSubscriber(this);
        SomeipAppBase::scheduleSelfMsg(omnetpp::SimTime(1,omnetpp::SIMTIME_MS));
    }
}

void SomeipSubscriber::handleMessageWhenUp(cMessage *msg) {
    if (msg->isSelfMessage()) {
        _someipSD->find(getSubscribeServiceID(),getInstanceID());
    } else {
        EV << "Service message arrived" << std::endl;
    }
    delete msg;
}

void SomeipSubscriber::processPacket(cPacket *packet) {

}

inet::L3Address SomeipSubscriber::getIpAddress(L3Address::AddressType addressType) {
    switch (addressType) {
        case L3Address::IPv4:
            return inet::IPv4Address(par("localAddress").stringValue());
            break;
        default:
            throw cRuntimeError("Unknown addresstype");
            break;
    }
}

int SomeipSubscriber::getPort() {
    return localPort;
}

uint16_t SomeipSubscriber::getSubscribeServiceID() {
    return _subscribeServiceID;
}

uint16_t SomeipSubscriber::getInstanceID() {
    return _instanceID;
}

} /* end namespace SOQoSMW */
