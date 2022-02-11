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

#ifndef SOA4CORE_DISCOVERY_SOMEIP_SOMEIPSDSUBSCRIPTIONINFORMATION_H_
#define SOA4CORE_DISCOVERY_SOMEIP_SOMEIPSDSUBSCRIPTIONINFORMATION_H_

#include <stdint.h>
#include "soa4core/service/subscriberapplicationinformation/SubscriberApplicationInformation.h"

namespace SOA4CoRE {

/**
 * @brief This class provides holding the informations needed for subscription
 *
 * @ingroup soa4core/discovery/someipservicediscovery
 *
 * @author Mehmet Mueller
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

} /* end namespace SOA4CoRE */
#endif /* SOA4CORE_DISCOVERY_SOMEIP_SOMEIPSDSUBSCRIPTIONINFORMATION_H_ */