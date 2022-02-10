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

#ifndef __SOA4CORE_SOMEIPUDPPUBLISHERENDPOINT_H_
#define __SOA4CORE_SOMEIPUDPPUBLISHERENDPOINT_H_

#include "soa4core/endpoints/publisher/standard/udp/UDPPublisherEndpoint.h"
#include "soa4core/endpoints/publisher/someip/base/SOMEIPPublisherEndpointBase.h"

namespace SOA4CoRE {

/**
 * @brief The SOME/IP Publisher endpoint using the UDP protocol.
 *
 * @ingroup soa4core/endpoints
 *
 * @author Mehmet Mueller for HAW Hamburg
 */
class SOMEIPUDPPublisherEndpoint : public SOMEIPPublisherEndpointBase, public UDPPublisherEndpoint
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
     * Publish a payload to all subscribers. This needs to be implemented by all subclasses.
     * The message will be deleted by the caller afterwards.
     *
     * @param payload   the payload to be published.
     */
    virtual void publish(cPacket* msg) override;

private:

    /**
     * Member variables
     */
public:
protected:
private:
};
} /* end namespace SOA4CoRE */
#endif
