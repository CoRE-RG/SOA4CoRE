/*
 * IServiceRegistry.h
 *
 *  Created on: Apr 1, 2021
 *      Author: mehkir
 */

#ifndef SOQOSMW_SERVICEREGISTRY_BASE_ISERVICEREGISTRY_H_
#define SOQOSMW_SERVICEREGISTRY_BASE_ISERVICEREGISTRY_H_

#include <soqosmw/service/qosservice/QoSService.h>
#include <soqosmw/service/qosserviceidentifier/QoSServiceIdentifier.h>

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
    virtual void addPublisherService(QoSService service) = 0;

    /**
     * Returns a service for given service identifier
     * @return QoSService
     */
    virtual QoSService getService(QoSServiceIdentifier serviceIdentifier) = 0;

    /**
     * Returns true if there is a service for the given service identifier
     * @param serviceIdentifier the service identifier
     * @return true if there is a service for the given service identifier, else false
     */
    virtual bool containsService(QoSServiceIdentifier serviceIdentifier) = 0;

    typedef uint32_t ServiceId;
};

} /* namespace SOQoSMW */

#endif /* SOQOSMW_SERVICEREGISTRY_BASE_ISERVICEREGISTRY_H_ */
