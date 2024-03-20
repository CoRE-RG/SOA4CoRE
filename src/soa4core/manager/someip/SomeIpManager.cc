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

#include "soa4core/discovery/SomeIpDiscoveryNotification.h"
#include "soa4core/discovery/someip/SomeIpSD.h"
#include "soa4core/manager/someip/SomeIpManager.h"
#include "soa4core/endpoints/publisher/someip/udp/SOMEIPUDPPublisherEndpoint.h"
#include "soa4core/endpoints/publisher/someip/tcp/SOMEIPTCPPublisherEndpoint.h"
#include "soa4core/endpoints/publisher/someip/udp/SOMEIPUDPMcastPublisherEndpoint.h"
#include "soa4core/endpoints/subscriber/someip/tcp/SOMEIPTCPSubscriberEndpoint.h"
#include "soa4core/endpoints/subscriber/someip/udp/SOMEIPUDPSubscriberEndpoint.h"
#include "soa4core/endpoints/subscriber/someip/udp/SOMEIPUDPMcastSubscriberEndpoint.h"
//CoRE4INET
#include <core4inet/base/CoRE4INET_Defs.h>
//INET
#include "inet/networklayer/contract/ipv4/IPv4Address.h"
#include "inet/linklayer/ethernet/Ethernet.h"
#include "inet/networklayer/ipv4/IPv4Datagram_m.h"
#include "inet/transportlayer/udp/UDPPacket_m.h"
//STD
#include <set>
#include <algorithm>
#include <math.h>

using namespace std;
using namespace inet;

namespace SOA4CoRE {

#define MSG_INITIAL_WAIT_OVER "INITIAL_WAIT_OVER"
#define MSG_REPETITION "REPETITION"
#define MSG_CYCLIC_OFFER "CYCLIC_OFFER"

Define_Module(SomeIpManager);

void SomeIpManager::initialize(int stage) {
    Manager::initialize(stage);
    if (stage == INITSTAGE_APPLICATION_LAYER) {
        if (!(_sd = dynamic_cast<IServiceDiscovery*>(getParentModule()->getSubmodule(par("sdmoduleName"))))) {
            throw cRuntimeError("No IServiceDiscovery found.");
        }
        if(SomeIpSD* sd = dynamic_cast<SomeIpSD*>(_sd)) {
            sd->subscribe("serviceFindSignal",this);
            sd->subscribe("serviceOfferSignal",this);
            sd->subscribe("subscribeEventGroupSignal",this);
            sd->subscribe("subscribeEventGroupAckSignal",this);
        } else {
            throw cRuntimeError("No SOME/IP SD found.");
        }

        _findResultSignal = omnetpp::cComponent::registerSignal("findResultSignal");
        _subscribeSignal = omnetpp::cComponent::registerSignal("subscribeSignal");
        _subscribeAckSignal = omnetpp::cComponent::registerSignal("subscribeAckSignal");
    }
}

void SomeIpManager::handleMessage(cMessage *msg) {
    if (msg->isSelfMessage() && (strcmp(msg->getName(), MSG_INITIAL_WAIT_OVER) == 0)) {
        handleInitialWaitPhaseOver((SomeIpSDState*) msg->getContextPointer());
        delete msg;
    } else if (msg->isSelfMessage() && (strcmp(msg->getName(), MSG_REPETITION) == 0)) {
        handleNextRepetitionPhase((SomeIpSDState*) msg->getContextPointer());
        delete msg;
    } else if (msg->isSelfMessage() && (strcmp(msg->getName(), MSG_CYCLIC_OFFER) == 0)) {
        handleCyclicOffer((SomeIpSDState*) msg->getContextPointer());
        delete msg;
    } else {
        Manager::handleMessage(msg);
    }
}

void SomeIpManager::handleParameterChange(const char* parname) {
    Manager::handleParameterChange(parname);

    if (!parname || !strcmp(parname, "repetitionsMax")) {
        _repetitionsMax = par("repetitionsMax").intValue();
    }
}

PublisherConnector* SomeIpManager::registerPublisherService(ServiceBase* publisherApplication) {
    PublisherConnector* publisherConnector = Manager::registerPublisherService(publisherApplication);
    Enter_Method_Silent();
    // intercept connector to register service in the offer table and start offering it
    if(publisherConnector) {
        if(Publisher* publisher = dynamic_cast<Publisher*>(publisherConnector->getApplication()))
        {
            if(publisher->isStatic()) {
                if(publisher->getQoSGroups().empty() || publisher->getQoSGroups().find(QoSGroup::SOMEIP_UDP_MCAST) == publisher->getQoSGroups().end())
                {
                    throw cRuntimeError("Static endpoint only supported for SOMEIP_UDP_MCAST ...");
                }
                SOMEIPUDPMcastPublisherEndpoint* publisherEndpoint =
                        dynamic_cast<SOMEIPUDPMcastPublisherEndpoint*>(
                                createOrFindPublisherEndpoint(publisher->getServiceId(),QoSGroup::SOMEIP_UDP_MCAST)
                                );
                if(!publisherEndpoint) {
                    throw cRuntimeError("No SOME/IP UDP Mcast Publisher was created.");
                }
                CSI_SOMEIP_UDP* csi_udp_someip = new CSI_SOMEIP_UDP();
                auto mcastAddr = IPv4Address(publisher->getMcastDestAddr().c_str());
                auto mcastPort = publisher->getMcastDestPort();
                if (!(mcastPort > 0 && mcastAddr.isMulticast()))
                {
                    throw cRuntimeError("No mcast information set for publisher");
                }
                csi_udp_someip->setAddress(publisher->getMcastDestAddr().c_str());
                csi_udp_someip->setPort(publisher->getMcastDestPort());
                publisherEndpoint->addRemote(csi_udp_someip);
                delete csi_udp_someip;
            }
            else
            {
                int serviceId = publisher->getServiceId();
                int instanceId = publisher->getInstanceId();
                //check if already exists
                if(_offers.count(serviceId))
                {
                    if(_offers[serviceId].count(instanceId))
                    {
                        throw cRuntimeError("Service already in offer map. This should never happen!");
                    }
                }
                else
                {
                    _offers[serviceId] = OfferInstanceStateMap();
                }
                // create offer state
                OfferState* offerState = new OfferState();
                offerState->serviceOffering = SomeIpDiscoveryNotification(publisher->getServiceId(),
                        IPv4Address::UNSPECIFIED_ADDRESS,
                        publisher->getInstanceId(),
                        publisher->getQoSGroups(),
                        QoSGroup::UNDEFINED,
                        publisher->getTcpPort(),
                        publisher->getUdpPort(),
                        L3Address(publisher->getMcastDestAddr().c_str()),
                        publisher->getMcastDestPort(),
                        publisher->getPayloadMax(),
                        publisher->getIntervalMin(),
                        publisher->getVlanId(),
                        publisher->getPcp(),
                        publisher->getDeadline()
                );
                _offers[serviceId][instanceId] = offerState;
                // start wait phase
                startInitialWaitPhase(offerState);
            }
        }
    }
    return publisherConnector;
}

void SomeIpManager::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj, cObject *details) {
    if (!strcmp(getSignalName(signalID),"serviceFindSignal")) {
        lookForService(obj);
    } else if(!strcmp(getSignalName(signalID),"serviceOfferSignal")) {
        subscribeServiceIfThereIsAPendingRequest(obj);
    } else if(!strcmp(getSignalName(signalID),"subscribeEventGroupSignal")){
        acknowledgeSubscription(obj);
    } else if (!strcmp(getSignalName(signalID),"subscribeEventGroupAckSignal")) {
        processAcknowledgedSubscription(obj);
    } else {
        throw cRuntimeError("Unknown signal.");
    }
}

