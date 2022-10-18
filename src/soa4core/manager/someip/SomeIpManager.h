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

#include "soa4core/manager/base/Manager.h"
//OMNETPP
#include <omnetpp.h>

#include <map>

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
enum class SubscriptionState_E {WAITING_FOR_OFFER, WAITING_FOR_SUBACK};

class SomeIpManager : public Manager
{
    class ServiceState: cObject {
    public:
        enum class SdPhase {
            INITIAL_WAIT_PHASE,
            REPETITION_PHASE,
            MAIN_PHASE,
        };

        SdPhase phase = SdPhase::INITIAL_WAIT_PHASE;
        double randInitialDelay = 0;
        int numRepetitions = 0;
    };
    class SubscriptionState: ServiceState {
    public:
//        enum class SubscriptionState {WAITING_FOR_OFFER, WAITING_FOR_SUBACK};
        ServiceIdentifier publisherServiceIdentifier; // for reuse after first find
//        SubscriptionState subState = SubscriptionState::WAITING_FOR_OFFER;
    };
    class OfferState: ServiceState {
    public:
        SomeIpDiscoveryNotification serviceOffering; // for reuse after first offer
    };

    typedef std::map<int, OfferState*> OfferInstanceStateMap;
    typedef std::map<int, InstanceStateMap> OfferStateMap;

    typedef std::map<QoSGroup, SubscriptionState*> SubscriptionQoSGroupStateMap;
    typedef std::map<int, QoSGroupStateMap> SubscriptionInstanceStateMap;
    typedef std::map<int, InstanceStateMap> SubscriptionStateMap;
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

    /**
     * @brief Indicates a parameter has changed.
     *
     * @param parname Name of the changed parameter or nullptr if multiple parameter changed.
     */
    virtual void handleParameterChange(const char* parname) override;

    /**
     * Dispatcher method that calls the correct QoS dependent create function.
     *
     * @param csi the csi
     * @param subscriberConnector the subscriber connector
     * @return the subscriber endpoint
     */
    virtual SubscriberEndpointBase* createConnectionSpecificSubscriberEndpoint(ConnectionSpecificInformation* csi, SubscriberConnector* subscriberConnector) override;

    /**
     * @brief Dispatcher method that calls the correct QoS dependent create publisher function.
     *
     * @param qosGroup the QoS group
     * @param publisherConnector the publisher connector
     * @return the publisher endpoint
     */
    virtual PublisherEndpointBase* createQoSSpecificPublisherEndpoint(QoSGroup qosGroup, PublisherConnector* publisherConnector) override;

    /**
     * Returns the QoS group for the given connection type
     * @param  connectionType @see~ConnectionType
     * @return the qos group. @see~QoSGroups
     */
    virtual QoSGroup getQoSGroupForConnectionType(ConnectionType connectionType) override;

    /**
     * @brief Initiates a service discovery for the given service
     *
     * @param publisherServiceIdentifier service identifier of the service to be subscribed to
     * @param subscriberApplication the subscriber application
     */
    virtual void discoverService(ServiceIdentifier publisherServiceIdentifier, ServiceBase* subscriberApplication) override;
private:

    /**
     * @brief Creates a SOME/IP TCP subscriber endpoint
     *
     * @param csi the csi
     * @param subscriberConnector the subscriber connector
     * @return the SOME/IP TCP subscriber endpoint
     */
    SubscriberEndpointBase* createSomeIpTCPSubscriberEndpoint(ConnectionSpecificInformation* csi, SubscriberConnector* subscriberConnector);

    /**
     * @brief Creates a SOME/IP UDP subscriber endpoint
     *
     * @param csi the csi
     * @param subscriberConnector the subscriber connector
     * @return the SOME/IP UDP subscriber endpoint
     */
    SubscriberEndpointBase* createSomeIpUDPSubscriberEndpoint(ConnectionSpecificInformation* csi, SubscriberConnector* subscriberConnector);

    /**
     * @brief Creates a SOME/IP UDP multicast subscriber endpoint
     *
     * @param csi the csi
     * @param subscriberConnector the subscriber connector
     * @return the SOME/IP UDP mcast subscriber endpoint
     */
    SubscriberEndpointBase* createSomeIpUDPMcastSubscriberEndpoint(ConnectionSpecificInformation* csi, SubscriberConnector* subscriberConnector);

    /**
     * @brief Creates a SOME/IP TCP publisher endpoint
     *
     * @param publisherConnector the publisher connector
     * @return the SOME/IP TCP publisher endpoint
     */
    PublisherEndpointBase* createSomeIpTCPPublisherEndpoint(PublisherConnector* publisherConnector);

    /**
     * @brief Creates a SOME/IP UDP publisher endpoint
     *
     * @param publisherConnector the publisher connector
     * @return the SOME/IP UDP publisher endpoint
     */
    PublisherEndpointBase* createSomeIpUDPPublisherEndpoint(PublisherConnector* publisherConnector);

    /**
     * @brief Creates a SOME/IP UDP multicast publisher endpoint
     *
     * @param publisherConnector the publisher connector
     * @return the SOME/IP UDP mcast publisher endpoint
     */
    PublisherEndpointBase* createSomeIpUDPMcastPublisherEndpoint(PublisherConnector* publisherConnector);

    /**
     * Looks for a requested service in the local registry
     * @param obj the SomeIpSDFindRequest
     */
    void lookForService(cObject* obj);

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
     * @param someIpDiscoveryNotification the SOME/IP discovery notification containing the publisher's information
     * @param qosGroup the QoS group
     */
    void createSubscriberEndpoint(SomeIpDiscoveryNotification* someIpDiscoveryNotification, QoSGroup qosGroup);

    /**
     * Returns the corresponding L4 protocol of the QoS group
     * @param qosGroup the QoS group
     * @return the corresponding L4 protocol of the QoS group
     */
    IPProtocolId getIPProtocolId(QoSGroup qosGroup);

    /**
     * Start the initial wait phase and schedule self message
     * @param serviceState the service to handle the event
     */
    void startInitialWaitPhase(ServiceState* serviceState);

    /**
     * Finish the initial wait phase
     * @param serviceState the service to handle the event
     */
    void handleInitialWaitPhaseOver(ServiceState* serviceState);

    /**
     * Start the first/next repetition phase and schedule self message
     * @param serviceState the service to handle the event
     */
    void handleNextRepetitionPhase(ServiceState* serviceState);

    /**
     * Executes find/offer for the service state
     * @param serviceState the service to handle the event
     */
    void executeSdForServiceState(ServiceState* serviceState);

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
     * Cached parameter initialDelayMin for initial wait phase
     */
    double _initialDelayMin;

    /**
     * Cached parameter initialDelayMax for initial wait phase
     */
    double _initialDelayMax;

    /**
     * Cached parameter repitionMax for repetition phase
     */
    double _repetitionsMax;

    /**
     * Cached parameter initialDelayMax for repetition phase
     */
    double _repititionBaseDelay;

    /**
     * Cached parameter cyclicOfferDelay for main phase
     */
    double _cyclicOfferDelay;

    /**
     * The service discovery.
     */
    IServiceDiscovery* _sd;

    /**
     * Contains pending SOME/IP subscriptions
     */
    std::unordered_map<Registry::ServiceId, std::unordered_map<QoSGroup, SubscriptionState_E>> _pendingSubscriptionsMap;
};
} /* end namespace SOA4CoRE */
#endif
