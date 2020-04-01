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
    REQUEST              = 0;
    REQUEST_NO_RETURN    = 1;
    NOTIFICATION         = 2;
    RESPONSE             = 3;
    ERROR                = 4;
    TP_REQUEST           = 5;
    TP_REQUEST_NO_RETURN = 6;
    TP_NOTIFICATION      = 7;
    TP_RESPONSE          = 8;
    TP_ERROR             = 9;
}

//Return code
enum ReturnCode{
    E_OK                      = 0;
    E_NOT_OK                  = 1;
    E_UNKNOWN_SERVICE         = 2;
    E_UNKNOWN_METHOD          = 3;
    E_NOT_READY               = 4;
    E_NOT_REACHABLE           = 5;
    E_TIMEOUT                 = 6;
    E_WRONG_PROTOCOL_VERSION  = 7;
    E_WRONG_INTERFACE_VERSION = 8;
    E_MALFORMED_MESSAGE       = 9;
    E_WRONG_MESSAGE_TYPE      = 10;
}

enum ProtocolVersion{
    V_1 = 1;
}

packet SomeIpHeader {
    byteLength = 16;
    uint32_t messageID;
    uint32_t length;
    uint32_t requestID;
    uint8_t  protcolVersion @enum(ProtocolVersion);
    uint8_t  interfaceVersion;
    uint8_t  messageType @enum(MessageType);
    uint8_t  returnCode @enum(ReturnCode);
}
