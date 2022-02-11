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

#include <omnetpp.h>
#include <soa4core/registry/base/IRegistry.h>
#include <unordered_map>
#include <inet/networklayer/common/L3Address.h>

using namespace omnetpp;
namespace SOA4CoRE {

/**
 * @brief This class manages all registered publishers and subscribers
 *
 * @ingroup soa4core/serviceregistry
 *
 * @author Mehmet Mueller for HAW Hamburg
 */
class Registry : public cSimpleModule, public IRegistry
{
  public:
    ~Registry();
    Registry();

    /**
     * Adds a published service
     */
    virtual void addPublisherService(PublisherApplicationInformation publisherApplicationInformation) override;

    /**
     * Returns a service for given service identifier
     * @return QoSService
     */
    virtual PublisherApplicationInformation getService(ServiceIdentifier serviceIdentifier) override;

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
    virtual bool containsService(ServiceIdentifier serviceIdentifier) override;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
  private:
    /**
     * Caches a map of service IDs and the corresponding application information of the application which serves the service
     */
    std::unordered_map<ServiceId,PublisherApplicationInformation> _serviceRegistry;
};

} /* namespace SOA4CoRE */

#endif /* SOA4CORE_SERVICEREGISTRY_LOCALSERVICEREGISTRY_H_ */
