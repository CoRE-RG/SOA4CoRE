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

#ifndef __SOA4CORE_SOMEIPUDPMCASTSUBSCRIBERENDPOINT_H_
#define __SOA4CORE_SOMEIPUDPMCASTSUBSCRIBERENDPOINT_H_

#include "soa4core/endpoints/subscriber/standard/udp/UDPMcastSubscriberEndpoint.h"

namespace SOA4CoRE {
/**
 * @brief The SOMEIPUDPMcastSubscriberEndpoint is a subscriber endpoint module  SOME/IP traffic via UDP multicasts.
 * It decapsulates a message in a SOME/IP header before it is forwarded to the subscriber.
 *
 * @ingroup soa4core/endpoints
 *
 * @author Timo Haeckel for HAW Hamburg
 */
class SOMEIPUDPMcastSubscriberEndpoint : public UDPMcastSubscriberEndpoint
{
/**
 * Methods
 */
public:
    /**
     * Creates a connection specific information for this endpoint.
     * @return  the connection specific information.
     */
    virtual ConnectionSpecificInformation* getConnectionSpecificInformation() override;

protected:

    /**
     * Handles messages incoming from transport gate and
     * forwards them to the connector if they are for applications.
     *
     * @param msg   the message to handle.
     */
    virtual void handleTransportIn(cMessage *msg) override;
private:

/**
 * Member variables
 */
public:
protected:
private:
};

} /*end namespace SOA4CoRE*/

#endif //__SOA4CORE_SOMEIPUDPMCASTSUBSCRIBERENDPOINT_H_
