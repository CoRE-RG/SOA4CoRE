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

#ifndef SOA4CORE_SERVICEREGISTRY_LOCALSERVICEREGISTRY_H_
#define SOA4CORE_SERVICEREGISTRY_LOCALSERVICEREGISTRY_H_

#include "soa4core/connector/publisher/PublisherConnector.h"
#include "soa4core/connector/subscriber/SubscriberConnector.h"
//OMNETPP
#include <omnetpp.h>
//INET
#include <inet/networklayer/common/L3Address.h>
//STD
#include <unordered_map>


using namespace omnetpp;
namespace SOA4CoRE {

/**
 * @brief The registry stores all registered publishers and subscribers
 *
 * @ingroup soa4core/registry
 *
 * @author Mehmet Mueller for HAW Hamburg
 */
class Registry : public cSimpleModule
{
    /**
     * Typedefs
     */
  public:
    typedef uint32_t ServiceId;

    /**
     * Methods
     */
  public:
    ~Registry();
    Registry();

    /**
     * Adds a publisher service connector
     * @param serviceId the service ID
     * @param publisherConnector the publisher connector
     */
    void addPublisherServiceConnector(ServiceId serviceId, PublisherConnector* publisherConnector);

    /**
     * Adds a subscriber service connector
     * @param serviceId the service ID
     * @param subscriberConnector the subscriber connector
     */
    void addSubscriberServiceConnector(ServiceId serviceId, SubscriberConnector* subscriberConnector);

    /**
     * Returns a publisher connector for a given service identifier
     * @return the publisher connector for the given service identifier, null if not present
     */
    PublisherConnector* getPublisherConnector(ServiceId serviceId);

    /**
     * Returns a subscriber connector for a given service identifier
     * @return the subscriber connector for the given service identifier, null if not present
     */
    SubscriberConnector* getSubscriberConnector(ServiceId serviceId);

    /**
     * Returns the service registry as a map
     * @return registry map
     */
    std::unordered_map<ServiceId,PublisherApplicationInformation> getRegistry();

    /**
     * Returns true if the service registry contains a service for the given service identifier
     * @param serviceIdentifier
     * @return true if a service for the given service identifier is present, else false
     */
    bool containsService(ServiceIdentifier serviceIdentifier);



  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
  private:

    /**
     * Member variables
     */
  public:
  protected:
  private:
    /**
     * Contains pointers to the existing publisher connectors on a node.
     * It is used to look for provided services on a node.
     */
    std::unordered_map<ServiceId, PublisherConnector*> _publisherConnectors;

    /**
     * Contains pointers to the existing subscriber connectors on a node.
     * It used to track subscriber connectors and add Subscriber apps.
     */
    std::unordered_map<ServiceId, std::list<SubscriberConnector*>> _subscriberConnectors;
};

} /* namespace SOA4CoRE */

#endif /* SOA4CORE_SERVICEREGISTRY_LOCALSERVICEREGISTRY_H_ */
