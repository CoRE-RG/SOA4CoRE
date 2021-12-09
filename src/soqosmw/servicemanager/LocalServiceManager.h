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

#ifndef __SOQOSMW_SERVICEMANAGER_LOCALSERVICEMANAGER_H_
#define __SOQOSMW_SERVICEMANAGER_LOCALSERVICEMANAGER_H_

#include <omnetpp.h>
#include <soqosmw/messages/qosnegotiation/ConnectionSpecificInformation_m.h>
#include "soqosmw/qosmanagement/negotiation/datatypes/Request.h"
#include "soqosmw/qospolicy/base/qospolicy.h"
#include "soqosmw/connector/pubsub/reader/SubscriberConnector.h"
#include "soqosmw/connector/pubsub/writer/PublisherConnector.h"
#include "soqosmw/endpoints/publisher/base/PublisherEndpointBase.h"
#include "soqosmw/endpoints/subscriber/base/SubscriberEndpointBase.h"
#include "soqosmw/qosmanagement/negotiation/QoSNegotiationProtocol.h"
#include "soqosmw/qosmanagement/negotiation/broker/QoSBroker.h"
#include "soqosmw/applications/base/SOQoSMWApplicationBase.h"
#include "soqosmw/discovery/static/StaticServiceDiscovery.h"
#include "soqosmw/serviceregistry/base/IServiceRegistry.h"
#include "soqosmw/servicemanager/base/ILocalServiceManager.h"
#include "soqosmw/service/qosservice/QoSService.h"
#include <atomic>
#include <string>
#include <map>
#include <vector>
//INET
#include <inet/networklayer/common/L3Address.h>
#include "inet/common/InitStages.h"

using namespace omnetpp;

namespace SOQoSMW {

/**
 * @brief The LocalServiceManager creates, finds and removes local Services.
 *
 * @ingroup soqosmw/servicemanager
 *
 * @author Timo Haeckel and Mehmet Cakir for HAW Hamburg
 */
class LocalServiceManager: public ILocalServiceManager, public cSimpleModule, public cListener {
    friend QoSNegotiationProtocol;
    friend QoSBroker;

public:
    LocalServiceManager();
    virtual ~LocalServiceManager();

    /**
     * @brief This Method creates a new Publisher according to the QoSPolicies.
     *
     * @param serviceId Path of the Publisher Service (e.g. "reifendruck/links")
     * @param qosPolicies The QoS Policies for the Publisher.
     * @param executingModule The service executing the request.
     */
    void registerPublisherService(
            uint32_t serviceId,
            QoSPolicyMap& qosPolicies,
            SOQoSMWApplicationBase* executingApplication);

    /**
     * @brief This Method creates a new Subscriber for the publisher Service according to the QoSPolicies.
     *
     * @param subscriberPath Path of the Subscriber Service (e.g. "bordcomputer")
     * @param publisherPath Path of the Publisher Service (e.g. "reifendruck/links")
     * @param qosPolicies The QoS Policies for the Subscriber.
     * @param executingModule The service executing the request.
     */
    void registerSubscriberService(
            uint32_t publisherServiceId,
            QoSPolicyMap& qosPolicies,
            SOQoSMWApplicationBase* executingApplication);

    /**
     * @brief Subscribes the given service
     * @param subscriberServiceIdentifier
     * @param publisherServiceIdentifier
     * @param qosPolicyMap
     */
    void subscribeQoSService(IServiceIdentifier& publisherServiceIdentifier, QoSPolicyMap& qosPolicyMap);

    /**
     * @brief Receives discovery response
     * @param source
     * @param signalID
     * @param obj
     * @param details
     */
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj, cObject *details) override;

    /**
     * @brief Subscribes the given service
     * @param obj
     */
    void subscribeService(cObject* obj) override;

    /**
     * Returns the publisher connector for the given publisher service
     * @param publisherServiceId
     * @return corresponding publisher connector
     */
    PublisherConnector* getPublisherConnector(uint32_t publisherServiceId);

    /**
     * Returns the subscriber connector for the given publisher service
     * @param publisherServiceId
     * @return corresponding subscriber connector
     */
    SubscriberConnector* getSubscriberConnector(uint32_t publisherServiceId);

protected:
    /**
     * @brief Initialization of the module.
     */
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override {
        return inet::NUM_INIT_STAGES;
    }

    /**
     * @brief This module should receive any messages.
     *
     * @param msg Parameter
     */
    virtual void handleMessage(cMessage *msg) override;

    virtual void handleParameterChange(const char* parname) override;

    /**
     * Finds the connector for the publisher
     * @param publisherServiceId     the service id of the publisher
     * @return                       the connector if found, else nullptr
     */
    PublisherConnector* getPublisherConnectorForServiceId (uint32_t publisherServiceId);

    /**
     * Finds the connector for the subscriber
     * @param publisherServiceId     the service id of the publisher
     * @return                       the connector if found, else nullptr
     */
    SubscriberConnector* getSubscriberConnectorForServiceId (uint32_t publisherServiceId);

    /**
     * Searches for a subscriber connector on this node for the given name and QoS.
     *
     * @param publisherServiceId     the service id of the publisher.
     * @param qos                    the QoS of the publisher.
     * @return                       the subscriber if found, else nullptr.
     */
    SubscriberConnector* findSubscriberConnectorLike(uint32_t publisherServiceId, int qos);

