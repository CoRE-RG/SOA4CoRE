//
// c Timo Haeckel for HAW Hamburg
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

//
// @brief Message class for envelope.
//
// @ingroup soa4core/messages
// 
// @author Timo Haeckel and Mehmet Mueller for HAW Hamburg
//

cplusplus {{
#include "soa4core/base/EndpointDescription.h"

#define SOA4CORE sizeof(int) + 2*sizeof(EndpointDescription)
}}

class noncobject EndpointDescription;

namespace SOA4CoRE;

enum MiddlewareProtocols {
    QOSNEGOTIATIONPROTOCOL = 0;
}

//
// TODO generated message class
//
packet Envelope {
    int 					protocol @enum(MiddlewareProtocols); //indicates the type of message for better casting and correct forwarding.
    EndpointDescription 	receiver; //contains information about the receiver of the message.
    EndpointDescription		sender; //contains information about the sender of the message.
}
