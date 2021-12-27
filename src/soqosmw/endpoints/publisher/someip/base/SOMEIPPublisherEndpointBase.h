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

#ifndef __SOQOSMW_SOMEIPPUBLISHERENDPOINTBASE_H_
#define __SOQOSMW_SOMEIPPUBLISHERENDPOINTBASE_H_

#include "soqosmw/endpoints/publisher/base/PublisherEndpointBase.h"
#include "soqosmw/messages/someip/SomeIpHeader_m.h"

namespace SOQoSMW {

#define ARBITRARY_EVENT_ID 0x8001
#define ARBITRARY_CLIENT_ID_PREFIX 0x01
#define ARBITRARY_CLIENT_ID 0x01
#define SESSION_ID 0x0000 //0x0000 means inactive session handling

/**
 * @brief The SOMEIPPublisherEndpointBase adds SOME/IP packet creation.
 *
 * @ingroup soqosmw/endpoints
 *
 * @author Mehmet Cakir
 */
class SOMEIPPublisherEndpointBase
{
    /**
     * Methods
     */
public:
protected:
    /**
     * Creates a SOME/IP packet with the specified parameters
     *
     * @param serviceID the service identifier
     * @param method_EventID the method or event identifier
     * @param clientIDPrefix the client identifier prefix
     * @param clientID the client identifier
     * @param sessionID the session identifier
     * @param protocolVersion the protocol version of the SOME/IP protocol
     * @param interfaceVersion the interface version of the service interface
     * @param messageType the message type
     * @param returnCode the return code
     * @param payload the payload
     * @return the created SOME/IP packet
     */
    cPacket* createSOMEIPPacket(uint16_t serviceID, cPacket *payload, uint16_t method_EventID = ARBITRARY_EVENT_ID,
            uint8_t clientIDPrefix = ARBITRARY_CLIENT_ID_PREFIX, uint8_t clientID = ARBITRARY_CLIENT_ID,
            uint16_t sessionID = SESSION_ID, uint8_t protocolVersion = SOQoSMW::ProtocolVersion::PV_1,
            uint8_t interfaceVersion = SOQoSMW::InterfaceVersion::IV_1, uint8_t messageType = SOQoSMW::MessageType::REQUEST,
            uint8_t returnCode = SOQoSMW::ReturnCode::E_OK);
private:
    /**
     * Member variables
     */
public:
protected:
private:
};

} /*end namespace SOQoSMW*/

#endif
