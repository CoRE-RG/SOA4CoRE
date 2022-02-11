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

#ifndef __SOA4CORE_SERVICEMANAGER_BASE_IMANAGER_H_
#define __SOA4CORE_SERVICEMANAGER_BASE_IMANAGER_H_

#include <omnetpp.h>
#include "soa4core/service/subscriberapplicationinformation/SubscriberApplicationInformation.h"

namespace SOA4CoRE {

/**
 * @brief Interface for the service manager module
 *
 * @ingroup soa4core/manager
 *
 * @author Mehmet Mueller
 */
class IManager
{
public:
  virtual ~IManager();
  virtual void subscribeService(QoSServiceIdentifier publisherServiceIdentifier, SubscriberApplicationInformation subscriberApplicationInformation) = 0;

};

} /*end namespace SOA4CoRE*/

#endif
