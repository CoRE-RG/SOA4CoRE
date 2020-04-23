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

namespace SOQoSMW;
cplusplus {{
#define HEADER_COVERED_BY_PAYLOADLENGTH 8
}}

//Type of message 
enum MessageType{
    REQUEST              = 0x00;
    REQUEST_NO_RETURN    = 0x01;
    NOTIFICATION         = 0x02;
    RESPONSE             = 0x80;
    ERROR                = 0x81;
    TP_REQUEST           = 0x20;
    TP_REQUEST_NO_RETURN = 0x21;
    TP_NOTIFICATION      = 0x22;
    TP_RESPONSE          = 0x23;
    TP_ERROR             = 0x24;
}

//Return code
enum ReturnCode{
    E_OK                      = 0x00;
    E_NOT_OK                  = 0x01;
    E_UNKNOWN_SERVICE         = 0x02;
    E_UNKNOWN_METHOD          = 0x03;
    E_NOT_READY               = 0x04;
    E_NOT_REACHABLE           = 0x05;
    E_TIMEOUT                 = 0x06;
    E_WRONG_PROTOCOL_VERSION  = 0x07;
    E_WRONG_INTERFACE_VERSION = 0x08;
    E_MALFORMED_MESSAGE       = 0x09;
    E_WRONG_MESSAGE_TYPE      = 0x0a;
}

enum ProtocolVersion{
    PV_1 = 0x01;
}

enum InterfaceVersion{
    IV_1 = 0x01;
}

packet SomeIpHeader {
    byteLength = 16;
    uint32_t messageID;
    uint32_t length;
    uint32_t requestID;
    uint8_t  protocolVersion @enum(ProtocolVersion);
    uint8_t  interfaceVersion @enum(InterfaceVersion);
    uint8_t  messageType @enum(MessageType);
    uint8_t  returnCode @enum(ReturnCode);
}
