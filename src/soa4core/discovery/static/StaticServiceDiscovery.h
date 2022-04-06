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

#include "soa4core/registry/base/Registry.h"
#include "soa4core/discovery/base/IServiceDiscovery.h"
//INET
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
/**
 * Methods
 */
public:
    virtual ~StaticServiceDiscovery();

    /**
     * Discovers a service for the given service identifier
     * @param serviceIdentifier the given service identifier
     */
    void discover(ServiceIdentifier serviceIdentifier) override;

protected:
    /**
     * Initializes the module
     * @param stage the initialization stage
     */
    virtual void initialize(int stage) override;

    /**
     * Returns the maximum number of initialization stages
     * @return the maximum number of initialization stages
     */
    virtual int numInitStages() const override {
        return inet::NUM_INIT_STAGES;
    }

    /**
     * Handles the incoming message
     * @param msg the message
     */
    virtual void handleMessage(cMessage *msg) override;
private:

/**
 * Member variables
 */
public:
protected:
private:
    /**
     * Map used to abstract discovery of services
     */
    std::unordered_map<Registry::ServiceId, inet::L3Address> _discoveryAbstractionMap;
};

} /*end namespace SOA4CoRE*/

#endif
