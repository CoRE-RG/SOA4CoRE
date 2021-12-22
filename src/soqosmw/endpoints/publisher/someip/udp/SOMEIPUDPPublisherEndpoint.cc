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

#include "SOMEIPUDPPublisherEndpoint.h"
#include "soqosmw/messages/someip/SomeIpHeader_m.h"

namespace SOQoSMW {

Define_Module(SOMEIPUDPPublisherEndpoint);

ConnectionSpecificInformation* SOMEIPUDPPublisherEndpoint::getConnectionSpecificInformation() {
    CSI_SOMEIP* connection = new CSI_SOMEIP();
    connection->setAddress(_localAddress.c_str());
    connection->setPort(_localPort);
    return connection;
}

void SOMEIPUDPPublisherEndpoint::publish(cPacket* msg) {
    if(_isConnected) {
        sendPacket(0b1000000000000001, 0b1111111111111111, 0b10101010, 0b10100010, 0b0000000011111111,
                SOQoSMW::ProtocolVersion::PV_1, 42, SOQoSMW::MessageType::REQUEST, SOQoSMW::ReturnCode::E_OK, msg->dup());
    }
}

void SOMEIPUDPPublisherEndpoint::sendPacket(uint16_t serviceID, uint16_t method_EventID, uint8_t clientIDPrefix, uint8_t clientID, uint16_t sessionID,
        uint8_t protocolVersion, uint8_t interfaceVersion, uint8_t messageType, uint8_t returnCode,cPacket *payload) {
    SomeIpHeader *someipheader = new SomeIpHeader("someip");
    //Message ID ("Service ID" 16 Bit | "0" 1 Bit | "Method ID" 15 Bit) or
    //Message ID ("Service ID" 16 Bit | "1" 1 Bit | "Event ID" 15 Bit) or
    uint32_t messageID = 0;
    messageID = serviceID;
    messageID = messageID << 16;
    messageID = messageID | method_EventID;
    someipheader->setMessageID(messageID);
    // Length of whole SOME/IP Payload + header length in Payload.
    someipheader->setLength(ADDITIONAL_BYTES_TO_PAYLOADLENGTH + payload->getByteLength());
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
    for(auto pair = _remotes.begin(); pair != _remotes.end(); pair++){
        _serverSocket.sendTo(someipheader->dup(), pair->first, pair->second);
    }
    delete(someipheader);
}

} /* end namespace SOQoSMW */
