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

#include "soa4core/serviceidentifier/ServiceIdentifier.h"
//OMNETPP
#include <omnetpp.h>

namespace SOA4CoRE {
class ServiceBase;
class SubscriberConnector;
class PublisherConnector;

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

  /**
   * @brief Registers a new Publisher and returns its connector.
   *
   * @param publisherApplication The application publishing a service.
   * @return the publisher connector
   */
  virtual PublisherConnector* registerPublisherService(ServiceBase* publisherApplication) = 0;

  /**
   * @brief Registers the given Subscriber application and subscribes the service given in the publisher service identifier.
   *
   * @param publisherServiceIdentifier the publisher service identifier
   * @param subscriberApplication the subscriber application
   * @return the subscriber connector
   */
  virtual SubscriberConnector* registerSubscriberServiceAndSubscribeService(ServiceIdentifier publisherServiceIdentifier, ServiceBase* subscriberApplication) = 0;

protected:
  /**
   * @brief Initiates a service discovery for the given service
   *
   * @param publisherServiceIdentifier service identifier of the service to be subscribed to
   * @param subscriberApplication the subscriber application
   */
  virtual void discoverService(ServiceIdentifier publisherServiceIdentifier, ServiceBase* subscriberApplication) = 0;

};

} /*end namespace SOA4CoRE*/

#endif
