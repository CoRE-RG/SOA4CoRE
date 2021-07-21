/*
 * IServiceRegistry.h
 *
 *  Created on: Apr 1, 2021
 *      Author: mehkir
 */

#ifndef SOQOSMW_SERVICEREGISTRY_BASE_ISERVICEREGISTRY_H_
#define SOQOSMW_SERVICEREGISTRY_BASE_ISERVICEREGISTRY_H_

#include <soqosmw/service/base/IService.h>
#include <soqosmw/service/serviceidentifier/ServiceIdentifier.h>

namespace SOQoSMW {

/**
 * @brief The interface for service registry.
 *
 * @ingroup soqosmw/serviceregistry
 *
 * @author Mehmet Cakir
 */

class IServiceRegistry {

public:
    /**
     * Methods
     */

    /**
     * Adds a published service
     */
    virtual void addPublisherService(IService *service) = 0;

    /**
     * Returns a service for given service identifier
     * @return IService
     */
    virtual IService* getService(ServiceIdentifier serviceIdentifier) = 0;

    typedef uint32_t ServiceId;
};

} /* namespace SOQoSMW */

#endif /* SOQOSMW_SERVICEREGISTRY_BASE_ISERVICEREGISTRY_H_ */
