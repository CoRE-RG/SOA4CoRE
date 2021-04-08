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
#include "soqosmw/discovery/someipservicediscovery/SomeIpSD.h"
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
 * @brief The LocalServiceManager is used to create, find and remove local Services.
 *
 * @ingroup soqosmw/servicemanager
 *
 * @author Timo Haeckel and Mehmet Cakir for HAW Hamburg
 */
class LocalServiceManager: public cSimpleModule {
    friend QoSNegotiationProtocol;
    friend QoSBroker;

public:
    LocalServiceManager();
    virtual ~LocalServiceManager();

    /**
     * @brief This Method creates a new Publisher according to the QoSPolicies.
     *
     * @param publisherPath Path of the Publisher Service (e.g. "reifendruck/links")
     * @param qosPolicies The QoS Policies for the Publisher.
     * @param executingModule The service executing the request.
     */
    void registerPublisherService(std::string& publisherPath,
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
    void registerSubscriberService(std::string& subscriberPath,
            std::string& publisherPath,
            QoSPolicyMap& qosPolicies,
            SOQoSMWApplicationBase* executingApplication);

    /**
     * Returns the publisher connector for the given publisher service
     * @param publisherPath
     * @return corresponding publisher connector
     */
    PublisherConnector* getPublisherConnector(std::string& publisherPath);

    /**
     * Returns the subscriber connector for the given publisher service
     * @param publisherPath
     * @return corresponding subscriber connector
     */
    SubscriberConnector* getSubscriberConnector(std::string& publisherPath);

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
     * @param publisherPath     the name of the publisher as a path
     * @return                  the connector if found, else nullptr
     */
    PublisherConnector* getPublisherConnectorForName (std::string& publisherPath);

    /**
     * Finds the connector for the subscriber
     * @param publisherPath     the name of the publisher as a path
     * @return                  the connector if found, else nullptr
     */
    SubscriberConnector* getSubscriberConnectorForName (std::string& publisherPath);

    /**
     * Searches for a subscriber connector on this node for the given name and QoS.
     *
     * @param publisherPath     the path of the publisher.
     * @param qos               the QoS of the publisher.
     * @return                  the subscriber if found, else nullptr.
     */
    SubscriberConnector* findSubscriberConnectorLike(std::string& publisherPath, int qos);

    /**
     * Tries to find a publisher on this node for the given name and QoS.
     * If it can't be found a new one is created.
     * The connector will be connected as well.
     *
     * @param publisherPath     the path of the publisher.
     * @param csi               the csi of the publisher.
     * @return                  the corresponding publisher
     */
    PublisherEndpointBase* createOrFindPublisherFor(std::string& publisherPath,  int qos);

    /**
     * Tries to find a subscriber on this node for the given publisher name and QoS.
     * If it can't be found a new one is created.
     * The connector will be connected as well.
     *
     * @param publisherPath     the path of the publisher.
     * @param csi               the csi of the publisher.
     * @return                  the corresponding publisher
     */
    SubscriberEndpointBase* createOrFindSubscriberFor(std::string& publisherPath, ConnectionSpecificInformation* csi);

    /**
     * Searches for a publisher on this node for the given name and QoS.
     *
     * @param publisherPath     the path of the publisher.
     * @param qos               the QoS of the publisher.
     * @return                  the publisher if found, else nullptr.
     */
    PublisherEndpointBase* findPublisherLike(std::string& publisherPath, int qos);

    /**
     * Searches for a subscriber on this node for the given name and QoS.
     *
     * @param publisherPath     the path of the publisher.
     * @param qos               the QoS of the publisher.
     * @return                  the corresponding publisher
     */
    SubscriberEndpointBase* findSubscriberLike(std::string& publisherPath, int qos);


    /**
     * Contains pointers to the existing publisher connectors on a node.
     */
    std::map<std::string, PublisherConnector*> _publisherConnectors;

    /**
     * Counter for publishing endpoints created.
     */
    int _publisherEndpointCount = 0;

    /**
     * contains pointers to the existing subscriber connectors on a node.
     * TODO maybe we need to allow more than one subscriber connector per publisher for differen QoS?
     * Or we take the best QoS needed on device?
     */
    std::map<std::string, SubscriberConnector*> _subscriberConnectors;

    /**
     * Counter for subscribing endpoints created.
     */
    int _subscriberEndpointCount = 0;

    /**
     * A pointer to the service discovery.
     */
    StaticServiceDiscovery* _sd;

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
     * @param subscriberPath
     * @param publisherPath
     * @param qosPolicies
     * @return the negotiation request
     */
    Request* createNegotiationRequest(std::string& subscriberPath, std::string& publisherPath, QoSPolicyMap& qosPolicies);

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

    SubscriberEndpointBase* createAVBSubscriberEndpoint(std::string& publisherPath, ConnectionSpecificInformation* csi, SubscriberConnector* connector);
    SubscriberEndpointBase* createTCPSubscriberEndpoint(std::string& publisherPath, ConnectionSpecificInformation* csi, SubscriberConnector* connector);
    SubscriberEndpointBase* createUDPSubscriberEndpoint(std::string& publisherPath, ConnectionSpecificInformation* csi, SubscriberConnector* connector);
    SubscriberEndpointBase* createSOMEIPSubscriberEndpoint(std::string& publisherPath, ConnectionSpecificInformation* csi, SubscriberConnector* connector);

    PublisherEndpointBase* createAVBPublisherEndpoint(std::string& publisherPath, int qos, PublisherConnector* connector);
    PublisherEndpointBase* createTCPPublisherEndpoint(std::string& publisherPath, int qos, PublisherConnector* connector);
    PublisherEndpointBase* createUDPPublisherEndpoint(std::string& publisherPath, int qos, PublisherConnector* connector);
    PublisherEndpointBase* createSOMEIPPublisherEndpoint(std::string& publisherPath, int qos, PublisherConnector* connector);
};

} /* end namespace  */

#endif
