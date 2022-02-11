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

#ifndef __SOA4CORE_SERVICEMANAGER_LOCALSERVICEMANAGER_H_
#define __SOA4CORE_SERVICEMANAGER_LOCALSERVICEMANAGER_H_

#include <omnetpp.h>
#include "soa4core/service/publisherapplicationinformation/PublisherApplicationInformation.h"
#include "soa4core/service/subscriberapplicationinformation/SubscriberApplicationInformation.h"
#include "soa4core/qosmanagement/negotiation/datatypes/Request.h"
#include "soa4core/connector/pubsub/reader/SubscriberConnector.h"
#include "soa4core/connector/pubsub/writer/PublisherConnector.h"
#include "soa4core/endpoints/publisher/base/PublisherEndpointBase.h"
#include "soa4core/endpoints/subscriber/base/SubscriberEndpointBase.h"
#include "soa4core/applications/base/MiddlewareApplicationBase.h"
#include "soa4core/serviceregistry/base/IServiceRegistry.h"
#include "soa4core/servicemanager/base/ILocalServiceManager.h"
#include <atomic>
#include <string>
#include <map>
#include <vector>
//AUTO-GENERATED MESSAGES
#include "soa4core/messages/qosnegotiation/ConnectionSpecificInformation_m.h"
//INET
#include "inet/common/InitStages.h"

using namespace omnetpp;

namespace SOA4CoRE {

/**
 * @brief The LocalServiceManager creates, finds and removes local Services.
 *
 * @ingroup soa4core/servicemanager
 *
 * @author Timo Haeckel and Mehmet Mueller for HAW Hamburg
 */
class LocalServiceManager: public ILocalServiceManager, public cSimpleModule, public cListener {

    friend class QoSBroker;

    /**
     * Methods
     */
public:
    LocalServiceManager();
    virtual ~LocalServiceManager();

    /**
     * @brief This Method creates a new Publisher according to the QoSPolicies.
     *
     * @param publisherApplicationInformation The application informations of the Publisher application (e.g. "reifendruck/links")
     * @param executingModule The application publishing a service.
     */
    void registerPublisherService(PublisherApplicationInformation publisherApplicationInformation, MiddlewareApplicationBase* executingApplication);

    /**
     * @brief This Method creates a new Subscriber for the publisher Service according to the QoSPolicies.
     *
     * @param subscriberApplicationInformation The application informations of the Subscriber application (e.g. "bordcomputer")
     * @param executingModule The application executing the request.
     */
    void registerSubscriberService(SubscriberApplicationInformation subscriberApplicationInformation, MiddlewareApplicationBase* executingApplication);

    /**
     * @brief Subscribes the given service
     * @param publisherServiceIdentifier service identifier of the service to be subscribed to
     * @param subscriberApplicationInformation the subscriber application information
     */
    virtual void subscribeService(QoSServiceIdentifier publisherServiceIdentifier, SubscriberApplicationInformation subscriberApplicationInformation) override;

    /**
     * Finds the connector for the publisher
     * @param publisherServiceId     the service id of the publisher
     * @return                       the connector if found, else nullptr
     */
    PublisherConnector* getPublisherConnectorForServiceId (uint32_t publisherServiceId);

    /**
     * Finds the connector for the subscriber
     * @param publisherServiceId     the service id of the publisher
     * @return                       the connectors if found, else empty vector
     */
    std::vector<SubscriberConnector*> getSubscriberConnectorsForServiceId (uint32_t publisherServiceId);

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
     * Searches for a subscriber connector on this node for the given name and QoS.
     *
     * @param publisherServiceId     the service id of the publisher.
     * @param qosGroup               the QoS of the publisher.
     * @return                       the subscriber if found, else nullptr.
     */
    SubscriberConnector* findSubscriberConnectorLike(uint32_t publisherServiceId, QoSGroup qosGroup);

    /**
     * Tries to find a publisher on this node for the given name and QoS.
     * If it can't be found a new one is created.
     * The connector will be connected as well.
     *
     * @param publisherServiceId     the service id of the publisher.
     * @param qosGroup               the qosGroup of the publisher.
     * @return                       the corresponding publisher
     */
    PublisherEndpointBase* createOrFindPublisherFor(uint32_t publisherServiceId,  QoSGroup qosGroup);

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
     * @param qosGroup               the QoS of the publisher.
     * @return                       the publisher if found, else nullptr.
     */
    PublisherEndpointBase* findPublisherLike(uint32_t publisherServiceId, QoSGroup qosGroup);

