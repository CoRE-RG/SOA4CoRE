/*
 * IRegistry.h
 *
 *  Created on: Apr 1, 2021
 *      Author: mehkir
 */

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
     * Returns a service for given service identifier
     * @return QoSService
     */
    virtual PublisherApplicationInformation getService(ServiceIdentifier serviceIdentifier) = 0;

    /**
     * Returns true if there is a service for the given service identifier
     * @param serviceIdentifier the service identifier
     * @return true if there is a service for the given service identifier, else false
     */
    virtual bool containsService(ServiceIdentifier serviceIdentifier) = 0;

    typedef uint32_t ServiceId;
};

} /* namespace SOA4CoRE */

#endif /* SOA4CORE_REGISTRY_BASE_IREGISTRY_H_ */