// Subscriber-side
void SomeIpManager::processAcknowledgedSubscription(cObject* obj) {
    SomeIpDiscoveryNotification* someIpDiscoveryNotification = nullptr;
    if (!(someIpDiscoveryNotification = dynamic_cast<SomeIpDiscoveryNotification*>(obj))) {
        throw cRuntimeError("The discovery notification must be of type SomeIpDiscoveryNotification");
    }
    auto serviceId = someIpDiscoveryNotification->getServiceId();
    auto instanceId = someIpDiscoveryNotification->getInstanceId();
    set<QoSGroup> qosGroups = someIpDiscoveryNotification->getQoSGroups();
    if(_subscriptions.count(serviceId)) {
        for (auto it=qosGroups.begin(); it!=qosGroups.end();it++) {
            QoSGroup qosGroup = *it;
            if(_subscriptions[serviceId].count(qosGroup)) {
                if(_subscriptions[serviceId][qosGroup].count(instanceId)) {
                    //specific instance requested
                    SubscriptionState* subState = _subscriptions[serviceId][qosGroup][instanceId];
                    if(subState->requested) {
                        if(!subState->active) {
                            // this is a new established subscription, and no cyclic refresh
                            createSubscriberEndpoint(someIpDiscoveryNotification, qosGroup);
                            subState->active = true;
                        }
                        subState->requested = false;
                    }
                }
                if(_subscriptions[serviceId][qosGroup].count(0xFFFF)) {
                    //any instance requested
                    SubscriptionState* subState = _subscriptions[serviceId][qosGroup][0xFFFF];
                    if(subState->requested) {
                        if(!subState->active) {
                            // this is a new established subscription, and no cyclic refresh
                            createSubscriberEndpoint(someIpDiscoveryNotification, qosGroup);
                            subState->active = true;
                        }
                        subState->requested = false;
                    }
                }
            }
        }
    }
    delete obj;
}

SubscriberEndpointBase* SomeIpManager::createConnectionSpecificSubscriberEndpoint(ConnectionSpecificInformation* csi,
        SubscriberConnector* subscriberConnector) {
    SubscriberEndpointBase* sub = nullptr;
    //create according endpoint
    switch(csi->getConnectionType()){
    case ConnectionType::ct_someip_tcp:
        sub = createSomeIpTCPSubscriberEndpoint(csi, subscriberConnector);
        break;
    case ConnectionType::ct_someip_udp:
        sub = createSomeIpUDPSubscriberEndpoint(csi, subscriberConnector);
        break;
    case ConnectionType::ct_someip_udp_mcast:
        sub = createSomeIpUDPMcastSubscriberEndpoint(csi, subscriberConnector);
        break;
    default:
        throw cRuntimeError("Unknown connection type for subscriber.");
        break;
    }
    return sub;
}

