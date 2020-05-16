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

#include <soqosmw/applications/someipapp/base/SomeIpAppBase.h>

#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/lifecycle/NodeOperations.h"
#include "inet/transportlayer/contract/udp/UDPControlInfo_m.h"

namespace SOQoSMW {
Define_Module(SomeIpAppBase);

#define MESSAGEIDMETHODPATTERN 0xFFFF7FFF
#define MESSAGEIDEVENTPATTERN 0xFFFFFFFF

SomeIpAppBase::SomeIpAppBase() {
}

SomeIpAppBase::~SomeIpAppBase() {
}

void SomeIpAppBase:: initialize(int stage) {
    UDPBasicApp::initialize(stage);

    if (stage == inet::INITSTAGE_LOCAL) {
        processStart();
    }
}

void SomeIpAppBase::handleMessageWhenUp(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        EV << "Self message arrived" << std::endl;
    } else {
        EV << "Message arrived" << std::endl;
    }
    delete msg;
}

void SomeIpAppBase::refreshDisplay() const {

}

SomeIpHeader* SomeIpAppBase::encapsulatePayload(uint16_t serviceID, uint16_t method_EventID, uint8_t clientIDPrefix, uint8_t clientID, uint16_t sessionID,
        uint8_t protocolVersion, uint8_t interfaceVersion, uint8_t messageType, uint8_t returnCode,cPacket *payload) {
    SomeIpHeader *someipheader = new SomeIpHeader("SOME/IP");
    //Message ID ("Service ID" 16 Bit | "0" 1 Bit | "Method ID" 15 Bit) or
    //Message ID ("Service ID" 16 Bit | "1" 1 Bit | "Event ID" 15 Bit) or
    uint32_t messageID = 0;
    messageID = serviceID;
    messageID = messageID << 16;
    messageID = messageID | method_EventID;
    someipheader->setMessageID(messageID);
    // Length of whole SOME/IP Payload + header length in Payload.
    someipheader->setLength(HEADER_COVERED_BY_PAYLOADLENGTH + payload->getByteLength());
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
    someipheader->setProtocolVersion(protocolVersion);
    //Major version of the service interface
    someipheader->setInterfaceVersion(interfaceVersion);
    //The message type
    someipheader->setMessageType(messageType);
    //The return code
    someipheader->setReturnCode(returnCode);
    someipheader->encapsulate(payload);
    return someipheader;
}

void SomeIpAppBase::sendSomeIpPacket(SomeIpHeader* packet) {
    for (inet::L3Address destAddr : destAddresses) {
        socket.sendTo(packet, destAddr, destPort);
    }
}

void SomeIpAppBase::processStart() {
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

void SomeIpAppBase::processSend() {

}

bool SomeIpAppBase::handleNodeStart(inet::IDoneCallback *doneCallback) {
    return true;
}

void SomeIpAppBase::scheduleSelfMsg(omnetpp::simtime_t scheduleTime, short int kind) {
    omnetpp::cPacket *msg = new omnetpp::cPacket("selfMsg",kind);
    msg->setByteLength(8);
    scheduleAt(simTime() + scheduleTime, msg);
}
} /* end namespace SOQoSMW */
