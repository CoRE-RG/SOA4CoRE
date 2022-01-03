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

#ifndef SOQOSMW_DISCOVERY_SOMEIPSERVICEDISCOVERY_SOMEIPSDSUBSCRIPTIONINFORMATION_H_
#define SOQOSMW_DISCOVERY_SOMEIPSERVICEDISCOVERY_SOMEIPSDSUBSCRIPTIONINFORMATION_H_

#include <stdint.h>
#include "soqosmw/service/subscriberapplicationinformation/SubscriberApplicationInformation.h"

namespace SOQoSMW {

/**
 * @brief This class provides holding the informations needed for subscription
 *
 * @ingroup soqosmw/discovery/someipservicediscovery
 *
 * @author Mehmet Cakir
 */
class SomeIpSDSubscriptionInformation : public cObject, noncopyable {
/**
 * Methods
 */
public:
    SomeIpSDSubscriptionInformation(inet::L3Address remoteAddress, SubscriberApplicationInformation subscriberApplicationInformation);
    virtual ~SomeIpSDSubscriptionInformation();

    const inet::L3Address& getRemoteAddress() const;

    const SubscriberApplicationInformation& getSubscriberApplicationInformation() const;

protected:
private:
/**
 * Member variables
 */
public:
protected:
private:
    /**
     * The remote address of the publisher
     */
    inet::L3Address _remoteAddress;

    /**
     * The subscriber application information
     */
    SubscriberApplicationInformation _subscriberApplicationInformation;
};

} /* end namespace SOQoSMW */
#endif /* SOQOSMW_DISCOVERY_SOMEIPSERVICEDISCOVERY_SOMEIPSDSUBSCRIPTIONINFORMATION_H_ */
