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

#ifndef __SOA4CORE_PUBLISHERCONNECTOR_H_
#define __SOA4CORE_PUBLISHERCONNECTOR_H_

#include "soa4core/applications/base/ServiceBase.h"
#include "soa4core/connector/base/ConnectorBase.h"
#include "soa4core/endpoints/publisher/base/PublisherEndpointBase.h"
#include "soa4core/service/publisherapplicationinformation/PublisherApplicationInformation.h"

namespace SOA4CoRE {

/**
 * @brief The PublisherConnector implements the common interface for all connctor modules @see~ConnectorBase.
 * This module is created by the Manager module (@see~Manager)
 * during runtime to connect a publisher application to its protocol specific publisher endpoints.
 *
 * @ingroup soa4core/connector
 *
 * @author Timo Haeckel for HAW Hamburg
 */
class PublisherConnector : public ConnectorBase
{
    /**
     * Methods
     */
public:
    /**
     * Adds the endpoint to this connector only if the endpoint is not already in the list
     * and the list is not larger then max applications.
     * Implementing ConnectorClasses need to check for the correct type of endpoint.
     *
     * @param endpoint    the endpoint to add.
     *
     * @return            true if the endpoint has been added.
     */
    bool addEndpoint(PublisherEndpointBase* endpoint);

    /**
     * Removes the endpoint from this connector if present.
     * Implementing ConnectorClasses need to check for the correct type of endpoint.
     *
     * @param endpoint    the endpoint to remove.
     * @return  the endpoint if it was removed (pointer no longer managed by this module)
     *          nullptr if the endpoint is not registered.
     */
    PublisherEndpointBase* removeEndpoint(PublisherEndpointBase* endpoint);

    /**
     * Sets the application of this connector.
     * Implementing ConnectorClasses need to check for the correct type of application.
     *
     * @param middlewareApplicationBase    the application to add.
     */
    void setApplication(ServiceBase* middlewareApplicationBase);

    /**
     * Returns the application of this connector.
     *
     * @return the application of this connector.
     */
    ServiceBase* getApplication();

    /**
     * Sets the application information
     * @param qosGroups
     */
    void setPublisherApplicationInformation(PublisherApplicationInformation publisherApplicationInformation);

    /**
     * Returns the application information
     * @return
     */
    PublisherApplicationInformation getPublisherApplicationInformation();

    /**
     * Returns the endpoints on which the Publisher publishes
     * @return the endpoints
     */
    const std::vector<PublisherEndpointBase*>& getEndpoints() const;
protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void handleParameterChange(const char* parname) override;
    virtual void finish() override;
private:

    /**
     * Members
     */
public:
protected:
private:
    /**
     * The maximum number of allowed endpoints connected to this connector.
     * if < 0 infinite.
     */
    int _maxEndpoints = -1;

    ServiceBase* _middlewareApplicationBase = nullptr;

    /**
     * Endpoints to connect to the applications.
     */
    std::vector<PublisherEndpointBase*> _endpoints;

    /**
     * The publisher application information
     */
    PublisherApplicationInformation _publisherApplicationInformation;

    /**
     * Signal to emit messages which are forwarded to Endpoints
     */
    simsignal_t _forwardedToEndpointsSignal;

    /**
     * Gate name for traffic from the application module.
     */
    static const char APPLICATION_IN_GATE_NAME []; // = "applicationIn";

    /**
     * Gate name for traffic to the endpoint module.
     */
    static const char ENDPOINT_OUT_GATE_NAME []; //= "connectorIn";
};
} /*end namespace SOA4CoRE*/

#endif
