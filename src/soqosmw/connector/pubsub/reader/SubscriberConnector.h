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

#ifndef __SOQOSMW_SUBSCRIPTIONCONNECTOR_H_
#define __SOQOSMW_SUBSCRIPTIONCONNECTOR_H_

#include "soqosmw/connector/base/ConnectorBase.h"
#include "soqosmw/endpoints/subscriber/base/SubscriberEndpointBase.h"
#include "soqosmw/service/subscriberapplicationinformation/SubscriberApplicationInformation.h"
#include "soqosmw/applications/base/SOQoSMWApplicationBase.h"

namespace SOQoSMW {

/**
 * @brief The SubscriptionConnector implements the common interface for all connector modules @see~ConnectorBase.
 * This module is created by the LocalServiceManager module (@see~LocalServiceManager)
 * during runtime to connect a subscriber applications to its protocol specific subscriber endpoint.
 * Only allows traffic from one subscriber endpoint to the applications.
 *
 * @ingroup soqosmw/connector
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
     * @param application    the application to add.
     *
     * @return               true if the application has been added.
     */
    bool addApplication(SOQoSMWApplicationBase* soqosMwApplicationBase);

    /**
     * Removes the application from this connector if present.
     * Implementing ConnectorClasses need to check for the correct type of application.
     *
     * @param application    the endpoint to remove.
     * @return  the application if it was removed (pointer no longer managed by this module)
     *          nullptr if the application is not registered.
     */
    SOQoSMWApplicationBase* removeApplication(SOQoSMWApplicationBase* soqosMwApplicationBase);

    /**
     * Returns the applications
     * @return the applications
     */
    const std::vector<SOQoSMWApplicationBase*>& getApplications() const;

    /**
     * TODO should rather be ConnectorInformation
     * Returns the subscriber application information
     * @return the subscriber application information
     */
    SubscriberApplicationInformation getSubscriberApplicationInformation();

    /**
     * Sets the subscriber application information
     * @param subscriberApplicationInformation the subscriber application information
     */
    void setSubscriberApplicationInformation(SubscriberApplicationInformation subscriberApplicationInformation);

    /**
     * Returns the endpoint to which the applications are connected
     * @return the endpoint
     */
    SubscriberEndpointBase* getEndpoint();

    /**
     * Sets the endpoint
     */
    void setEndpoint(SubscriberEndpointBase* subscriberEndpointBase);
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
private:
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
     * The endpoint to which this connector is connected
     */
    SubscriberEndpointBase* _subscriberEndpointBase = nullptr;

    /**
     * The subscriber application information
     */
    SubscriberApplicationInformation _subscriberApplicationInformation;

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
};

} /*end namespace SOQoSMW*/

#endif
