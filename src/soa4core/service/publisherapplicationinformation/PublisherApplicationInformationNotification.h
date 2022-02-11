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

#ifndef SOA4CORE_SERVICE_PUBLISHERAPPLICATIONINFORMATION_PUBLISHERAPPLICATIONINFORMATIONNOTIFICATION_H_
#define SOA4CORE_SERVICE_PUBLISHERAPPLICATIONINFORMATION_PUBLISHERAPPLICATIONINFORMATIONNOTIFICATION_H_

#include "soa4core/service/publisherapplicationinformation/PublisherApplicationInformation.h"

namespace SOA4CoRE {
class PublisherApplicationInformationNotification : public cObject, noncopyable {

/**
 * Methods
 */
public:
    PublisherApplicationInformationNotification(PublisherApplicationInformation publisherApplicationInformation);
    virtual ~PublisherApplicationInformationNotification();

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
#endif /* SOA4CORE_SERVICE_PUBLISHERAPPLICATIONINFORMATION_PUBLISHERAPPLICATIONINFORMATIONNOTIFICATION_H_ */