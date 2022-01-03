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

#ifndef SOQOSMW_DISCOVERY_SOMEIPSERVICEDISCOVERY_SOMEIPSDFINDREQUEST_H_
#define SOQOSMW_DISCOVERY_SOMEIPSERVICEDISCOVERY_SOMEIPSDFINDREQUEST_H_

#include <omnetpp.h>
#include "soqosmw/service/subscriberapplicationinformation/SubscriberApplicationInformation.h"

/**
 * @brief This class provides a container for a SOME/IP SD find request
 *
 * @ingroup soqosmw/discovery/someipservicediscovery
 *
 * @author Mehmet Cakir
 */

namespace SOQoSMW {

class SomeIpSDFindRequest : public cObject{
/**
 * Methods
 */
public:
    SomeIpSDFindRequest(inet::L3Address remoteAddress, SubscriberApplicationInformation subscriberApplicationInformation);
    virtual ~SomeIpSDFindRequest();

    /**
     * Getter for the remote ip address
     * @return the remote address
     */
    inet::L3Address getRemoteAddress() const;

    /**
     * Getter for the subscriber application information
     * @return the subscriber application information
     */
    SubscriberApplicationInformation getSubscriberApplicationInformation() const;

protected:
private:

/**
 * Member variables
 */
public:
protected:
private:
    /**
     * The remote ip address
     */
    inet::L3Address _remoteAddress;

    /**
     * The subscriber application information
     */
    SubscriberApplicationInformation _subscriberApplicationInformation;
};

} /* end namespace SOQoSMW */

#endif /* SOQOSMW_DISCOVERY_SOMEIPSERVICEDISCOVERY_SOMEIPSDFINDREQUEST_H_ */
