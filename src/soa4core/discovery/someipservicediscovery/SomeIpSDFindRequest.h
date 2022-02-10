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

#ifndef SOA4CORE_DISCOVERY_SOMEIPSERVICEDISCOVERY_SOMEIPSDFINDREQUEST_H_
#define SOA4CORE_DISCOVERY_SOMEIPSERVICEDISCOVERY_SOMEIPSDFINDREQUEST_H_

#include <omnetpp.h>
#include "soa4core/service/subscriberapplicationinformation/SubscriberApplicationInformation.h"

/**
 * @brief This class provides a container for a SOME/IP SD find request
 *
 * @ingroup soa4core/discovery/someipservicediscovery
 *
 * @author Mehmet Mueller
 */

namespace SOA4CoRE {

class SomeIpSDFindRequest : public cObject, noncopyable {
/**
 * Methods
 */
public:
    SomeIpSDFindRequest(uint16_t serviceId, uint16_t instanceId, inet::L3Address remoteAddress);
    virtual ~SomeIpSDFindRequest();

    /**
     * Returns the service ID
     * @return the service ID
     */
    uint16_t getServiceId() const;

    /**
     * Returns the instance ID
     * @return the instance ID
     */
    uint16_t getInstanceId() const;

    /**
     * Returns the remote ip address
     * @return the remote address
     */
    inet::L3Address getRemoteAddress() const;

protected:
private:

/**
 * Member variables
 */
public:
protected:
private:
    /**
     * The service ID
     */
    uint16_t _serviceId;

    /**
     * The instance ID
     */
    uint16_t _instanceId;

    /**
     * The remote ip address
     */
    inet::L3Address _remoteAddress;

    /**
     * The subscriber application information
     */
    SubscriberApplicationInformation _subscriberApplicationInformation;
};

} /* end namespace SOA4CoRE */

#endif /* SOA4CORE_DISCOVERY_SOMEIPSERVICEDISCOVERY_SOMEIPSDFINDREQUEST_H_ */
