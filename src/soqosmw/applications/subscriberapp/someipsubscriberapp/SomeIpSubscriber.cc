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

#include <soqosmw/applications/subscriberapp/someipsubscriberapp/SomeIpSubscriber.h>

namespace SOQoSMW {

Define_Module(SomeIpSubscriber);

void SomeIpSubscriber::initialize(int stage) {
    SomeIpAppBase::initialize(stage);
    if (stage == inet::INITSTAGE_LOCAL) {
        _subscribeServiceID = par("subscribeServiceID").intValue();
        _instanceID = par("instanceID").intValue();
        cModule* module = getParentModule()->getSubmodule("lsm");
        if ((_someIpLSM = dynamic_cast<SomeIpLocalServiceManager*>(module))) {
            _someIpLSM->registerSubscriberService(this);
            SomeIpAppBase::scheduleSelfMsg(omnetpp::SimTime(1,omnetpp::SIMTIME_MS));
        } else {
            throw cRuntimeError("No SOME/IP local service manager found.");
        }
    }
}

void SomeIpSubscriber::handleMessageWhenUp(cMessage *msg) {
    if (msg->isSelfMessage()) {
        _someIpLSM->discoverService(_subscribeServiceID,_instanceID, getIpAddress(L3Address::IPv4), localPort);
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

int SomeIpSubscriber::getPort() {
    return localPort;
}

uint16_t SomeIpSubscriber::getSubscribeServiceID() {
    return _subscribeServiceID;
}

uint16_t SomeIpSubscriber::getInstanceID() {
    return _instanceID;
}

bool SomeIpSubscriber::operator==(SomeIpSubscriber* other) {
    return this->_subscribeServiceID == other->getSubscribeServiceID()
            && this->getIpAddress(L3Address::IPv4) == other->getIpAddress(L3Address::IPv4)
            && this->getPort() == other->getPort();
}

bool SomeIpSubscriber::operator!=(SomeIpSubscriber* other) {
    return !(this == other);
}

} /* end namespace SOQoSMW */