PublisherEndpointBase* SomeIpManager::createQoSSpecificPublisherEndpoint(QoSGroup qosGroup, PublisherConnector* publisherConnector) {
    PublisherEndpointBase* pub = nullptr;
    //create according endpoint
    switch(qosGroup){
    case QoSGroup::SOMEIP_TCP:
        pub = createSomeIpTCPPublisherEndpoint(publisherConnector);
        break;
    case QoSGroup::SOMEIP_UDP:
        pub = createSomeIpUDPPublisherEndpoint(publisherConnector);
        break;
    case QoSGroup::SOMEIP_UDP_MCAST:
        pub = createSomeIpUDPMcastPublisherEndpoint(publisherConnector);
        break;
    default:
        throw cRuntimeError("Unknown connection type for publisher.");
        break;
    }
    return pub;
}

QoSGroup SomeIpManager::getQoSGroupForConnectionType(ConnectionType connectionType){
    switch(connectionType){
    case ConnectionType::ct_someip_tcp:
        return QoSGroup::SOMEIP_TCP;
        break;
    case ConnectionType::ct_someip_udp:
        return QoSGroup::SOMEIP_UDP;
        break;
    case ConnectionType::ct_someip_udp_mcast:
        return QoSGroup::SOMEIP_UDP_MCAST;
        break;
    default:
        throw cRuntimeError("Unknown connection type.");
        break;
    }
}

// Subscriber-side
void SomeIpManager::discoverService(ServiceIdentifier publisherServiceIdentifier, ServiceBase* subscriberApplication) {
    Subscriber* subscriberApplication_ = nullptr;
    if (!(subscriberApplication_ = dynamic_cast<Subscriber*>(subscriberApplication))) {
        throw cRuntimeError("Subscriber application must be of type Subscriber.");
    }
    auto serviceId = publisherServiceIdentifier.getServiceId();
    auto instanceId = publisherServiceIdentifier.getInstanceId();
    QoSGroup qosGroup = subscriberApplication_->getQoSGroup();
    if (subscriberApplication_->isStatic())
    {
        if (qosGroup != QoSGroup::SOMEIP_UDP_MCAST)
        {
            throw cRuntimeError("Static endpoint only supported for SOMEIP_UDP_MCAST ...");
        }
        // no discovery needed, everything should be known.
        CSI_SOMEIP_UDP_MCAST* csi = new CSI_SOMEIP_UDP_MCAST();
        auto mcastAddr = IPv4Address(subscriberApplication_->getMcastDestAddr().c_str());
        auto mcastPort = subscriberApplication_->getMcastDestPort();
        if (!(mcastPort > 0 && mcastAddr.isMulticast()))
        {
            throw cRuntimeError("No mcast information set for subscriber");
        }
        csi->setAddress(subscriberApplication_->getAddress().str().c_str());
        csi->setPort(subscriberApplication_->getUdpPort());
        csi->setDestAddress(subscriberApplication_->getMcastDestAddr().c_str());
        csi->setDestPort(subscriberApplication_->getMcastDestPort());
        SubscriberEndpointBase* sub = createOrFindSubscriberEndpoint(serviceId, csi);
        if(!sub) {
            throw cRuntimeError("No subscriber was created...");
        }
        delete csi;
        return;
    }
    if(_subscriptions.count(serviceId)) {
        if(_subscriptions[serviceId].count(qosGroup)) {
            if(_subscriptions[serviceId][qosGroup].count(instanceId)) {
                throw cRuntimeError("Sepecific instance already requested, this should never happen!");
            } else if(_subscriptions[serviceId][qosGroup].count(0xFFFF)) {
                throw cRuntimeError("Currently we cannot handle any simultaneous any subscriptions and specific subs");
            }
        } else {
            _subscriptions[serviceId][qosGroup] = SubscriptionInstanceStateMap();
        }
    } else {
        _subscriptions[serviceId] = SubscriptionQoSGroupStateMap();
        _subscriptions[serviceId][qosGroup] = SubscriptionInstanceStateMap();
    }
    SubscriptionState* subState = new SubscriptionState();
    subState->publisherIdentifier = publisherServiceIdentifier;
    // TODO OFFER CHACHING check if service already known?
    _subscriptions[serviceId][qosGroup][instanceId] = subState;
    startInitialWaitPhase(subState);
}

