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

#ifndef SOA4CORE_REGISTRY_BASE_IREGISTRY_H_
#define SOA4CORE_REGISTRY_BASE_IREGISTRY_H_

#include "soa4core/applicationinformation/publisher/PublisherApplicationInformation.h"
#include "soa4core/serviceidentifier/ServiceIdentifier.h"

namespace SOA4CoRE {

/**
 * @brief The interface for service registry.
 *
 * @ingroup soa4core/registry
 *
 * @author Mehmet Mueller for HAW Hamburg
 */

class IRegistry {

public:
    /**
     * Methods
     */

    /**
     * Adds a published service
     */
    virtual void addPublisherService(PublisherApplicationInformation publisherApplicationInformation) = 0;

    /**
     * Returns a service for the given service identifier
     * @return the service for the given service identifier
     */
    virtual PublisherApplicationInformation getService(ServiceIdentifier serviceIdentifier) = 0;

    /**
     * Returns true if there is a service for the given service identifier
     * @param serviceIdentifier the service identifier
     *
     * @return true if there is a service for the given service identifier, else false
     */
    virtual bool containsService(ServiceIdentifier serviceIdentifier) = 0;

    typedef uint32_t ServiceId;
};

} /* namespace SOA4CoRE */

#endif /* SOA4CORE_REGISTRY_BASE_IREGISTRY_H_ */
