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

packet SomeIpHeader {
    byteLength = 16;
    uint32_t messageID;
    uint32_t length;
    uint32_t requestID;
    uint8_t  protcolVersion;
    uint8_t  interfaceVersion;
    uint8_t  messageType;
    uint8_t  returnCode;
}

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