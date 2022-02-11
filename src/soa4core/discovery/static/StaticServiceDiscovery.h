//
// c Timo Haeckel for HAW Hamburg
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

#ifndef __SOA4CORE_DISCOVERY_STATIC_STATICSERVICEDISCOVERY_H_
#define __SOA4CORE_DISCOVERY_STATIC_STATICSERVICEDISCOVERY_H_

#include <soa4core/applicationinformation/publisher/PublisherApplicationInformation.h>
#include <soa4core/registry/base/IRegistry.h>
#include "soa4core/discovery/base/IServiceDiscovery.h"
#include <inet/networklayer/common/L3Address.h>
#include <inet/common/InitStages.h>
//STD
#include <string>
#include <unordered_map>

namespace SOA4CoRE {

/**
 * @brief This class provides a static ServiceDiscovery Module via XML.
 *
 * @ingroup soa4core/discovery
 *
 * @author Timo Haeckel and Mehmet Mueller for HAW Hamburg
 */
class StaticServiceDiscovery: public IServiceDiscovery, public cSimpleModule {
public:
    virtual ~StaticServiceDiscovery();

    /**
     * Discovers a service in the network
     * @param serviceIdentifier
     */
    void discover(ServiceIdentifier serviceIdentifier) override;

    /**
     * Check if the registry contains a service with the service identifier.
     * @param serviceIdentifier The serviceIdentifier of the service to find
     * @return True if the registry contains the service, otherwise false.
     */
    //bool contains(ServiceIdentifier serviceIdentifier);

protected:
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override {
        return inet::NUM_INIT_STAGES;
    }
    virtual void handleMessage(cMessage *msg) override;

private:
    /**
     * Map used to abstract discovery of services
     */
    std::unordered_map<IRegistry::ServiceId,PublisherApplicationInformation> _discoveryAbstractionMap;
};

} /*end namespace SOA4CoRE*/

#endif