    /**
     * Tries to find a publisher on this node for the given name and QoS.
     * If it can't be found a new one is created.
     * The connector will be connected as well.
     *
     * @param publisherServiceId     the service id of the publisher.
     * @param csi                    the csi of the publisher.
     * @return                       the corresponding publisher
     */
    PublisherEndpointBase* createOrFindPublisherFor(uint32_t publisherServiceId,  int qos);

    /**
     * Tries to find a subscriber on this node for the given publisher name and QoS.
     * If it can't be found a new one is created.
     * The connector will be connected as well.
     *
     * @param publisherPath     the service id of the publisher.
     * @param csi               the csi of the publisher.
     * @return                  the corresponding publisher
     */
    SubscriberEndpointBase* createOrFindSubscriberFor(uint32_t publisherServiceId, ConnectionSpecificInformation* csi);

    /**
     * Searches for a publisher on this node for the given name and QoS.
     *
     * @param publisherServiceId     the service id of the publisher.
     * @param qos                    the QoS of the publisher.
     * @return                       the publisher if found, else nullptr.
     */
    PublisherEndpointBase* findPublisherLike(uint32_t publisherServiceId, int qos);

    /**
     * Searches for a subscriber on this node for the given name and QoS.
     *
     * @param publisherServiceId     the service id of the publisher.
     * @param qos                    the QoS of the publisher.
     * @return                       the corresponding publisher
     */
    SubscriberEndpointBase* findSubscriberLike(uint32_t publisherServiceId, int qos);


    /**
     * Contains pointers to the existing publisher connectors on a node.
     */
    std::map<IServiceRegistry::ServiceId, PublisherConnector*> _publisherConnectors;

    /**
     * Counter for publishing endpoints created.
     */
    int _publisherEndpointCount = 0;

    /**
     * contains pointers to the existing subscriber connectors on a node.
     * TODO maybe we need to allow more than one subscriber connector per publisher service for different QoS?
     * Or we take the best QoS needed on device?
     */
    std::map<IServiceRegistry::ServiceId, SubscriberConnector*> _subscriberConnectors;

    /**
     * Contains pending requests
     */
    std::map<IServiceRegistry::ServiceId, std::list<QoSService>> _pendingRequestsMap;

    /**
     * Counter for subscribing endpoints created.
     */
    int _subscriberEndpointCount = 0;

    /**
     * A pointer to the service discovery.
     */
    IServiceDiscovery* _sd;

    /**
     * A pointer to the local service registry.
     */
    IServiceRegistry* _lsr;

    /**
     * A pointer to the QoS Negotiation Protocol module.
     */
    QoSNegotiationProtocol* _qosnp;

    /**
     * Static ID for created Requests.
     */
    std::atomic<int> _requestID;

    /**
     * Stores all issued requests.
     */
    std::vector<Request*> _requests;

    /**
     * Caches the localAddress parameter.
     */
    inet::L3Address _localAddress;

private:

    /**
     * Adds publisher service to a given publisher connector
     * @param publisherConnector
     * @param qosPolicies
     * @param executingApplication
     */
    void addPublisherServiceToConnector(PublisherConnector* publisherConnector, QoSPolicyMap& qosPolicies, SOQoSMWApplicationBase* executingApplication);

    /**
     * Adds subscriber service to a given subscriber connector
     * @param subscriberConnector
     * @param qosPolicies
     * @param executingApplication
     */
    void addSubscriberServiceToConnector(SubscriberConnector* subscriberConnector, QoSPolicyMap& qosPolicies, SOQoSMWApplicationBase* executingApplication);

    /**
     * Creates a negotiation request
     * @param publisherService
     * @param qosPolicies
     * @return the negotiation request
     */
    Request* createNegotiationRequest(IService* publisherService, QoSPolicyMap qosPolicies);

    /**
     * Creates a publisher connector
     * @param qosPolicies
     * @param executingApplication
     * @return the publisher connector module
     */
    PublisherConnector* createPublisherConnector(QoSPolicyMap& qosPolicies, SOQoSMWApplicationBase* executingApplication);

    /**
     * Creates a subscriber connector
     * @param qosPolicies
     * @param executingApplication
     * @return the subscriber connector module
     */
    SubscriberConnector* createSubscriberConnector(QoSPolicyMap& qosPolicies, SOQoSMWApplicationBase* executingApplication);

    /**
     * Matches the connection type to the qos group
     * @param type      connection type. @see~ConnectionType
     * @return          the qos group. @see~QoSGroups
     */
    int getQoSGroupForConnectionType(int type);

    SubscriberEndpointBase* createAVBSubscriberEndpoint(ConnectionSpecificInformation* csi, SubscriberConnector* connector);
    SubscriberEndpointBase* createTCPSubscriberEndpoint(ConnectionSpecificInformation* csi, SubscriberConnector* connector);
    SubscriberEndpointBase* createUDPSubscriberEndpoint(ConnectionSpecificInformation* csi, SubscriberConnector* connector);
    SubscriberEndpointBase* createSOMEIPSubscriberEndpoint(ConnectionSpecificInformation* csi, SubscriberConnector* connector);

    PublisherEndpointBase* createAVBPublisherEndpoint(int qos, PublisherConnector* connector);
    PublisherEndpointBase* createTCPPublisherEndpoint(int qos, PublisherConnector* connector);
    PublisherEndpointBase* createUDPPublisherEndpoint(int qos, PublisherConnector* connector);
    PublisherEndpointBase* createSOMEIPPublisherEndpoint(int qos, PublisherConnector* connector);
};

} /* end namespace  */

#endif
