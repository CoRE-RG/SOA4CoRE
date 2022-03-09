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

#ifndef __SOA4CORE_SERVICEMANAGER_MANAGER_H_
#define __SOA4CORE_SERVICEMANAGER_MANAGER_H_

#include "soa4core/applicationinformation/publisher/PublisherApplicationInformation.h"
#include "soa4core/applicationinformation/subscriber/SubscriberApplicationInformation.h"
#include "soa4core/applications/base/ServiceBase.h"
#include "soa4core/connector/publisher/PublisherConnector.h"
#include "soa4core/connector/subscriber/SubscriberConnector.h"
#include "soa4core/manager/base/IManager.h"
#include "soa4core/registry/base/IRegistry.h"
#include "soa4core/qosmanagement/negotiation/datatypes/Request.h"
#include "soa4core/endpoints/publisher/base/PublisherEndpointBase.h"
#include "soa4core/endpoints/subscriber/base/SubscriberEndpointBase.h"
//AUTO-GENERATED MESSAGES
#include "soa4core/messages/qosnegotiation/ConnectionSpecificInformation_m.h"
//OMNETPP
#include <omnetpp.h>
//INET
#include <inet/common/InitStages.h>
//STD
#include <atomic>
#include <string>
#include <map>
#include <vector>

using namespace omnetpp;

namespace SOA4CoRE {

/**
 * @brief The Manager creates, finds and removes local Services.
 *
 * @ingroup soa4core/manager
 *
 * @author Timo Haeckel and Mehmet Mueller for HAW Hamburg
 */
class Manager: public IManager, public cSimpleModule, public cListener {

friend class QoSBroker;

/**
 * Methods
 */
public:
    Manager();
    virtual ~Manager();

    /**
     * @brief This Method registers a new Publisher according to its application information.
     *
     * @param publisherApplicationInformation The application informations of the Publisher application (e.g. "reifendruck/links")
     * @param executingModule The application publishing a service.
     */
    void registerPublisherService(PublisherApplicationInformation publisherApplicationInformation, ServiceBase* executingApplication);

    /**
     * @brief This Method registers a new Subscriber according to its application information.
     * It adds the application to a possibly created or already present connector.
     *
     * @param subscriberApplicationInformation The application informations of the Subscriber application (e.g. "bordcomputer")
     * @param executingModule The application executing the request.
     */
    void registerSubscriberService(SubscriberApplicationInformation subscriberApplicationInformation, ServiceBase* executingApplication);

    /**
     * @brief Subscribes the given service
     * @param publisherServiceIdentifier service identifier of the service to be subscribed to
     * @param subscriberApplicationInformation the subscriber application information
     */
    virtual void subscribeService(ServiceIdentifier publisherServiceIdentifier, SubscriberApplicationInformation subscriberApplicationInformation) override;

    /**
     * @brief Finds the connector for the publisher
     *
     * @param publisherServiceId     the service id of the publisher
     * @return                       the connector if found, else nullptr
     */
    PublisherConnector* getPublisherConnectorForServiceId (uint32_t publisherServiceId);

    /**
     * @brief Finds the connector for the subscriber
     *
     * @param publisherServiceId     the service id of the publisher
     * @return                       the connectors if found, else empty vector
     */
    std::vector<SubscriberConnector*> getSubscriberConnectorsForServiceId (uint32_t publisherServiceId);

protected:
    /**
     * @brief Initialization of the module.
     *
     * @param stage the initialization stage
     */
    virtual void initialize(int stage) override;

    /**
     * @brief Returns the maximum number of initialization stages
     *
     * @return the maximum number of initialization stages
     */
    virtual int numInitStages() const override {
        return inet::NUM_INIT_STAGES;
    }

    /**
     * @brief This module should receive any messages.
     *
     * @param msg Parameter
     */
    virtual void handleMessage(cMessage *msg) override;

