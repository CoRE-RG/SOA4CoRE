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

#include <soqosmw/applications/subscriberapp/someip/SomeIpSubscriber.h>

namespace SOQoSMW {

Define_Module(SomeIpSubscriber);

void SomeIpSubscriber::initialize(int stage) {
    SomeIpAppBase::initialize(stage);
    if (stage == inet::INITSTAGE_LOCAL) {
        _subscribeServiceID = par("subscribeServiceID").intValue();
        _instanceID = par("instanceID").intValue();
        cModule* module = getParentModule()->getSubmodule("lsm");
        if ((_someIpLSM = dynamic_cast<SomeIpLocalServiceManager*>(module))) {
        } else {
            throw cRuntimeError("No SOME/IP local service manager found.");
        }
    } else if (stage == inet::INITSTAGE_PHYSICAL_ENVIRONMENT) {
        _someIpLSM->registerSubscriberService(this);
        _sendInterval = par("sendInterval");
        SomeIpAppBase::scheduleSelfMsg(_sendInterval);
    }
}

void SomeIpSubscriber::handleMessageWhenUp(cMessage *msg) {
    if (msg->isSelfMessage()) {
        _someIpLSM->discoverService(this);
    } else if (SomeIpHeader *someIpheader = dynamic_cast<SomeIpHeader*>(msg)){
        EV << "SomeIpHeader service message arrived" << std::endl;
    }
    delete msg;
}

void SomeIpSubscriber::processPacket(cPacket *packet) {

}

inet::L3Address SomeIpSubscriber::getIpAddress(L3Address::AddressType addressType) {
    switch (addressType) {
        case L3Address::IPv4:
            return inet::IPv4Address(par("localAddress").stringValue());
            break;
        default:
            throw cRuntimeError("Unknown addresstype");
            break;
    }
}

uint16_t SomeIpSubscriber::getPort() {
    return localPort;
}

uint16_t SomeIpSubscriber::getServiceID() {
    return _subscribeServiceID;
}

uint16_t SomeIpSubscriber::getInstanceID() {
    return _instanceID;
}

} /* end namespace SOQoSMW */