SubscriberEndpointBase* SomeIpManager::createSomeIpTCPSubscriberEndpoint(
        ConnectionSpecificInformation* csi,
        SubscriberConnector* subscriberConnector) {
    SubscriberEndpointBase* ret = nullptr;

    CSI_SOMEIP_TCP* csi_someip = dynamic_cast<CSI_SOMEIP_TCP*>(csi);

    if(csi_someip){
        // 1. Find the factory object;
        // 2. Create the module;
        SOMEIPTCPSubscriberEndpoint* someipTcpEndpoint =
                                dynamic_cast<SOMEIPTCPSubscriberEndpoint*>(
                                        addSubscriberEndpointModule(
                                                "soa4core.endpoints.subscriber.someip.tcp.SOMEIPTCPSubscriberEndpoint"));
        // 3. Set up its parameters and gate sizes as needed;
        string localAddr = subscriberConnector->getAddress().str();
        someipTcpEndpoint->par("localAddress").setStringValue(localAddr);
        int localPort = subscriberConnector->getTcpPort();
        someipTcpEndpoint->par("localPort").setIntValue(localPort);
        someipTcpEndpoint->par("remoteAddress").setStringValue(csi_someip->getAddress());
        someipTcpEndpoint->par("remotePort").setIntValue(csi_someip->getPort());

        // cast back.
        ret = dynamic_cast<SubscriberEndpointBase*>(someipTcpEndpoint);
        //connect endpoint to the reader
        ret->setSubscriberConnector(subscriberConnector);
        subscriberConnector->setEndpoint(ret);
    }

    return ret;
}

SubscriberEndpointBase* SomeIpManager::createSomeIpUDPSubscriberEndpoint(
        ConnectionSpecificInformation* csi,
        SubscriberConnector* subscriberConnector) {

    SubscriberEndpointBase* ret = nullptr;

    CSI_SOMEIP_UDP* csi_someip = dynamic_cast<CSI_SOMEIP_UDP*>(csi);

    if(csi_someip){
        // 1. Find the factory object;
        // 2. Create the module;
        SOMEIPUDPSubscriberEndpoint* someipUdpMcastEndpoint =
                                dynamic_cast<SOMEIPUDPSubscriberEndpoint*>(
                                        addSubscriberEndpointModule(
                                                "soa4core.endpoints.subscriber.someip.udp.SOMEIPUDPSubscriberEndpoint"));
        // 3. Set up its parameters and gate sizes as needed;
        string localAddr = subscriberConnector->getAddress().str();
        someipUdpMcastEndpoint->par("localAddress").setStringValue(localAddr);
        int localPort = subscriberConnector->getUdpPort();
        someipUdpMcastEndpoint->par("localPort").setIntValue(localPort);

        // cast back.
        ret = dynamic_cast<SubscriberEndpointBase*>(someipUdpMcastEndpoint);
        //connect endpoint to the reader
        ret->setSubscriberConnector(subscriberConnector);
        subscriberConnector->setEndpoint(ret);
    }

    return ret;
}

SubscriberEndpointBase* SomeIpManager::createSomeIpUDPMcastSubscriberEndpoint(
        ConnectionSpecificInformation* csi,
        SubscriberConnector* subscriberConnector) {

    SubscriberEndpointBase* ret = nullptr;

    CSI_SOMEIP_UDP_MCAST* csi_someip = dynamic_cast<CSI_SOMEIP_UDP_MCAST*>(csi);

    if(csi_someip){
        // 1. Find the factory object;
        // 2. Create the module;
        SOMEIPUDPMcastSubscriberEndpoint* someipUdpMcastEndpoint =
                                dynamic_cast<SOMEIPUDPMcastSubscriberEndpoint*>(
                                        addSubscriberEndpointModule(
                                                "soa4core.endpoints.subscriber.someip.udp.SOMEIPUDPMcastSubscriberEndpoint"));
        // 3. Set up its parameters and gate sizes as needed;
        string localAddr = subscriberConnector->getAddress().str();
        someipUdpMcastEndpoint->par("localAddress").setStringValue(localAddr);
        int localPort = subscriberConnector->getUdpPort();
        someipUdpMcastEndpoint->par("localPort").setIntValue(localPort);
        string mcastAddr = csi_someip->getDestAddress();
        someipUdpMcastEndpoint->par("mcastDestAddress").setStringValue(mcastAddr);
        int mcastPort = csi_someip->getDestPort();
        someipUdpMcastEndpoint->par("mcastDestPort").setIntValue(mcastPort);

        // cast back.
        ret = dynamic_cast<SubscriberEndpointBase*>(someipUdpMcastEndpoint);
        //connect endpoint to the reader
        ret->setSubscriberConnector(subscriberConnector);
        subscriberConnector->setEndpoint(ret);
    }

    return ret;
}

PublisherEndpointBase* SomeIpManager::createSomeIpTCPPublisherEndpoint(
        PublisherConnector* publisherConnector) {
    // 1. Find the factory object;
    // 2. Create the module;
    SOMEIPTCPPublisherEndpoint* someipTcpEndpoint =
                        dynamic_cast<SOMEIPTCPPublisherEndpoint*>(
                            addPublisherEndpointModule(
                                "soa4core.endpoints.publisher.someip.tcp.SOMEIPTCPPublisherEndpoint"));
    Publisher* publisherApplication = nullptr;
    if (!(publisherApplication = dynamic_cast<Publisher*>(publisherConnector->getApplication()))) {
        throw cRuntimeError("The publisher application is expected to be of the type Publisher.");
    }
    // 3. Set up its parameters and gate sizes as needed;
    string localAddr = publisherApplication->getAddress().str();
    someipTcpEndpoint->par("localAddress").setStringValue(localAddr);
    int localPort = publisherApplication->getTcpPort();
    someipTcpEndpoint->par("localPort").setIntValue(localPort);
    someipTcpEndpoint->par("pcp").setIntValue(publisherApplication->getPcp());
    someipTcpEndpoint->par("vlan_id").setIntValue(publisherApplication->getVlanId());
    someipTcpEndpoint->par("registerStream").setBoolValue(this->_registerStreams);
    someipTcpEndpoint->par("advertiseStreamRegistration").setBoolValue(this->_advertiseStreamRegistration);

    // cast back.
    PublisherEndpointBase* ret = dynamic_cast<PublisherEndpointBase*>(someipTcpEndpoint);
    //connect endpoint to the reader
    ret->setPublisherConnector(publisherConnector);
    publisherConnector->addEndpoint(ret);

    return ret;
}

