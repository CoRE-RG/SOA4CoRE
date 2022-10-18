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

#include "soa4core/applications/base/ServiceBase.h"
#include "soa4core/applications/publisher/base/Publisher.h"
#include "soa4core/applications/subscriber/base/Subscriber.h"
#include "soa4core/manager/base/IManager.h"
#include "soa4core/registry/base/Registry.h"
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
     * @brief Registers a new Publisher and returns its connector.
     *
     * @param publisherApplication The application publishing a service.
     * @return the publisher connector
     */
    virtual PublisherConnector* registerPublisherService(ServiceBase* publisherApplication) override;

    /**
     * Registers the given Subscriber application and subscribes the service given in the publisher service identifier
     * @param publisherServiceIdentifier the publisher service identifier
     * @param subscriberApplication the subscriber application
     */
    virtual SubscriberConnector* registerSubscriberServiceAndSubscribeService(ServiceIdentifier publisherServiceIdentifier, ServiceBase* subscriberApplication) override;

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
     * @brief Tries to find a publisher endpoint on this node for the given publisher service id and QoS.
     * If it can't be found, a new one is created.
     * The connector will be connected as well.
     *
     * @param publisherServiceId     the service id of the publisher.
     * @param qosGroup               the qosGroup of the publisher.
     * @return                       the corresponding publisher endpoint if found, else nullptr.
     */
    PublisherEndpointBase* createOrFindPublisherEndpoint(uint32_t publisherServiceId,  QoSGroup qosGroup);

    /**
     * @brief Tries to find a subscriber endpoint on this node for the given publisher service id
     * and Connection Specific Information (CSI).
     * If it can't be found a new one is created.
     * The connector will be connected as well.
     *
     * @param publisherServiceId    the service id of the publisher.
     * @param csi                   the CSI of the publisher.
     * @return                      the corresponding subscriber endpoint, else nullptr.
     */
    SubscriberEndpointBase* createOrFindSubscriberEndpoint(uint32_t publisherServiceId, ConnectionSpecificInformation* csi);

    /**
     * @brief Searches for a publisher endpoint on this node for the given service id and QoS.
     *
     * @param publisherServiceId     the service id of the publisher.
     * @param qosGroup               the QoS group of the publisher.
     * @return                       the publisher endpoint if found, else nullptr.
     */
    PublisherEndpointBase* findPublisherEndpoint(uint32_t publisherServiceId, QoSGroup qosGroup);

    /**
     * @brief Searches for a subscriber endpoint on this node for the given service id and QoS.
     *
     * @param publisherServiceId     the service id of the publisher.
     * @param qosGroup               the QoS group of the publisher.
     * @return                       the subscriber endpoint if found, else nullptr.
     */
    SubscriberEndpointBase* findSubscriberEndpoint(uint32_t publisherServiceId, QoSGroup qosGroup);

    /**
     * @brief Searches for a subscriber connector on this node for the given service id and QoS.
     *
     * @param publisherServiceId     the service id of the publisher.
     * @param qosGroup               the QoS group of the publisher.
     * @return                       the subscriber connector if found, else nullptr.
     */
    SubscriberConnector* findSubscriberConnector(uint32_t publisherServiceId, QoSGroup qosGroup);

    /**
     * @brief Dispatcher method that calls the correct QoS dependent create subscriber function.
     *
     * @param csi the csi
     * @param subscriberConnector the subscriber connector
     * @return the subscriber endpoint
     */
    virtual SubscriberEndpointBase* createConnectionSpecificSubscriberEndpoint(ConnectionSpecificInformation* csi, SubscriberConnector* subscriberConnector);

    /**
     * @brief Dispatcher method that calls the correct QoS dependent create publisher function.
     *
     * @param qosGroup the QoS group
     * @param publisherConnector the publisher connector
     * @return the publisher endpoint
     */
    virtual PublisherEndpointBase* createQoSSpecificPublisherEndpoint(QoSGroup qosGroup, PublisherConnector* publisherConnector);

    /**
     * Returns the QoS group for the given connection type
     * @param  connectionType @see~ConnectionType
     * @return the qos group. @see~QoSGroups
     */
    virtual QoSGroup getQoSGroupForConnectionType(ConnectionType connectionType);

    /**
     * @brief Initiates a service discovery for the given service
     *
     * @param publisherServiceIdentifier service identifier of the service to be subscribed to
     * @param subscriberApplication the subscriber application
     */
    virtual void discoverService(ServiceIdentifier publisherServiceIdentifier, ServiceBase* subscriberApplication) override;

