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

#include <soa4core/applicationinformation/subscriber/SubscriberApplicationInformation.h>
#include <stdint.h>

namespace SOA4CoRE {

/**
 * @brief This class provides a container holding the informations needed for subscription that is used as a notification object.
 *
 * @ingroup soa4core/discovery
 *
 * @author Mehmet Mueller for HAW Hamburg
 */

class SomeIpSDSubscriptionInformation : public cObject, noncopyable {
/**
 * Methods
 */
public:
    /**
     * Constructor
     * @param remoteAddress the remote address of the publisher node
     * @param subscriberApplicationInformation the application information of the subscriber
     */
    SomeIpSDSubscriptionInformation(inet::L3Address remoteAddress, SubscriberApplicationInformation subscriberApplicationInformation);
    virtual ~SomeIpSDSubscriptionInformation();

    /**
     * Returns the remote address of the publisher node
     * @return the remote address of the publisher node
     */
    const inet::L3Address& getRemoteAddress() const;

    /**
     * Returns the application information of the subscriber
     * @return the application information of the subscriber
     */
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
