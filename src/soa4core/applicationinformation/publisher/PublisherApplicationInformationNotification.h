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

#ifndef SOA4CORE_APPLICATIONINFORMATION_PUBLISHER_PUBLISHERAPPLICATIONINFORMATIONNOTIFICATION_H_
#define SOA4CORE_APPLICATIONINFORMATION_PUBLISHER_PUBLISHERAPPLICATIONINFORMATIONNOTIFICATION_H_

#include "soa4core/applicationinformation/publisher/PublisherApplicationInformation.h"

namespace SOA4CoRE {

/**
 * @brief The PublisherApplicationInformationNotification is used as a notification class for the PublisherApplicationInformation.
 *
 * @ingroup soa4core/applicationinformation
 *
 * @author Mehmet Mueller
 */

class PublisherApplicationInformationNotification : public cObject, noncopyable {

/**
 * Methods
 */
public:
    /**
     * Constructor
     * @param publisherApplicationInformation the publisher application information
     */
    PublisherApplicationInformationNotification(PublisherApplicationInformation publisherApplicationInformation);
    virtual ~PublisherApplicationInformationNotification();

    /**
     * Returns the publisher application information
     * @return the publisher application information
     */
    PublisherApplicationInformation getPublisherApplicationInformation();
protected:
private:

/**
 * Member variables
 */
public:
protected:
private:
    /**
     * The publisher application information
     */
    PublisherApplicationInformation _publisherApplicationInformation;
};
} /* namespace SOA4CoRE */
#endif /* SOA4CORE_APPLICATIONINFORMATION_PUBLISHER_PUBLISHERAPPLICATIONINFORMATIONNOTIFICATION_H_ */
