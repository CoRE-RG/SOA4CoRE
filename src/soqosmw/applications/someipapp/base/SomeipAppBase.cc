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

void SomeipAppBase::sendPacket(uint16_t serviceID, uint16_t methodID, uint8_t clientIDPrefix, uint8_t clientID, uint16_t sessionID,
        uint8_t protocolVersion, uint8_t interfaceVersion, uint8_t messageType, uint8_t returnCode,cPacket *payload) {
    SomeIpHeader *someipheader = new SomeIpHeader("someip");
    //Message ID ("Service ID" 16 Bit | "0" 1 Bit | "Method ID" 15 Bit)
    uint32_t messageID = 0;
    uint32_t messageIDCorrectionPattern = 0xFFFF7FFF;
    messageID = serviceID;
    messageID = messageID << 16;
    messageID = messageID | methodID;
    messageID = messageID & messageIDCorrectionPattern;
    someipheader->setMessageID(messageID);
    // Length of whole SOME/IP Packet
    someipheader->setLength(8+payload->getByteLength());
    // Request ID ("Client ID Prefix" 8 Bit | "Client ID" 8 Bit | "Session ID" 16 Bit)
    uint32_t requestID = 0;
    requestID = clientIDPrefix;
    requestID = requestID << 24;
    uint16_t clientID16Bit = clientID;
    clientID16Bit = clientID16Bit << 16;
    requestID = requestID | clientID16Bit;
    requestID = requestID | sessionID;
    someipheader->setRequestID(requestID);
    //Version of SOME/IP Protocol
    someipheader->setProtcolVersion(protocolVersion);
    //Major version of the service interface
    someipheader->setInterfaceVersion(interfaceVersion);
    //The message type
    someipheader->setMessageType(messageType);
    //The return code
    someipheader->setReturnCode(returnCode);
    someipheader->encapsulate(payload);
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

}

void SomeipAppBase::finish() {
    ApplicationBase::finish();
}

bool SomeipAppBase::handleNodeStart(inet::IDoneCallback *doneCallback) {
    return true;
}

void SomeipAppBase::scheduleSelfMsg(omnetpp::simtime_t scheduleTime) {
    omnetpp::cPacket *msg = new omnetpp::cPacket("selfMsg");
    msg->setByteLength(8);
    scheduleAt(simTime() + scheduleTime, msg);
}
} /* end namespace SOQoSMW */