PublisherEndpointBase* SomeIpManager::createSomeIpUDPPublisherEndpoint(
        PublisherConnector* publisherConnector) {
    // 1. Find the factory object;
    // 2. Create the module;
    SOMEIPUDPPublisherEndpoint* someipUdpEndpoint =
                        dynamic_cast<SOMEIPUDPPublisherEndpoint*>(addPublisherEndpointModule("soa4core.endpoints.publisher.someip.udp.SOMEIPUDPPublisherEndpoint"));
    Publisher* publisherApplication = nullptr;
    if (!(publisherApplication = dynamic_cast<Publisher*>(publisherConnector->getApplication()))) {
        throw cRuntimeError("The publisher application is expected to be of the type Publisher.");
    }
    // 3. Set up its parameters and gate sizes as needed;
    string localAddr = publisherApplication->getAddress().str();
    someipUdpEndpoint->par("localAddress").setStringValue(localAddr);
    int localPort = publisherApplication->getUdpPort();
    someipUdpEndpoint->par("localPort").setIntValue(localPort);
    someipUdpEndpoint->par("pcp").setIntValue(publisherApplication->getPcp());
    someipUdpEndpoint->par("vlan_id").setIntValue(publisherApplication->getVlanId());
    someipUdpEndpoint->par("registerStream").setBoolValue(this->_registerStreams);
    someipUdpEndpoint->par("advertiseStreamRegistration").setBoolValue(this->_advertiseStreamRegistration);

    // cast back.
    PublisherEndpointBase* ret = dynamic_cast<PublisherEndpointBase*>(someipUdpEndpoint);
    //connect endpoint to the reader
    ret->setPublisherConnector(publisherConnector);
    publisherConnector->addEndpoint(ret);

    return ret;
}

PublisherEndpointBase* SomeIpManager::createSomeIpUDPMcastPublisherEndpoint(
        PublisherConnector* publisherConnector) {

    // 1. Find the factory object;
    cModuleType * moduleType = cModuleType::get(
                "soa4core.endpoints.publisher.someip.udp.SOMEIPUDPMcastPublisherEndpoint");
    // 2. Create the module;
    SOMEIPUDPMcastPublisherEndpoint* someipUdpMcastEndpoint =
                        dynamic_cast<SOMEIPUDPMcastPublisherEndpoint*>(
                            addPublisherEndpointModule(
                                "soa4core.endpoints.publisher.someip.udp.SOMEIPUDPMcastPublisherEndpoint"));
    Publisher* publisherApplication = nullptr;
    if (!(publisherApplication = dynamic_cast<Publisher*>(publisherConnector->getApplication()))) {
        throw cRuntimeError("The publisher application is expected to be of the type Publisher.");
    }
    // 3. Set up its parameters and gate sizes as needed;
    string localAddr = publisherApplication->getAddress().str();
    someipUdpMcastEndpoint->par("localAddress").setStringValue(localAddr);
    int localPort = publisherApplication->getUdpPort();
    someipUdpMcastEndpoint->par("localPort").setIntValue(localPort);
    string mcastDestAddr = publisherApplication->getMcastDestAddr();
    someipUdpMcastEndpoint->par("mcastDestAddress").setStringValue(mcastDestAddr);
    int mcastDestPort = publisherApplication->getMcastDestPort();
    someipUdpMcastEndpoint->par("mcastDestPort").setIntValue(mcastDestPort);
    someipUdpMcastEndpoint->par("pcp").setIntValue(publisherApplication->getPcp());
    someipUdpMcastEndpoint->par("vlan_id").setIntValue(publisherApplication->getVlanId());
    someipUdpMcastEndpoint->par("registerStream").setBoolValue(this->_registerStreams);
    someipUdpMcastEndpoint->par("advertiseStreamRegistration").setBoolValue(this->_advertiseStreamRegistration);

    // cast back.
    PublisherEndpointBase* ret = dynamic_cast<PublisherEndpointBase*>(someipUdpMcastEndpoint);
    //connect endpoint to the reader
    ret->setPublisherConnector(publisherConnector);
    publisherConnector->addEndpoint(ret);

    return ret;
}

