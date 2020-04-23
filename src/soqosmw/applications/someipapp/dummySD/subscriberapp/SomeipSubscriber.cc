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

#include <soqosmw/applications/someipapp/dummySD/subscriberapp/SomeipSubscriber.h>

namespace SOQoSMW {

Define_Module(SomeipSubscriber);

void SomeipSubscriber::initialize(int stage) {
    SomeipAppBase::initialize(stage);
    if (stage == inet::INITSTAGE_LOCAL) {
        cModule* module = getParentModule()->getSubmodule("udpApp", 1);
        _someipSD = dynamic_cast<SomeipSD*>(module);
        _someipSD->registerSubscriber(this);
        SomeipAppBase::scheduleSelfMsg(omnetpp::SimTime(1,omnetpp::SIMTIME_MS));
    }
}

void SomeipSubscriber::handleMessageWhenUp(cMessage *msg) {
    _someipSD->find(0x0042,0xFFFF);
    delete msg;
}

void SomeipSubscriber::processPacket(cPacket *packet) {

}

} /* end namespace SOQoSMW */
