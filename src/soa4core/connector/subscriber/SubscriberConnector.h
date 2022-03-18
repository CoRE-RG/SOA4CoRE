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

#ifndef __SOA4CORE_SUBSCRIPTIONCONNECTOR_H_
#define __SOA4CORE_SUBSCRIPTIONCONNECTOR_H_

#include "soa4core/applications/base/ServiceBase.h"
#include "soa4core/connector/base/ConnectorBase.h"
#include "soa4core/endpoints/subscriber/base/SubscriberEndpointBase.h"

namespace SOA4CoRE {

/**
 * @brief The SubscriptionConnector implements the common interface for all connector modules @see~ConnectorBase.
 * This module is created by the Manager module (@see~Manager)
 * during runtime to connect a subscriber applications to its protocol specific subscriber endpoint.
 * Only allows traffic from one subscriber endpoint to the applications.
 *
 * @ingroup soa4core/connector
 *
 * @author Timo Haeckel for HAW Hamburg
 */
class SubscriberConnector : public ConnectorBase
{
/**
 * Methods
 */
public:
    /**
     * Adds the application to this connector only if the application is not already in the list
     * and the list is not larger then max applications.
     * Implementing ConnectorClasses need to check for the correct type of application.
     *
     * @param subscriberApplication        the subscriber application to add.
     *
     * @return                             true if the application has been added.
     */
    bool addApplication(ServiceBase* subscriberApplication);

    /**
     * Removes the application from this connector if present.
     * Implementing ConnectorClasses need to check for the correct type of application.
     *
     * @param   subscriberApplication   the subscriber application to remove.
     * @return  the application if it was removed (pointer no longer managed by this module)
     *          nullptr if the application is not registered.
     */
    ServiceBase* removeApplication(ServiceBase* subscriberApplication);

    /**
     * Returns the applications
     * @return the applications
     */
    const std::vector<ServiceBase*>& getApplications() const;

    /**
     * Returns the endpoint to which the applications are connected
     * @return the endpoint
     */
    SubscriberEndpointBase* getEndpoint();

    /**
     * Sets the endpoint
     */
    void setEndpoint(SubscriberEndpointBase* subscriberEndpointBase);

    /**
     * Returns the local IP address
     * @return the IP address
     */
    const inet::L3Address& getAddress() const;

    /**
     * Sets the local IP address of this connector
     * @param localAddress the local IP address
     */
    void setAddress(const inet::L3Address& localAddress);

    /**
     * Returns the TCP port
     * @return the TCP port
     */
    int getTcpPort() const;

    /**
     * Sets the TCP port
     * @param tcpPort the TCP port
     */
    void setTcpPort(int tcpPort);

    /**
     * Returns the UDP port
     * @return the UDP port
     */
    int getUdpPort() const;

    /**
     * Sets the UDP port
     * @param udpPort the UDP port
     */
    void setUdpPort(int udpPort);

protected:
    /**
     * Initializes the module
     */
    virtual void initialize() override;

    /**
     * Handles the incoming message
     * @param msg the message
     */
    virtual void handleMessage(cMessage *msg) override;

    /**
     * Indicates a parameter has changed.
     *
     * @param parname Name of the changed parameter or nullptr if multiple parameter changed.
     */
    virtual void handleParameterChange(const char* parname) override;

    /**
     * This method is called when the simulation is finished
     */
    virtual void finish() override;
private:

/**
 * Member variables
 */
public:
protected:
private:
    /**
     * Applications to connect to the endpoint.
     */
    std::vector<ServiceBase*> _applications;

    /**
     * The maximum number of allowed applications connected to this connector.
     * if < 0 infinite.
     */
    int _maxApplications = -1;

    /**
     * The endpoint to which this connector is connected
     */
    SubscriberEndpointBase* _subscriberEndpointBase = nullptr;

    /**
     * Signal to emit messages which are forwarded to Applications
     */
    simsignal_t _forwardedToApplicationsSignal;

    /**
     * Gate name for traffic to the application module.
     */
    static const char APPLICATION_OUT_GATE_NAME []; // = "connectorIn";
    /**
     * Gate name for traffic from the endpoint module.
     */
    static const char ENDPOINT_IN_GATE_NAME []; //= "endpointIn";

    /**
     * TCP port used for TCP endpoints
     */
    int _tcpPort;

    /**
     * UDP port used for UDP endpoints
     */
    int _udpPort;

    /**
     * The local address.
     */
    inet::L3Address _localAddress;
};

} /*end namespace SOA4CoRE*/

#endif