// Publisher-side
void SomeIpManager::lookForService(cObject* obj) {
    SomeIpDiscoveryNotification* request = dynamic_cast<SomeIpDiscoveryNotification*>(obj);
    // create offer state
    int serviceId = request->getServiceId();
    int instanceId = request->getInstanceId();
    if(_offers.count(serviceId)) {
        if(_offers[serviceId].count(instanceId)) {
            if(_offers[serviceId][instanceId]->phase == SomeIpSDState::SdPhase::INITIAL_WAIT_PHASE) {
                EV << "Ignoring find as offered service is still in initial wait phase" << endl;
            } else {
                SomeIpDiscoveryNotification* response =
                        new SomeIpDiscoveryNotification(_offers[serviceId][instanceId]->serviceOffering);
                // replace address to respond to find with unicast
                response->setAddress(request->getAddress());
                emit(_findResultSignal,response);
            }
        } else if(instanceId == 0xFFFF) {
            // respond with all local service instances of the service id
            for(auto offer : _offers[serviceId]) {
                if(offer.second->phase == SomeIpSDState::SdPhase::INITIAL_WAIT_PHASE) {
                    EV << "Ignoring find this offered instance as it is still in initial wait phase" << endl;
                } else {
                    SomeIpDiscoveryNotification* response =
                            new SomeIpDiscoveryNotification(offer.second->serviceOffering);
                    // replace address to respond to find with unicast
                    response->setAddress(request->getAddress());
                    emit(_findResultSignal,response);
                }
            }
        }
    }
    delete(request);
}

// Subscriber-side
void SomeIpManager::subscribeServiceIfThereIsAPendingRequest(cObject* obj) {
    SomeIpDiscoveryNotification* someIpdiscoveryNotificationOffer = nullptr;
    if (!(someIpdiscoveryNotificationOffer = dynamic_cast<SomeIpDiscoveryNotification*>(obj))) {
        throw cRuntimeError("Notification must be of type SomeIpDiscoveryNotification.");
    }
    auto serviceId = someIpdiscoveryNotificationOffer->getServiceId();
    auto instanceId = someIpdiscoveryNotificationOffer->getInstanceId();
    set<QoSGroup> qosGroups = someIpdiscoveryNotificationOffer->getQoSGroups();
    if(_subscriptions.count(serviceId)) {
        for (QoSGroup qosGroup : qosGroups) {
            if(_subscriptions[serviceId].count(qosGroup)) {
                SubscriptionState* subscription = nullptr;
                if(_subscriptions[serviceId][qosGroup].count(instanceId) > 0) {
                    subscription = _subscriptions[serviceId][qosGroup][instanceId];
                } else if (_subscriptions[serviceId][qosGroup].count(0xFFFF) > 0) {
                    subscription = _subscriptions[serviceId][qosGroup][0xFFFF];
                }
                if(subscription != nullptr && !subscription->requested) {
                    list<SubscriberConnector*> subscriberConnectors = _lsr->getSubscriberConnectors(someIpdiscoveryNotificationOffer->getServiceId());
                    for (SubscriberConnector* subscriberConnector: subscriberConnectors) {
                        vector<ServiceBase*> subscriberApplications = subscriberConnector->getApplications();
                        if (subscriberApplications.empty()) {
                            throw cRuntimeError("Empty subscriber connector without applications!.");
                        }
                        Subscriber* subscriberApplication = nullptr;
                        if (!(subscriberApplication = dynamic_cast<Subscriber*>(subscriberApplications[0]))) {
                            throw cRuntimeError("The subscriber application must be of the type Subscriber.");
                        }
                        if (subscriberApplication->getQoSGroup() == qosGroup) {
                            SomeIpDiscoveryNotification* someIpdiscoveryNotificationSubscription = new SomeIpDiscoveryNotification(serviceId,
                                    someIpdiscoveryNotificationOffer->getAddress(), instanceId, set<QoSGroup>{}, qosGroup, subscriberConnector->getTcpPort(),
                                    subscriberConnector->getUdpPort(), someIpdiscoveryNotificationOffer->getMcastDestAddr(), someIpdiscoveryNotificationOffer->getMcastDestPort()
                            );
                            someIpdiscoveryNotificationSubscription->setDeadline(subscriberApplication->getDeadline());
                            subscription->serviceOfferReceivedAndRequested();
                            emit(_subscribeSignal, someIpdiscoveryNotificationSubscription);
                            // Break here because there can be only one connector for a service ID and QoS Group pair
                            break;
                        }
                    }
                }
            }
        }
    }
    delete obj;
}

