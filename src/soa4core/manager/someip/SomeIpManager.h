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

#ifndef __SOA4CORE_SOMEIPLOCALSERVICEMANAGER_H_
#define __SOA4CORE_SOMEIPLOCALSERVICEMANAGER_H_

#include "soa4core/applicationinformation/publisher/PublisherApplicationInformation.h"
#include "soa4core/manager/Manager.h"
//OMNETPP
#include <omnetpp.h>

using namespace omnetpp;
namespace SOA4CoRE {

/**
 * @brief The SOME/IP service manager creates, finds and removes local SOME/IP services.
 *
 * @ingroup soa4core/manager
 *
 * @author Mehmet Mueller for HAW Hamburg
 */

/**
 * SubscriptionState provides states that are used to
 * indicate the current progress of subscribing to a service.
 */
enum class SubscriptionState {WAITING_FOR_OFFER, WAITING_FOR_SUBACK};

class SomeIpManager : public Manager
{
/**
 * Methods
 */
public:
    /**
     * @brief Receives discovery response
     * @param source the source
     * @param signalID the signal ID
     * @param obj the object
     * @param details the details
     */
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj, cObject *details) override;

    /**
     * @brief Subscribes the given service if it is already discovered otherwise a service discovery is initiated
     * @param publisherServiceIdentifier service identifier of the service to be subscribed to
     * @param subscriberApplicationInformation the application information of the subscriber
     */
    void subscribeService(ServiceIdentifier publisherServiceIdentifier, SubscriberApplicationInformation subscriberApplicationInformation) override;
protected:
    /**
     * Initializes the module and waits for find
     *
     * @param stage indicates the initialization stage
     */
    virtual void initialize(int stage) override;

    /**
     * Handles incoming message
     *
     * @param msg the received message
     */
    virtual void handleMessage(cMessage *msg) override;
private:
    /**
     * Looks for a requested service in the local registry
     * @param obj the SomeIpSDFindRequest
     */
    void lookForService(cObject* obj);

    /**
     * Adds a service into the local service registry
     * @param obj the service
     */
    void addToLocalServiceRegistry(cObject* obj);

    /**
     * Subscribes a service if there is a pending request for it
     * @param obj the given service
     */
    void subscribeServiceIfThereIsAPendingRequest(cObject* obj);

    /**
     * Acknowledges the subscription
     * @param obj the service to be subscribed to
     */
    void acknowledgeSubscription(cObject* obj);

    /**
     * Processes an acknowledged subscription
     * @param obj the service to be subscribed to
     */
    void processAcknowledgedSubscription(cObject* obj);

    /**
     * Creates a subscriber endpoint for a service
     * @param publisherApplicationInformation the publisher application information
     * @param qosGroup the QoS group
     */
    void createSubscriberEndpoint(PublisherApplicationInformation publisherApplicationInformation, QoSGroup qosGroup);

    /**
     * Returns the corresponding L4 protocol of the QoS group
     * @param qosGroup the QoS group
     * @return the corresponding L4 protocol of the QoS group
     */
    IPProtocolId getIPProtocolId(QoSGroup qosGroup);

/**
 * Member variables
 */
public:
protected:
    /**
     * The signal which is emitted when a requested service is found
     */
    omnetpp::simsignal_t _findResultSignal;

    /**
     * The signal which is emitted when a service is subscribed
     */
    omnetpp::simsignal_t _subscribeSignal;

    /**
     * The signal which is emitted when a subscription has been acknowledged
     */
    omnetpp::simsignal_t _subscribeAckSignal;

private:
    /**
     * The service discovery.
     */
    IServiceDiscovery* _sd;

    /**
     * Contains pending SOME/IP subscriptions
     */
    std::map<IRegistry::ServiceId, std::list<SubscriberApplicationInformation>> _pendingRequestsMap;
};
} /* end namespace SOA4CoRE */
#endif
