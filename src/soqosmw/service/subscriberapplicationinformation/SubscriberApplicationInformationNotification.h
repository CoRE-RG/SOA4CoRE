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

#ifndef SOQOSMW_SERVICE_SUBSCRIBERAPPLICATIONINFORMATION_SUBSCRIBERAPPLICATIONINFORMATIONNOTIFICATION_H_
#define SOQOSMW_SERVICE_SUBSCRIBERAPPLICATIONINFORMATION_SUBSCRIBERAPPLICATIONINFORMATIONNOTIFICATION_H_

#include "soqosmw/service/subscriberapplicationinformation/SubscriberApplicationInformation.h"

namespace SOQoSMW {
class SubscriberApplicationInformationNotification : public omnetpp::cObject, omnetpp::noncopyable {
    /**
     * Methods
     */
public:
    SubscriberApplicationInformationNotification(SubscriberApplicationInformation subscriberApplicationInformation);
    virtual ~SubscriberApplicationInformationNotification();

    const SubscriberApplicationInformation& getSubscriberApplicationInformation() const;

protected:
private:

    /**
     * Member variables
     */
public:
protected:
private:
    SubscriberApplicationInformation _subscriberApplicationInformation;
};
} /* end namespace */
#endif /* SOQOSMW_SERVICE_SUBSCRIBERAPPLICATIONINFORMATION_SUBSCRIBERAPPLICATIONINFORMATIONNOTIFICATION_H_ */