// Publisher-side
void SomeIpManager::acknowledgeSubscription(cObject* obj) {
    SomeIpDiscoveryNotification* someIpDiscoveryNotification = nullptr;
    if (!(someIpDiscoveryNotification = dynamic_cast<SomeIpDiscoveryNotification*>(obj))) {
        throw cRuntimeError("The discovery notification must be of type SomeIpDiscoveryNotification");
    }

    if (PublisherConnector* publisherConnector = _lsr->getPublisherConnector(someIpDiscoveryNotification->getServiceId())) {
        Publisher* publisher = nullptr;
        if (!(publisher = dynamic_cast<Publisher*>(publisherConnector->getApplication()))) {
            throw cRuntimeError("The application must not null and must be of type Publisher");
        }

        if (publisher->getQoSGroups().find( someIpDiscoveryNotification->getQoSGroup()) != publisher->getQoSGroups().end()) {
            // find or create local endpoint if needed
            switch (someIpDiscoveryNotification->getQoSGroup()) {
                case QoSGroup::SOMEIP_TCP: {
                    PublisherEndpointBase* publisherEndpointBase = createOrFindPublisherEndpoint(someIpDiscoveryNotification->getServiceId(), QoSGroup::SOMEIP_TCP);
                    if(!publisherEndpointBase) {
                        throw cRuntimeError("No SOME/IP TCP Publisher was created.");
                    }
                    SOMEIPTCPPublisherEndpoint* someipTcpPublisherEndpoint = dynamic_cast<SOMEIPTCPPublisherEndpoint*>(publisherEndpointBase);
                    if(!(someipTcpPublisherEndpoint)) {
                        throw cRuntimeError("Endpoint is no SOMEIPTCPPublisherEndpoint.");
                    }
                    break;
                }
                case QoSGroup::SOMEIP_UDP: {
                    PublisherEndpointBase* publisherEndpointBase = createOrFindPublisherEndpoint(someIpDiscoveryNotification->getServiceId(),QoSGroup::SOMEIP_UDP);
                    if(!publisherEndpointBase) {
                        throw cRuntimeError("No SOME/IP UDP Publisher was created.");
                    }
                    SOMEIPUDPPublisherEndpoint* someipUdpPublisherEndpoint = dynamic_cast<SOMEIPUDPPublisherEndpoint*>(publisherEndpointBase);
                    if(!(someipUdpPublisherEndpoint)) {
                        throw cRuntimeError("Endpoint is no SOMEIPUDPPublisherEndpoint.");
                    }
                    CSI_SOMEIP_UDP* csi_udp_someip = new CSI_SOMEIP_UDP();
                    csi_udp_someip->setAddress(someIpDiscoveryNotification->getAddress().str().c_str());
                    csi_udp_someip->setPort(someIpDiscoveryNotification->getUdpPort());
                    someipUdpPublisherEndpoint->addRemote(csi_udp_someip);
                    delete csi_udp_someip;
                    break;
                }
                case QoSGroup::SOMEIP_UDP_MCAST: {
                    PublisherEndpointBase* publisherEndpointBase = createOrFindPublisherEndpoint(someIpDiscoveryNotification->getServiceId(),QoSGroup::SOMEIP_UDP_MCAST);
                    if(!publisherEndpointBase) {
                        throw cRuntimeError("No SOME/IP UDP Mcast Publisher was created.");
                    }
                    SOMEIPUDPMcastPublisherEndpoint* someipUdpMcastPublisherEndpoint = dynamic_cast<SOMEIPUDPMcastPublisherEndpoint*>(publisherEndpointBase);
                    if(!(someipUdpMcastPublisherEndpoint)) {
                        throw cRuntimeError("Endpoint is no SOMEIPUDPMcastPublisherEndpoint.");
                    }
                    CSI_SOMEIP_UDP* csi_udp_someip = new CSI_SOMEIP_UDP();
                    csi_udp_someip->setAddress(someIpDiscoveryNotification->getAddress().str().c_str());
                    csi_udp_someip->setPort(someIpDiscoveryNotification->getUdpPort());
                    someipUdpMcastPublisherEndpoint->addRemote(csi_udp_someip);
                    delete csi_udp_someip;
                    break;
                }
                default:
                    throw cRuntimeError("Unknown QoS group");
            }
            // acknowledge subscription
            if(_offers.count(publisher->getServiceId()) && _offers[publisher->getServiceId()].count(publisher->getInstanceId())) {
                _offers[publisher->getServiceId()][publisher->getInstanceId()]->hasSubscription = true;
            }
            SomeIpDiscoveryNotification* someIpDiscoveryNotificationAcknowledge = new SomeIpDiscoveryNotification(someIpDiscoveryNotification->getServiceId(), someIpDiscoveryNotification->getAddress(),
                    publisher->getInstanceId(), set<QoSGroup>{}, someIpDiscoveryNotification->getQoSGroup(), publisher->getTcpPort(), publisher->getUdpPort(),
                    someIpDiscoveryNotification->getMcastDestAddr(), someIpDiscoveryNotification->getMcastDestPort());
            emit(_subscribeAckSignal,someIpDiscoveryNotificationAcknowledge);
        }
    }
    delete obj;
}

// Subscriber-side
void SomeIpManager::createSubscriberEndpoint(SomeIpDiscoveryNotification* someIpDiscoveryNotification, QoSGroup qosGroup) {
    ConnectionSpecificInformation* csi;
    switch (qosGroup) {
        case QoSGroup::SOMEIP_TCP: {
            CSI_SOMEIP_TCP* csi_someip_tcp = new CSI_SOMEIP_TCP();
            csi_someip_tcp->setAddress(someIpDiscoveryNotification->getAddress().str().c_str());
            csi_someip_tcp->setPort(someIpDiscoveryNotification->getTcpPort());
            csi = csi_someip_tcp;
            break;
        }
        case QoSGroup::SOMEIP_UDP: {
            CSI_SOMEIP_UDP* csi_someip_udp = new CSI_SOMEIP_UDP();
            csi_someip_udp->setAddress(someIpDiscoveryNotification->getAddress().str().c_str());
            csi_someip_udp->setPort(someIpDiscoveryNotification->getUdpPort());
            csi = csi_someip_udp;
            break;
        }
        case QoSGroup::SOMEIP_UDP_MCAST: {
            CSI_SOMEIP_UDP_MCAST* csi_someip_udp_mcast = new CSI_SOMEIP_UDP_MCAST();
            csi_someip_udp_mcast->setAddress(someIpDiscoveryNotification->getAddress().str().c_str());
            csi_someip_udp_mcast->setPort(someIpDiscoveryNotification->getUdpPort());
            csi_someip_udp_mcast->setDestAddress(someIpDiscoveryNotification->getMcastDestAddr().str().c_str());
            csi_someip_udp_mcast->setDestPort(someIpDiscoveryNotification->getMcastDestPort());
            csi = csi_someip_udp_mcast;
            break;
        }
        // TODO
        default:
            throw cRuntimeError("Unknown QoS group.");
    }

    SubscriberEndpointBase* sub = createOrFindSubscriberEndpoint(someIpDiscoveryNotification->getServiceId(), csi);
    if(!sub) {
        throw cRuntimeError("No subscriber was created...");
    }
    delete csi;
}