    /**
     * @brief Indicates a parameter has changed.
     *
     * @param parname Name of the changed parameter or nullptr if multiple parameter changed.
     */
    virtual void handleParameterChange(const char* parname) override;

    /**
     * @brief Searches for a subscriber connector on this node for the given service id and QoS group.
     *
     * @param publisherServiceId     the service id of the publisher.
     * @param qosGroup               the QoS group of the publisher.
     * @return                       the subscriber if found, else nullptr.
     */
    SubscriberConnector* findSubscriberConnectorLike(uint32_t publisherServiceId, QoSGroup qosGroup);

    /**
     * @brief Tries to find a publisher on this node for the given name and QoS.
     * If it can't be found, a new one is created.
     * The connector will be connected as well.
     *
     * @param publisherServiceId     the service id of the publisher.
     * @param qosGroup               the qosGroup of the publisher.
     * @return                       the corresponding publisher if found, else nullptr.
     */
    PublisherEndpointBase* createOrFindPublisherFor(uint32_t publisherServiceId,  QoSGroup qosGroup);

    /**
     * @brief Tries to find a subscriber on this node for the given publisher service id and QoS group.
     * If it can't be found a new one is created.
     * The connector will be connected as well.
     *
     * @param publisherServiceId    the service id of the publisher.
     * @param csi                   the csi of the publisher.
     * @return                      the corresponding publisher, else nullptr.
     */
    SubscriberEndpointBase* createOrFindSubscriberFor(uint32_t publisherServiceId, ConnectionSpecificInformation* csi);

    /**
     * @brief Searches for a publisher on this node for the given service id and QoS.
     *
     * @param publisherServiceId     the service id of the publisher.
     * @param qosGroup               the QoS group of the publisher.
     * @return                       the publisher if found, else nullptr.
     */
    PublisherEndpointBase* findPublisherLike(uint32_t publisherServiceId, QoSGroup qosGroup);

    /**
     * @brief Searches for a subscriber on this node for the given service id and QoS.
     *
     * @param publisherServiceId     the service id of the publisher.
     * @param qosGroup               the QoS group of the publisher.
     * @return                       the corresponding publisher
     */
    SubscriberEndpointBase* findSubscriberLike(uint32_t publisherServiceId, QoSGroup qosGroup);

    /**
     * @brief Adds an subscriber to the pendingRequestMap if the requested service is not known yet.
     *
     * @param publisherServiceIdentifier the service id of the publisher service.
     * @param subscriberApplicationInformation the application information of the subscriber.
     */
    void addSubscriberToPendingRequestsMap(ServiceIdentifier publisherServiceIdentifier, SubscriberApplicationInformation subscriberApplicationInformation);

private:
    /**
     * @brief Adds a subscriber service to a given subscriber connector.
     *
     * @param subscriberConnector the subscriber connector.
     * @param executingApplication the subscriber service application.
     */
    void addSubscriberServiceToConnector(SubscriberConnector* subscriberConnector, ServiceBase* executingApplication);

    /**
     * @brief Creates a publisher connector.
     *
     * @param publisherApplicationInformation the publisher application information.
     * @param executingApplication the publisher service application.
     * @return the publisher connector module.
     */
    PublisherConnector* createPublisherConnector(PublisherApplicationInformation publisherApplicationInformation, ServiceBase* executingApplication);

    /**
     * @brief Creates a subscriber connector.
     *
     * @param subscriberApplicationInformation the subscriber application information.
     * @param executingApplication the subscriber service application.
     * @return the subscriber connector module.
     */
    SubscriberConnector* createSubscriberConnector(SubscriberApplicationInformation subscriberApplicationInformation, ServiceBase* executingApplication);

    /**
     * Returns the QoS group for the given connection type
     * @param  connectionType @see~ConnectionType
     * @return the qos group. @see~QoSGroups
     */
    QoSGroup getQoSGroupForConnectionType(ConnectionType connectionType);

