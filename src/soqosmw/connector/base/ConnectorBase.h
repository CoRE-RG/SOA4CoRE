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

#ifndef __SOQOSMW_CONNECTORBASE_H_
#define __SOQOSMW_CONNECTORBASE_H_

#include <omnetpp.h>
#include <mutex>
#include "soqosmw/service/qosservice/QoSService.h"

namespace SOQoSMW {
class SOQoSMWApplicationBase;
class EndpointBase;
} /* namespace SOQoSMW */

using namespace omnetpp;

namespace SOQoSMW {

#define PROCESSINGDELAY_MSG_NAME "Processing Delay"

/**
 * @brief The ConnectorBase provides a common interface for all connctor modules.
 * Those modules are created by the LocalServiceManager module (@see~LocalServiceManager)
 * during runtime to connect a service application to its protocol specific service endpoints.
 *
 * @ingroup soqosmw/connector
 *
 * @author Timo Haeckel and Mehmet Cakir for HAW Hamburg
 */
class ConnectorBase : public cSimpleModule
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
    virtual bool addEndpoint(EndpointBase* endpoint);

    /**
     * Removes the endpoint from this connector if present.
     * Implementing ConnectorClasses need to check for the correct type of endpoint.
     *
     * @param endpoint    the endpoint to remove.
     * @return  the endpoint if it was removed (pointer no longer managed by this module)
     *          nullptr if the endpoint is not registered.
     */
    virtual EndpointBase* removeEndpoint(EndpointBase* endpoint);

    /**
     * Adds the application to this connector only if the application is not already in the list
     * and the list is not larger then max applications.
     * Implementing ConnectorClasses need to check for the correct type of application.
     *
     * @param application    the application to add.
     *
     * @return               true if the application has been added.
     */
    virtual bool addApplication(SOQoSMWApplicationBase* application);

    /**
     * Removes the application from this connector if present.
     * Implementing ConnectorClasses need to check for the correct type of application.
     *
     * @param application    the endpoint to remove.
     * @return  the application if it was removed (pointer no longer managed by this module)
     *          nullptr if the application is not registered.
     */
    virtual SOQoSMWApplicationBase* removeApplication(SOQoSMWApplicationBase* application);

    //getter + setter
    bool isApplicationFwdEnabled() const {
        return _applicationFwdEnabled;
    }

    void setApplicationFwdEnabled(bool applicationFwdEnabled) {
        _applicationFwdEnabled = applicationFwdEnabled;
    }

    bool isEndpointFwdEnabled() const {
        return _endpointFwdEnabled;
    }

    void setEndpointFwdEnabled(bool endpointFwdEnabled) {
        _endpointFwdEnabled = endpointFwdEnabled;
    }

    const std::vector<EndpointBase*>& getEndpoints() const {
        return _endpoints;
    }

    const std::vector<SOQoSMWApplicationBase*>& getApplications() const {
        return _applications;
    }

    QoSService getQoSService() {
        return _qosService;
    }

    void setQoSService(QoSService qosService) {
        _qosService = qosService;
    }

private:
    /**
     * Signal to emit messages which are forwarded to Endpoints
     */
    simsignal_t _forwardedToEndpointsSignal;

    /**
     * Signal to emit messages which are forwarded to Applications
     */
    simsignal_t _forwardedToApplicationsSignal;

    /**
     * Signal to emit messages which are dropped
     */
    simsignal_t _messageDroppedSignal;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void handleParameterChange(const char* parname) override;
    virtual void finish() override;

private:

    /**
     * Member variables
     */
public:
protected:
    /**
     * Caches if forwarding to the endpoints is enabled.
     */
    bool _endpointFwdEnabled;

    /**
     * Caches if forwarding to the applications is enabled.
     */
    bool _applicationFwdEnabled;

    /**
     * Caches if connector mapping creation after simulation is enabled.
     */
    bool _createConnectorMappingEnabled;

    /**
     * Endpoints to connect to the applications.
     */
    std::vector<EndpointBase*> _endpoints;

    /**
     * The maximum number of allowed endpoints connected to this connector.
     * if < 0 infinite.
     */
    int _maxEndpoints = -1;

    /**
     * Applications to connect to the endpoint.
     */
    std::vector<SOQoSMWApplicationBase*> _applications;

    /**
     * The maximum number of allowed applications connected to this connector.
     * if < 0 infinite.
     */
    int _maxApplications = -1;

    /**
     * Gate name for traffic from the application module.
     */
    static const char APPLICATION_IN_GATE_NAME []; // = "applicationIn";
    /**
     * Gate name for traffic to the application module.
     */
    static const char APPLICATION_OUT_GATE_NAME []; // = "connectorIn";
    /**
     * Gate name for traffic from the endpoint module.
     */
    static const char ENDPOINT_IN_GATE_NAME []; //= "endpointIn";
    /**
     * Gate name for traffic to the endpoint module.
     */
    static const char ENDPOINT_OUT_GATE_NAME []; //= "connectorIn";

private:
    /**
     * QoSService of the connection
     */
    QoSService _qosService;
};

} /*end namespace SOQoSMW*/

#endif