    /**
     * Searches for a subscriber on this node for the given name and QoS.
     *
     * @param publisherServiceId     the service id of the publisher.
     * @param qosGroup               the QoS of the publisher.
     * @return                       the corresponding publisher
     */
    SubscriberEndpointBase* findSubscriberLike(uint32_t publisherServiceId, QoSGroup qosGroup);

    /**
     * Adds an subscriber to the pendingRequestMap if the requested service is not known yet
     * @param publisherServiceIdentifier the service id of the publisher service
     * @param qosService the QoS service
     */
    void addSubscriberToPendingRequestsMap(QoSServiceIdentifier publisherServiceIdentifier, SubscriberApplicationInformation subscriberApplicationInformation);

private:

    /**
     * Adds subscriber service to a given subscriber connector
     * @param subscriberConnector
     * @param executingApplication
     */
    void addSubscriberServiceToConnector(SubscriberConnector* subscriberConnector, MiddlewareApplicationBase* executingApplication);

    /**
     * Creates a publisher connector
     * @param publisherApplicationInformation
     * @param executingApplication
     * @return the publisher connector module
     */
    PublisherConnector* createPublisherConnector(PublisherApplicationInformation publisherApplicationInformation, MiddlewareApplicationBase* executingApplication);

    /**
     * Creates a subscriber connector
     * @param qosService
     * @param executingApplication
     * @return the subscriber connector module
     */
    SubscriberConnector* createSubscriberConnector(SubscriberApplicationInformation subscriberApplicationInformation, MiddlewareApplicationBase* executingApplication);

    /**
     * Matches the connection type to the qos group
     * @param  connectionType @see~ConnectionType
     * @return the qos group. @see~QoSGroups
     */
    QoSGroup getQoSGroupForConnectionType(ConnectionType connectionType);

    SubscriberEndpointBase* createAVBSubscriberEndpoint(ConnectionSpecificInformation* csi, SubscriberConnector* connector);
    SubscriberEndpointBase* createTCPSubscriberEndpoint(ConnectionSpecificInformation* csi, SubscriberConnector* connector);
    SubscriberEndpointBase* createUDPSubscriberEndpoint(ConnectionSpecificInformation* csi, SubscriberConnector* connector);
    SubscriberEndpointBase* createSomeIpTCPSubscriberEndpoint(ConnectionSpecificInformation* csi, SubscriberConnector* connector);
    SubscriberEndpointBase* createSomeIpUDPSubscriberEndpoint(ConnectionSpecificInformation* csi, SubscriberConnector* connector);

    PublisherEndpointBase* createAVBPublisherEndpoint(QoSGroup qosGroup, PublisherConnector* connector);
    PublisherEndpointBase* createTCPPublisherEndpoint(QoSGroup qosGroup, PublisherConnector* connector);
    PublisherEndpointBase* createUDPPublisherEndpoint(QoSGroup qosGroup, PublisherConnector* connector);
    PublisherEndpointBase* createSomeIpTCPPublisherEndpoint(QoSGroup qosGroup, PublisherConnector* connector);
    PublisherEndpointBase* createSomeIpUDPPublisherEndpoint(QoSGroup qosGroup, PublisherConnector* connector);


    /**
     * Member variables
     */
public:
protected:
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
     * Update: We currently allow more than one subscriber connector
     */
    std::map<IServiceRegistry::ServiceId, std::vector<SubscriberConnector*>> _subscriberConnectors;

    /**
     * Counter for subscribing endpoints created.
     */
    int _subscriberEndpointCount = 0;

    /**
     * A pointer to the local service registry.
     */
    IServiceRegistry* _lsr;

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

    /**
     * Contains pending requests
     */
    std::map<IServiceRegistry::ServiceId, std::list<SubscriberApplicationInformation>> _pendingRequestsMap;
private:

};

} /* end namespace SOA4CoRE */

#endif
