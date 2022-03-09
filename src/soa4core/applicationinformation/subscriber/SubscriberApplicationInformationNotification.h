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

#ifndef SOA4CORE_APPLICATIONINFORMATION_SUBSCRIBER_SUBSCRIBERAPPLICATIONINFORMATIONNOTIFICATION_H_
#define SOA4CORE_APPLICATIONINFORMATION_SUBSCRIBER_SUBSCRIBERAPPLICATIONINFORMATIONNOTIFICATION_H_

#include "soa4core/applicationinformation/subscriber/SubscriberApplicationInformation.h"

namespace SOA4CoRE {

/**
 * @brief The SubscriberApplicationInformationNotification is used as a notification class for the SubscriberApplicationInformation.
 *
 * @ingroup soa4core/applicationinformation
 *
 * @author Mehmet Mueller
 */

class SubscriberApplicationInformationNotification : public omnetpp::cObject, omnetpp::noncopyable {
    /**
     * Methods
     */
public:
    SubscriberApplicationInformationNotification(SubscriberApplicationInformation subscriberApplicationInformation);
    virtual ~SubscriberApplicationInformationNotification();

    /**
     * Returns the subscriber application information
     * @return the subscriber application information
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
     * The subscriber application information
     */
    SubscriberApplicationInformation _subscriberApplicationInformation;
};
} /* end namespace */
#endif /* SOA4CORE_APPLICATIONINFORMATION_SUBSCRIBER_SUBSCRIBERAPPLICATIONINFORMATIONNOTIFICATION_H_ */
