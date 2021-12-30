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

#ifndef SOQOSMW_SERVICEREGISTRY_LOCALSERVICEREGISTRY_H_
#define SOQOSMW_SERVICEREGISTRY_LOCALSERVICEREGISTRY_H_

#include <omnetpp.h>
#include <unordered_map>
#include "inet/networklayer/common/L3Address.h"
#include "soqosmw/serviceregistry/base/IServiceRegistry.h"

using namespace omnetpp;
namespace SOQoSMW {

/**
 * @brief This class manages all registered publishers and subscribers
 *
 * @ingroup soqosmw/serviceregistry
 *
 * @author Mehmet Cakir
 */
class LocalServiceRegistry : public cSimpleModule, public IServiceRegistry
{
  public:
    ~LocalServiceRegistry();
    LocalServiceRegistry();

    /**
     * Adds a published service
     */
    virtual void addPublisherService(QoSService service) override;

    /**
     * Returns a service for given service identifier
     * @return QoSService
     */
    virtual QoSService getService(QoSServiceIdentifier serviceIdentifier) override;

    /**
     * Returns the service registry as a map
     * @return registry map
     */
    std::unordered_map<ServiceId,QoSService> getRegistry();

    /**
     * Returns true if the service registry contains a service for the given service identifier
     * @param serviceIdentifier
     * @return true if a service for the given service identifier is present, else false
     */
    virtual bool containsService(QoSServiceIdentifier serviceIdentifier) override;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
  private:
    std::unordered_map<ServiceId,QoSService> _serviceRegistry;
};

} /* namespace SOQoSMW */

#endif /* SOQOSMW_SERVICEREGISTRY_LOCALSERVICEREGISTRY_H_ */
