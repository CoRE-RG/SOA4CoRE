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

#ifndef __SOA4CORE_SERVICEMANAGER_QOS_QOSMANAGER_H_
#define __SOA4CORE_SERVICEMANAGER_QOS_QOSMANAGER_H_

#include "soa4core/manager/base/Manager.h"
#include "soa4core/discovery/base/IServiceDiscovery.h"
#include "soa4core/qosmanagement/negotiation/QoSNegotiationProtocol.h"
#include "soa4core/qosmanagement/negotiation/datatypes/Request.h"
//AUTO-GENERATED MESSAGES
#include "soa4core/messages/qosnegotiation/ConnectionSpecificInformation_m.h"
//OMNETPP
#include <omnetpp.h>

using namespace omnetpp;
namespace SOA4CoRE {
/**
 * @brief The QoSManager creates, finds and removes local QoS services.
 *
 * @ingroup soa4core/manager
 *
 * @author Mehmet Mueller for HAW Hamburg
 */
class QoSManager : public Manager
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
     * @param details the object details
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
     * @brief Initiates a service discovery for the given service
     *
     * @param publisherServiceIdentifier service identifier of the service to be subscribed to
     * @param subscriberApplication the subscriber application
     */
    virtual void discoverService(ServiceIdentifier publisherServiceIdentifier, ServiceBase* subscriberApplication) override;

    /**
     * @brief Dispatcher method that calls the correct QoS dependent create subscriber function.
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

private:
    /**
     * Creates a negotiation request
     * @param serviceId the service ID
     * @param publisherAddress the publisher ip address on the remote node
     * @param qosGroup the QoS group
     * @return the negotiation request
     */
    Request* createNegotiationRequest(Registry::ServiceId serviceId, inet::L3Address publisherAddress, QoSGroup qosGroup);

    /**
     * Subscribes to the offered service
     * @param obj the publisher application information
     */
    void subscribeOfferedService(cObject* obj);

    /**
     * Looks for a requested service in the local registry
     * @param obj the SomeIpSDFindRequest
     */
    void lookForService(cObject* obj);

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
     * The signal which is emitted when a requested service is found
     */
    omnetpp::simsignal_t _findResultSignal;
private:
    /**
     * The QoS Negotiation Protocol module.
     */
    QoSNegotiationProtocol* _qosnp;

    /**
     * The service discovery.
     */
    IServiceDiscovery* _sd;

    /**
     * Static ID for created Requests.
     */
    std::atomic<int> _requestID;

    /**
     * Contains pending requests
     */
    std::list<std::pair<Registry::ServiceId, QoSGroup>> _pendingRequests;

};
} /* end namespace SOA4CoRE */
#endif
