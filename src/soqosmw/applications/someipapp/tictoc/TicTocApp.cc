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

#include <soqosmw/applications/someipapp/tictoc/TicTocApp.h>

namespace SOQoSMW {
Define_Module(TicTocApp);

TicTocApp::TicTocApp() {
    // TODO Auto-generated constructor stub

}

TicTocApp::~TicTocApp() {
    // TODO Auto-generated destructor stub
}

void TicTocApp:: initialize(int stage) {
    SomeipAppBase::initialize(stage);
    if (stage == inet::INITSTAGE_LOCAL) {
        if (initialMsg){
            SomeipAppBase::scheduleSelfMsg(omnetpp::SimTime(1,omnetpp::SIMTIME_MS));
        }
    }
}

void TicTocApp::handleMessageWhenUp(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        EV << "Sending initial someip message" << std::endl;
    } else {
        EV << "Sending someip message" << std::endl;
    }
    delete msg;
    SomeipAppBase::sendPacket(0b1000000000000001, 0b1111111111111111, 42, 0b1010101010100010, 0b0000000011111111,
            SOQoSMW::ProtocolVersion::V_1, 42, 42, SOQoSMW::ReturnCode::E_OK);
}
} /* end namespace SOQoSMW */
