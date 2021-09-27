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

#ifndef SOQOSMW_APPLICATIONS_SOMEIPAPP_BASE_SOMEIPAPPBASE_H_
#define SOQOSMW_APPLICATIONS_SOMEIPAPP_BASE_SOMEIPAPPBASE_H_

//#include "soqosmw/messages/someip/SomeIpHeader_m.h"
#include "soqosmw/applications/someipapp/base/ISomeIpAppBase.h"

namespace SOQoSMW {

/**
 * @brief Base class for a SOME/IP application.
 *
 * @ingroup soqosmw/applications
 *
 * @author Mehmet Cakir
 */
class SomeIpAppBase{
    /**
     * Methods
     */
public:
    SomeIpAppBase();
    virtual ~SomeIpAppBase();
protected:

    /**
     * Encapsulates packet into a SOME/IP Header
     *
     * @param serviceID - the service id
     * @param method_EventID - the method id
     * @param clientIDPrefix - configurable prefix or fixed value to have unique identifier in the overall vehicle
     * @param clientID - ID of calling client
     * @param sessionID - unique identifier that allows to distinguish sequential messages or requests
     * @param protocolVersion - version of SOME/IP Protocol
     * @param interfaceVersion - version of service method interface
     * @param messageType - the message type
     * @param returnCode - the return code
     * @param payload - the payload
     * @return Packet encapsulated in SOME/IP Header
     */
    /*
    virtual SomeIpHeader* encapsulatePayload(uint16_t serviceID, uint16_t method_EventID, uint8_t clientIDPrefix, uint8_t clientID, uint16_t sessionID,
            uint8_t protocolVersion, uint8_t interfaceVersion, uint8_t messageType, uint8_t returnCode, cPacket *payload);
    */
private:
    /**
     * Member variables
     */
public:
protected:
private:
};
} /* end namespace SOQoSMW */
#endif /* SOQOSMW_APPLICATIONS_SOMEIPAPP_BASE_SOMEIPAPPBASE_H_ */
