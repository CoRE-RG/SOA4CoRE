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

#include <soqosmw/applications/someipbase/SomeipBaseApp.h>

#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/lifecycle/NodeOperations.h"
#include "inet/transportlayer/contract/udp/UDPControlInfo_m.h"
#include "soqosmw/messages/someip/SomeIpHeader_m.h"

namespace SOQoSMW {
Define_Module(SomeipBaseApp);

SomeipBaseApp::SomeipBaseApp() {
    // TODO Auto-generated constructor stub

}

SomeipBaseApp::~SomeipBaseApp() {
    // TODO Auto-generated destructor stub
}

void SomeipBaseApp:: initialize(int stage) {
    ApplicationBase::initialize(stage);
    if (stage == inet::INITSTAGE_LOCAL) {
           localPort = par("localPort");
           destPort = par("destPort");
           startTime = par("startTime").doubleValue();
           stopTime = par("stopTime").doubleValue();
           packetName = par("packetName");
           initialMsg = par("initialMsg");
           processStart();
           if (initialMsg){
               scheduleSelfMsg(omnetpp::SimTime(1,omnetpp::SIMTIME_MS));
           }
       }
}

void SomeipBaseApp::handleMessageWhenUp(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        EV << "Sending initial someip message" << std::endl;
    } else {
        EV << "Sending someip message" << std::endl;
    }
    processSend();
    delete msg;
}

void SomeipBaseApp::refreshDisplay() const {

}

void SomeipBaseApp::sendPacket() {
    SomeIpHeader *someipheader = new SomeIpHeader("someip");
    someipheader->setMessageID(42);
    someipheader->setLength(16);
    someipheader->setRequestID(24);
    someipheader->setProtcolVersion(1);
    someipheader->setInterfaceVersion(1);
    someipheader->setMessageType(255);
    someipheader->setReturnCode(0);
    for (inet::L3Address destAddr : destAddresses) {
        socket.sendTo(someipheader, destAddr, destPort);
    }
}

void SomeipBaseApp::processPacket(cPacket *pk) {
    EV_INFO << "Received packet: " << inet::UDPSocket::getReceivedPacketInfo(pk) << endl;
    delete pk;
}

void SomeipBaseApp::processStart() {
    socket.setOutputGate(gate("udpOut"));
    const char *localAddress = par("localAddress");
    socket.bind(*localAddress ? inet::L3AddressResolver().resolve(localAddress) : inet::L3Address(), localPort);
    UDPBasicApp::setSocketOptions();

    const char *destAddrs = par("destAddresses");
    cStringTokenizer tokenizer(destAddrs);
    const char *token;

    while ((token = tokenizer.nextToken()) != nullptr) {
        inet::L3Address result;
        inet::L3AddressResolver().tryResolve(token, result);
        if (result.isUnspecified())
            EV_ERROR << "cannot resolve destination address: " << token << endl;
        else
            destAddresses.push_back(result);
    }
}

void SomeipBaseApp::processSend() {
    sendPacket();
}

void SomeipBaseApp::finish() {
    ApplicationBase::finish();
}

bool SomeipBaseApp::handleNodeStart(inet::IDoneCallback *doneCallback) {
    return true;
}

void SomeipBaseApp::scheduleSelfMsg(omnetpp::simtime_t scheduleTime) {
    omnetpp::cMessage *msg = new omnetpp::cMessage("selfMsg");
    scheduleAt(simTime() + scheduleTime, msg);
}
}