IPProtocolId SomeIpManager::getIPProtocolId(QoSGroup qosGroup) {
    IPProtocolId ipProtocolId = IPProtocolId::IP_PROT_ICMP;
    switch (qosGroup) {
        case QoSGroup::SOMEIP_TCP:
            ipProtocolId = IPProtocolId::IP_PROT_TCP;
            break;
        case QoSGroup::SOMEIP_UDP:
        case QoSGroup::SOMEIP_UDP_MCAST:
            ipProtocolId = IPProtocolId::IP_PROT_UDP;
            break;
        default:
            throw cRuntimeError("Unknown QoS group.");
    }
    return ipProtocolId;
}

void SomeIpManager::startInitialWaitPhase(SomeIpSDState* serviceState) {
    serviceState->initialDelay = this->par("initialDelay").doubleValue();
    serviceState->phase = SomeIpSDState::SdPhase::INITIAL_WAIT_PHASE;
    if(serviceState->initialDelay == 0) {
        handleInitialWaitPhaseOver(serviceState);
    } else {
        cMessage* message = new cMessage(MSG_INITIAL_WAIT_OVER);
        message->setContextPointer(serviceState);
        scheduleAt(simTime() + serviceState->initialDelay, message);
    }
}

void SomeIpManager::handleInitialWaitPhaseOver(SomeIpSDState* serviceState) {
    if(serviceState->isSubscriptionAndServiceSeen()) {
        // subscription and service was seen --> we are already in another phase
        return;
    }
    executeSdForServiceState(serviceState);
    // progress to repetition phase
    handleNextRepetitionPhase(serviceState);
}

void SomeIpManager::handleNextRepetitionPhase(SomeIpSDState* serviceState) {
    if(serviceState->isSubscriptionAndServiceSeen()) {
        // subscription and service was seen --> we are already in another phase
        return;
    }
    if(serviceState->numRepetitions > 0) {
        // not the first operation, so send again
        executeSdForServiceState(serviceState);
    }
    if(_repetitionsMax > serviceState->numRepetitions) {
        // we have more repetitions to do
        serviceState->phase = SomeIpSDState::SdPhase::REPETITION_PHASE;
        cMessage* message = new cMessage(MSG_REPETITION);
        message->setContextPointer(serviceState);
        // Wait 2^(repetitionsMax-1) * repititionBaseDelay
        double delay = par("repititionBaseDelay").doubleValue() * pow(2, serviceState->numRepetitions);
        scheduleAt(simTime() + delay, message);
        serviceState->numRepetitions++;
    } else {
        // no (more) repetitions needed goto main phase
        startMainPhase(serviceState);
    }
}

void SomeIpManager::startMainPhase(SomeIpSDState* serviceState) {
    serviceState->phase = SomeIpSDState::SdPhase::MAIN_PHASE;
    if(dynamic_cast<OfferState*>(serviceState)) {
        double cyclicOfferDelay = par("cyclicOfferDelay").doubleValue();
        if(cyclicOfferDelay>0) {
            cMessage* message = new cMessage(MSG_CYCLIC_OFFER);
            message->setContextPointer(serviceState);
            scheduleAt(simTime() + cyclicOfferDelay, message);
        }
    }
}

void SomeIpManager::handleCyclicOffer(SomeIpSDState* serviceState) {
    if(dynamic_cast<OfferState*>(serviceState)) {
        executeSdForServiceState(serviceState);
        cMessage* message = new cMessage(MSG_CYCLIC_OFFER);
        message->setContextPointer(serviceState);
        scheduleAt(simTime() + par("cyclicOfferDelay").doubleValue(), message);
    } else {
        throw cRuntimeError("Can not handly cyclic offer event for serviceState not of type OfferState ");
    }
}

void SomeIpManager::executeSdForServiceState(SomeIpSDState* serviceState) {
    if(SubscriptionState* subState = dynamic_cast<SubscriptionState*>(serviceState)) {
        // send first find
        _sd->discover(subState->publisherIdentifier);
    }
    else if(OfferState* offerState = dynamic_cast<OfferState*>(serviceState)) {
        // send first offer
        emit(_findResultSignal,new SomeIpDiscoveryNotification(offerState->serviceOffering));
    }
}

} /* end namespace SOA4CoRE */