private:
    /**
     * @brief Adds a subscriber service to a given subscriber connector.
     *
     * @param subscriberConnector the subscriber connector.
     * @param subscriberApplication the subscriber  application.
     */
    void addSubscriberServiceToConnector(SubscriberConnector* subscriberConnector, ServiceBase* subscriberApplication);

    /**
     * @brief Creates a publisher connector.
     *
     * @param publisherApplication the publisher application.
     * @return the publisher connector module.
     */
    PublisherConnector* createPublisherConnector(Publisher* publisherApplication);

    /**
     * @brief Creates a subscriber connector.
     *
     * @param subscriberApplication the subscriber service application.
     * @return the subscriber connector module.
     */
    SubscriberConnector* createSubscriberConnector(ServiceBase* subscriberApplication);

    /**
     * @brief Creates an AVB subscriber endpoint
     *
     * @param csi the csi
     * @param subscriberConnector the subscriber connector
     * @return the AVB subscriber endpoint
     */
    SubscriberEndpointBase* createAVBSubscriberEndpoint(ConnectionSpecificInformation* csi, SubscriberConnector* subscriberConnector);

    /**
     * @brief Creates a TCP subscriber endpoint
     *
     * @param csi the csi
     * @param subscriberConnector the subscriber connector
     * @return the TCP subscriber endpoint
     */
    SubscriberEndpointBase* createTCPSubscriberEndpoint(ConnectionSpecificInformation* csi, SubscriberConnector* subscriberConnector);

    /**
     * @brief Creates an UDP subscriber endpoint
     *
     * @param csi the csi
     * @param subscriberConnector the subscriber connector
     * @return the UDP subscriber endpoint
     */
    SubscriberEndpointBase* createUDPSubscriberEndpoint(ConnectionSpecificInformation* csi, SubscriberConnector* subscriberConnector);

    /**
     * @brief Creates an UDP multicast subscriber endpoint
     *
     * @param csi the csi
     * @param subscriberConnector the subscriber connector
     * @return the UDP mcast subscriber endpoint
     */
    SubscriberEndpointBase* createUDPMcastSubscriberEndpoint(ConnectionSpecificInformation* csi, SubscriberConnector* subscriberConnector);

    /**
     * @brief Creates an AVB publisher endpoint
     *
     * @param qosGroup the QoS group
     * @param publisherConnector the publisher connector
     * @return the AVB publisher endpoint
     */
    PublisherEndpointBase* createAVBPublisherEndpoint(QoSGroup qosGroup, PublisherConnector* publisherConnector);

    /**
     * @brief Creates a TCP publisher endpoint
     *
     * @param qosGroup the QoS group
     * @param publisherConnector the publisher connector
     * @return the TCP publisher endpoint
     */
    PublisherEndpointBase* createTCPPublisherEndpoint(QoSGroup qosGroup, PublisherConnector* publisherConnector);

    /**
     * @brief Creates a UDP publisher endpoint
     *
     * @param qosGroup the QoS group
     * @param publisherConnector the publisher connector
     * @return the UDP publisher endpoint
     */
    PublisherEndpointBase* createUDPPublisherEndpoint(QoSGroup qosGroup, PublisherConnector* publisherConnector);

    /**
     * @brief Creates a UDP multicast publisher endpoint
     *
     * @param qosGroup the QoS group
     * @param publisherConnector the publisher connector
     * @return the UDP mcast publisher endpoint
     */
    PublisherEndpointBase* createUDPMcastPublisherEndpoint(QoSGroup qosGroup, PublisherConnector* publisherConnector);

/**
 * Member variables
 */
public:
protected:
    /**
     * Counter for subscribing endpoints created.
     */
    int _subscriberEndpointCount = 0;

    /**
     * Counter for publishing endpoints created.
     */
    int _publisherEndpointCount = 0;

    /**
     * A pointer to the local service registry.
     */
    Registry* _lsr;

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

};

} /* end namespace SOA4CoRE */

#endif
