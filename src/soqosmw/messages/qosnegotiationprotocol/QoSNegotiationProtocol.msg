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
// @brief Message class for quality of services negotiation protocol.
//
// @ingroup soqosmw/messages
// 
// @author Timo Haeckel and Mehmet Cakir for HAW Hamburg
//

cplusplus {{
#include "soqosmw/messages/Envelope_m.h"
#include "soqosmw/messages/QoSNegotiationProtocol/ConnectionSpecificInformation_m.h"
}}
packet SOQoSMW::Envelope;
enum SOQoSMWProtocols;

namespace SOQoSMW;

//Groups of QoS Requirements
enum QoSGroups {
    RT = 0; // RealTime Requirements --> AVB / TSN
    STD_TCP = 1; // Standard Requirements --> TCP IP
    STD_UDP = 2; // Standard Requirements --> UDP IP
    WEB = 3; // Web Requirements --> HTTP
}

//Type of Negotiation message for @see QoSNegotiationProtocol
enum QoSNegotiationMsgType{
    QoS_Request 	= 0;
    QoS_Response 	= 1;
    QoS_Establish	= 2;
    QoS_Finalise	= 3;
}

//Type of Negotiation response payload status @see QoSNegotiationResponsePayload
enum QoSNegotiationStatus{
 	Failure	= 0;
 	Success	= 1;
}


//Message type for all Protocol message of the qos negotiation
packet QoSNegotiationProtocolMsg extends SOQoSMW::Envelope {
    protocol = SOQoSMWProtocols::QOSNEGOTIATIONPROTOCOL; //indicates the type of message for better casting.
    //receiver; inherited from Envelope
    //sender; inherited from Envelope  
    int messageType @enum(QoSNegotiationMsgType); //indicates the type of message for better casting.
    int qosClass @enum(QoSGroups); 
}

//request for qos negotiation
packet QoSNegotiationRequest extends QoSNegotiationProtocolMsg {
    //protocol = SOQoSMWProtocols::QOSNEGOTIATIONPROTOCOL; //inherited from QoSNegotiationProtocol and Envelope
    //receiver; inherited from Envelope
    //sender; inherited from Envelope
    messageType = QoSNegotiationMsgType::QoS_Request; //inherited from QoSNegotiationProtocol
    //qosClass; inherited from QoSNegotiationProtocol
}

//response to a request for qos negotiation
packet QoSNegotiationResponse extends QoSNegotiationProtocolMsg {
    //protocol = SOQoSMWProtocols::QOSNEGOTIATIONPROTOCOL; //inherited from QoSNegotiationProtocol and Envelope
    //receiver; inherited from Envelope
    //sender; inherited from Envelope
    messageType = QoSNegotiationMsgType::QoS_Response; //inherited from QoSNegotiationProtocol 
    //qosClass; inherited form QoSNegotiationProtocol
    int responseStatus @enum(QoSNegotiationStatus);
}

//establish the negotiated connection
packet QoSNegotiationEstablish extends QoSNegotiationProtocolMsg {
    //protocol = SOQoSMWProtocols::QOSNEGOTIATIONPROTOCOL; //inherited from QoSNegotiationProtocol and Envelope
    //receiver; inherited from Envelope
    //sender; inherited from Envelope
    messageType = QoSNegotiationMsgType::QoS_Establish; //inherited from QoSNegotiationProtocol
    //qosClass; inherited form QoSNegotiationProtocol
}

//finalise the connection
packet QoSNegotiationFinalise extends QoSNegotiationProtocolMsg {
    //protocol = SOQoSMWProtocols::QOSNEGOTIATIONPROTOCOL; //inherited from QoSNegotiationProtocol and Envelope
    //receiver; inherited from Envelope
    //sender; inherited from Envelope
    messageType = QoSNegotiationMsgType::QoS_Finalise; //inherited from QoSNegotiationProtocol
    //qosClass; inherited form QoSNegotiationProtocol
    int finalStatus @enum(QoSNegotiationStatus);
}