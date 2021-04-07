/*
 * IServiceRegistry.h
 *
 *  Created on: Apr 1, 2021
 *      Author: mehkir
 */

#ifndef SOQOSMW_SERVICEREGISTRY_BASE_ISERVICEREGISTRY_H_
#define SOQOSMW_SERVICEREGISTRY_BASE_ISERVICEREGISTRY_H_

#include <soqosmw/service/base/IService.h>
#include <soqosmw/service/base/IServiceIdentifier.h>

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
    virtual void addPublisherService(IService) = 0;

    /**
     * Returns a service for given serice identifier
     * @return IService
     */
    virtual IService getService(IServiceIdentifier) = 0;
};

} /* namespace SOQoSMW */

#endif /* SOQOSMW_SERVICEREGISTRY_BASE_ISERVICEREGISTRY_H_ */