    /**
     * @brief Creates an AVB subscriber endpoint
     *
     * @param csi the csi
     * @param connector the subscriber connector
     * @return the AVB subscriber endpoint
     */
    SubscriberEndpointBase* createAVBSubscriberEndpoint(ConnectionSpecificInformation* csi, SubscriberConnector* connector);

    /**
     * @brief Creates a TCP subscriber endpoint
     *
     * @param csi the csi
     * @param connector the subscriber connector
     * @return the TCP subscriber endpoint
     */
    SubscriberEndpointBase* createTCPSubscriberEndpoint(ConnectionSpecificInformation* csi, SubscriberConnector* connector);

    /**
     * @brief Creates an UDP subscriber endpoint
     *
     * @param csi the csi
     * @param connector the subscriber connector
     * @return the UDP subscriber endpoint
     */
    SubscriberEndpointBase* createUDPSubscriberEndpoint(ConnectionSpecificInformation* csi, SubscriberConnector* connector);

    /**
     * @brief Creates a SOME/IP TCP subscriber endpoint
     *
     * @param csi the csi
     * @param connector the subscriber connector
     * @return the SOME/IP TCP subscriber endpoint
     */
    SubscriberEndpointBase* createSomeIpTCPSubscriberEndpoint(ConnectionSpecificInformation* csi, SubscriberConnector* connector);

    /**
     * @brief Creates a SOME/IP UDP subscriber endpoint
     *
     * @param csi the csi
     * @param connector the subscriber connector
     * @return the SOME/IP UDP subscriber endpoint
     */
    SubscriberEndpointBase* createSomeIpUDPSubscriberEndpoint(ConnectionSpecificInformation* csi, SubscriberConnector* connector);


    /**
     * @brief Creates an AVB publisher endpoint
     *
     * @param qosGroup the QoS group
     * @param connector the publisher connector
     * @return the AVB publisher endpoint
     */
    PublisherEndpointBase* createAVBPublisherEndpoint(QoSGroup qosGroup, PublisherConnector* connector);

    /**
     * @brief Creates a TCP publisher endpoint
     *
     * @param qosGroup the QoS group
     * @param connector the publisher connector
     * @return the TCP publisher endpoint
     */
    PublisherEndpointBase* createTCPPublisherEndpoint(QoSGroup qosGroup, PublisherConnector* connector);

    /**
     * @brief Creates a UDP publisher endpoint
     *
     * @param qosGroup the QoS group
     * @param connector the publisher connector
     * @return the UDP publisher endpoint
     */
    PublisherEndpointBase* createUDPPublisherEndpoint(QoSGroup qosGroup, PublisherConnector* connector);

    /**
     * @brief Creates a SOME/IP TCP publisher endpoint
     *
     * @param qosGroup the QoS group
     * @param connector the publisher connector
     * @return the SOME/IP TCP publisher endpoint
     */
    PublisherEndpointBase* createSomeIpTCPPublisherEndpoint(QoSGroup qosGroup, PublisherConnector* connector);

    /**
     * @brief Creates a SOME/IP UDP publisher endpoint
     *
     * @param qosGroup the QoS group
     * @param connector the publisher connector
     * @return the SOME/IP UDP publisher endpoint
     */
    PublisherEndpointBase* createSomeIpUDPPublisherEndpoint(QoSGroup qosGroup, PublisherConnector* connector);


/**
 * Member variables
 */
public:
protected:
    /**
     * Contains pointers to the existing publisher connectors on a node.
     */
    std::map<IRegistry::ServiceId, PublisherConnector*> _publisherConnectors;

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
    std::map<IRegistry::ServiceId, std::vector<SubscriberConnector*>> _subscriberConnectors;

    /**
     * Counter for subscribing endpoints created.
     */
    int _subscriberEndpointCount = 0;

    /**
     * A pointer to the local service registry.
     */
    IRegistry* _lsr;

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
    std::map<IRegistry::ServiceId, std::list<SubscriberApplicationInformation>> _pendingRequestsMap;
private:

};

} /* end namespace SOA4CoRE */

#endif
