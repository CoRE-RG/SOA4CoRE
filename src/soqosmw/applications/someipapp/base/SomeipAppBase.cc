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

#include <soqosmw/applications/someipapp/base/SomeipAppBase.h>

#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/lifecycle/NodeOperations.h"
#include "inet/transportlayer/contract/udp/UDPControlInfo_m.h"
#include "soqosmw/messages/someip/SomeIpHeader_m.h"

namespace SOQoSMW {
Define_Module(SomeipAppBase);

SomeipAppBase::SomeipAppBase() {
    // TODO Auto-generated constructor stub

}

SomeipAppBase::~SomeipAppBase() {
    // TODO Auto-generated destructor stub
}

void SomeipAppBase:: initialize(int stage) {
    ApplicationBase::initialize(stage);
    if (stage == inet::INITSTAGE_LOCAL) {
        localPort = par("localPort");
        destPort = par("destPort");
        startTime = par("startTime").doubleValue();
        stopTime = par("stopTime").doubleValue();
        packetName = par("packetName");
        initialMsg = par("initialMsg");
        processStart();
    }
}

void SomeipAppBase::handleMessageWhenUp(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        EV << "Self message arrived" << std::endl;
    } else {
        EV << "Message arrived" << std::endl;
    }
    delete msg;
}

void SomeipAppBase::refreshDisplay() const {

}

void SomeipAppBase::sendPacket() {
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

void SomeipAppBase::processPacket(cPacket *pk) {
    EV_INFO << "Received packet: " << inet::UDPSocket::getReceivedPacketInfo(pk) << endl;
    delete pk;
}

void SomeipAppBase::processStart() {
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

void SomeipAppBase::processSend() {
    sendPacket();
}

void SomeipAppBase::finish() {
    ApplicationBase::finish();
}

bool SomeipAppBase::handleNodeStart(inet::IDoneCallback *doneCallback) {
    return true;
}

void SomeipAppBase::scheduleSelfMsg(omnetpp::simtime_t scheduleTime) {
    omnetpp::cMessage *msg = new omnetpp::cMessage("selfMsg");
    scheduleAt(simTime() + scheduleTime, msg);
}
} /* end namespace SOQoSMW */
