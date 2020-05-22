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

TicTocApp::TicTocApp(){
}

TicTocApp::~TicTocApp(){
}

void TicTocApp:: initialize(int stage) {
    SomeIpAppBase::initialize(stage);
    if (stage == inet::INITSTAGE_LOCAL) {
        _initialMsg = par("initialMsg");
        _sendInterval = par("sendInterval");
        if (_initialMsg){
            SomeIpAppBase::scheduleSelfMsg(_sendInterval);
        }
    }
}

void TicTocApp::handleMessageWhenUp(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        EV << "Sending initial someip message" << std::endl;
        delete msg;
    } else {
        cPacket *packet = dynamic_cast<cPacket*>(msg);
        processPacket(packet);
        EV << "Sending someip message" << std::endl;
    }
    cPacket *payload = new cPacket("payload");
    payload->setByteLength(8);
    SomeIpHeader* packet = SomeIpAppBase::encapsulatePayload(0b1000000000000001, 0b1111111111111111, 0b10101010, 0b10100010, 0b0000000011111111,
            SOQoSMW::ProtocolVersion::PV_1, SOQoSMW::InterfaceVersion::IV_1, SOQoSMW::MessageType::REQUEST, SOQoSMW::ReturnCode::E_OK, payload);
    SomeIpAppBase::sendSomeIpPacket(packet);

}

void TicTocApp::processPacket(cPacket *packet) {
    cPacket *payload = packet->decapsulate();
    EV_INFO << "Received packet: " << packet->getName() << "    Length: " << packet->getByteLength() << endl;
    EV_INFO << "SOME/IP covered length: " << dynamic_cast<SomeIpHeader*>(packet)->getLength() << endl;
    EV_INFO << "Received payload: " << payload->getName() << "    Length: " << payload->getByteLength() << endl;
    delete payload;
    delete packet;
}

} /* end namespace SOQoSMW */
