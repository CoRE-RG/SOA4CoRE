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
//STD
#include <algorithm>
#include <math.h>

using namespace std;

namespace SOA4CoRE {

#define MSG_INITIAL_WAIT_OVER "INITIAL_WAIT_OVER"
#define MSG_REPETITION "REPETITION"

Define_Module(SomeIpManager);

void SomeIpManager::initialize(int stage) {
    Manager::initialize(stage);
    if (stage == inet::INITSTAGE_APPLICATION_LAYER) {
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
        if(ServiceState* serviceState = dynamic_cast<ServiceState*>(msg->getContextPointer())){
            handleInitialWaitPhaseOver(serviceState);
        } else {
            throw cRuntimeError("Received INITIAL_WAIT_OVER without ServiceState attached");
        }
    } else if (msg->isSelfMessage() && (strcmp(msg->getName(), MSG_REPETITION) == 0)) {
        if(ServiceState* serviceState = dynamic_cast<ServiceState*>(msg->getContextPointer())){
            handleNextRepetitionPhase(serviceState);
        } else {
            throw cRuntimeError("Received INITIAL_WAIT_OVER without ServiceState attached");
        }
    } else {
        Manager::handleMessage(msg);
    }
}

void SomeIpManager::handleParameterChange(const char* parname) {
    Manager::handleParameterChange(parname);

    if (!parname || !strcmp(parname, "initialDelayMin")) {
        _initialDelayMin = par("initialDelayMin").doubleValue();
    }
    if (!parname || !strcmp(parname, "initialDelayMax")) {
        _initialDelayMax = par("initialDelayMax").doubleValue();
    }
    if (!parname || !strcmp(parname, "repetitionsMax")) {
        _repetitionsMax = par("repetitionsMax").intValue();
    }
    if (!parname || !strcmp(parname, "repititionBaseDelay")) {
        _repititionBaseDelay = par("repititionBaseDelay").doubleValue();
    }
    if (!parname || !strcmp(parname, "cyclicOfferDelay")) {
        _cyclicOfferDelay = par("cyclicOfferDelay").doubleValue();
    }
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
    set<QoSGroup> qosGroups = someIpDiscoveryNotification->getQoSGroups();
    for (QoSGroup qosGroup : qosGroups) {
        if (_pendingSubscriptionsMap.count(someIpDiscoveryNotification->getServiceId())
            && _pendingSubscriptionsMap[someIpDiscoveryNotification->getServiceId()].count(qosGroup)
            && _pendingSubscriptionsMap[someIpDiscoveryNotification->getServiceId()][qosGroup] == SubscriptionState_E::WAITING_FOR_SUBACK) {
            createSubscriberEndpoint(someIpDiscoveryNotification, qosGroup);
            _pendingSubscriptionsMap[someIpDiscoveryNotification->getServiceId()].erase(qosGroup);
            if (_pendingSubscriptionsMap[someIpDiscoveryNotification->getServiceId()].empty()) {
                _pendingSubscriptionsMap.erase(someIpDiscoveryNotification->getServiceId());
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
    _pendingSubscriptionsMap[publisherServiceIdentifier.getServiceId()][subscriberApplication_->getQoSGroup()] = SubscriptionState_E::WAITING_FOR_OFFER;
    _sd->discover(publisherServiceIdentifier);
}

SubscriberEndpointBase* SomeIpManager::createSomeIpTCPSubscriberEndpoint(
        ConnectionSpecificInformation* csi,
        SubscriberConnector* subscriberConnector) {
    SubscriberEndpointBase* ret = nullptr;

    CSI_SOMEIP_TCP* csi_someip = dynamic_cast<CSI_SOMEIP_TCP*>(csi);

    if(csi_someip){
        // 1. Find the factory object;
        cModuleType * moduleType = cModuleType::get(
                    "soa4core.endpoints.subscriber.someip.tcp.SOMEIPTCPSubscriberEndpoint");
        // 2. Create the module;
        SOMEIPTCPSubscriberEndpoint* someipTcpEndpoint =
                            dynamic_cast<SOMEIPTCPSubscriberEndpoint*>(
                                    moduleType->create("subscriberEndpoints", this->getParentModule(), _subscriberEndpointCount + 1, _subscriberEndpointCount));
        _subscriberEndpointCount++;
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
        cModuleType * moduleType = cModuleType::get(
                    "soa4core.endpoints.subscriber.someip.udp.SOMEIPUDPSubscriberEndpoint");
        // 2. Create the module;
        SOMEIPUDPSubscriberEndpoint* someipUdpMcastEndpoint =
                            dynamic_cast<SOMEIPUDPSubscriberEndpoint*>(
                                    moduleType->create("subscriberEndpoints", this->getParentModule(), _subscriberEndpointCount + 1, _subscriberEndpointCount));
        _subscriberEndpointCount++;
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
        cModuleType * moduleType = cModuleType::get(
                    "soa4core.endpoints.subscriber.someip.udp.SOMEIPUDPMcastSubscriberEndpoint");
        // 2. Create the module;
        SOMEIPUDPMcastSubscriberEndpoint* someipUdpMcastEndpoint =
                            dynamic_cast<SOMEIPUDPMcastSubscriberEndpoint*>(
                                    moduleType->create("subscriberEndpoints", this->getParentModule(), _subscriberEndpointCount + 1, _subscriberEndpointCount));
        _subscriberEndpointCount++;
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
    cModuleType * moduleType = cModuleType::get(
                "soa4core.endpoints.publisher.someip.tcp.SOMEIPTCPPublisherEndpoint");
    // 2. Create the module;
    SOMEIPTCPPublisherEndpoint* someipTcpEndpoint =
                        dynamic_cast<SOMEIPTCPPublisherEndpoint*>(
                                moduleType->create("publisherEndpoints", this->getParentModule(), _publisherEndpointCount + 1, _publisherEndpointCount));
    _publisherEndpointCount++;
    Publisher* publisherApplication = nullptr;
    if (!(publisherApplication = dynamic_cast<Publisher*>(publisherConnector->getApplication()))) {
        throw cRuntimeError("The publisher application is expected to be of the type Publisher.");
    }
    // 3. Set up its parameters and gate sizes as needed;
    string localAddr = publisherApplication->getAddress().str();
    someipTcpEndpoint->par("localAddress").setStringValue(localAddr);
    int localPort = publisherApplication->getTcpPort();
    someipTcpEndpoint->par("localPort").setIntValue(localPort);

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
    cModuleType * moduleType = cModuleType::get(
                "soa4core.endpoints.publisher.someip.udp.SOMEIPUDPPublisherEndpoint");
    // 2. Create the module;
    SOMEIPUDPPublisherEndpoint* someipUdpEndpoint =
                        dynamic_cast<SOMEIPUDPPublisherEndpoint*>(
                                moduleType->create("publisherEndpoints", this->getParentModule(), _publisherEndpointCount + 1, _publisherEndpointCount));
    _publisherEndpointCount++;
    Publisher* publisherApplication = nullptr;
    if (!(publisherApplication = dynamic_cast<Publisher*>(publisherConnector->getApplication()))) {
        throw cRuntimeError("The publisher application is expected to be of the type Publisher.");
    }
    // 3. Set up its parameters and gate sizes as needed;
    string localAddr = publisherApplication->getAddress().str();
    someipUdpEndpoint->par("localAddress").setStringValue(localAddr);
    int localPort = publisherApplication->getUdpPort();
    someipUdpEndpoint->par("localPort").setIntValue(localPort);

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
                                moduleType->create("publisherEndpoints", this->getParentModule(), _publisherEndpointCount + 1, _publisherEndpointCount));
    _publisherEndpointCount++;
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

    // cast back.
    PublisherEndpointBase* ret = dynamic_cast<PublisherEndpointBase*>(someipUdpMcastEndpoint);
    //connect endpoint to the reader
    ret->setPublisherConnector(publisherConnector);
    publisherConnector->addEndpoint(ret);

    return ret;
}

// Publisher-side
void SomeIpManager::lookForService(cObject* obj) {
    SomeIpDiscoveryNotification* someIpDiscoveryNotificationFindRequest = dynamic_cast<SomeIpDiscoveryNotification*>(obj);
    if (PublisherConnector* publisherConnector = _lsr->getPublisherConnector(someIpDiscoveryNotificationFindRequest->getServiceId())) {
        if(Publisher* publisher = dynamic_cast<Publisher*>(publisherConnector->getApplication())) {
            SomeIpDiscoveryNotification* someIpDiscoveryNotificationFindResponse = new SomeIpDiscoveryNotification(publisher->getServiceId(),
                    someIpDiscoveryNotificationFindRequest->getAddress(), publisher->getInstanceId(), publisher->getQoSGroups(),
                    QoSGroup::UNDEFINED, publisher->getTcpPort(), publisher->getUdpPort(), L3Address(publisher->getMcastDestAddr().c_str()), publisher->getMcastDestPort()
            );
            emit(_findResultSignal,someIpDiscoveryNotificationFindResponse);
        }
    }
    delete(someIpDiscoveryNotificationFindRequest);
}

// Subscriber-side
void SomeIpManager::subscribeServiceIfThereIsAPendingRequest(cObject* obj) {
    SomeIpDiscoveryNotification* someIpdiscoveryNotificationOffer = nullptr;
    if (!(someIpdiscoveryNotificationOffer = dynamic_cast<SomeIpDiscoveryNotification*>(obj))) {
        throw cRuntimeError("Notification must be of type SomeIpDiscoveryNotification.");
    }
    if (_pendingSubscriptionsMap.count(someIpdiscoveryNotificationOffer->getServiceId())) {
        set<QoSGroup> offeredQoSGroups = someIpdiscoveryNotificationOffer->getQoSGroups();
        list<SubscriberConnector*> subscriberConnectors = _lsr->getSubscriberConnectors(someIpdiscoveryNotificationOffer->getServiceId());

        for (QoSGroup offeredQoSGroup : offeredQoSGroups) {
            if (_pendingSubscriptionsMap.count(someIpdiscoveryNotificationOffer->getServiceId())
                && _pendingSubscriptionsMap[someIpdiscoveryNotificationOffer->getServiceId()].count(offeredQoSGroup)
                && _pendingSubscriptionsMap[someIpdiscoveryNotificationOffer->getServiceId()][offeredQoSGroup] == SubscriptionState_E::WAITING_FOR_OFFER) {
                _pendingSubscriptionsMap[someIpdiscoveryNotificationOffer->getServiceId()][offeredQoSGroup] = SubscriptionState_E::WAITING_FOR_SUBACK;
                // Find the appropriate connector to the QoSGroup
                for (SubscriberConnector* subscriberConnector: subscriberConnectors) {
                    vector<ServiceBase*> subscriberApplications = subscriberConnector->getApplications();
                    if (subscriberApplications.empty()) {
                        throw cRuntimeError("Empty subscriber connector without applications!.");
                    }
                    Subscriber* subscriberApplication = nullptr;
                    if (!(subscriberApplication = dynamic_cast<Subscriber*>(subscriberApplications[0]))) {
                        throw cRuntimeError("The subscriber application must be of the type Subscriber.");
                    }
                    if (subscriberApplication->getQoSGroup() == offeredQoSGroup) {
                        SomeIpDiscoveryNotification* someIpdiscoveryNotificationSubscription = new SomeIpDiscoveryNotification(someIpdiscoveryNotificationOffer->getServiceId(),
                                someIpdiscoveryNotificationOffer->getAddress(), someIpdiscoveryNotificationOffer->getInstanceId(), set<QoSGroup>{}, offeredQoSGroup, subscriberConnector->getTcpPort(),
                                subscriberConnector->getUdpPort(), someIpdiscoveryNotificationOffer->getMcastDestAddr(), someIpdiscoveryNotificationOffer->getMcastDestPort()
                        );
                        emit(_subscribeSignal, someIpdiscoveryNotificationSubscription);
                        // Break here because there can be only one connector for a service ID and QoS Group pair
                        break;
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
        if (find(publisher->getQoSGroups().begin(), publisher->getQoSGroups().end(), someIpDiscoveryNotification->getQoSGroup()) != publisher->getQoSGroups().end()) {
            SomeIpDiscoveryNotification* someIpDiscoveryNotificationAcknowledge = new SomeIpDiscoveryNotification(someIpDiscoveryNotification->getServiceId(), someIpDiscoveryNotification->getAddress(),
                    publisher->getInstanceId(), set<QoSGroup>{}, someIpDiscoveryNotification->getQoSGroup(), publisher->getTcpPort(), publisher->getUdpPort(),
                    someIpDiscoveryNotification->getMcastDestAddr(), someIpDiscoveryNotification->getMcastDestPort());
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

void SomeIpManager::startInitialWaitPhase(ServiceState* serviceState) {
    double diff = _initialDelayMax - _initialDelayMin;
    if(diff < 0) {
        throw cRuntimeError("Initial delay invalid as min is larger than max");
    }
    serviceState->randInitialDelay = this->dblrand()*diff + _initialDelayMin;
    serviceState->phase = ServiceState::SdPhase::INITIAL_WAIT_PHASE;
    if(serviceState->randInitialDelay == 0) {
        handleInitialWaitPhaseOver(serviceState);
    } else {
        cMessage message = new cMessage(MSG_INITIAL_WAIT_OVER);
        message->setContextPointer(serviceState);
        scheduleAt(simTime() + serviceState->randInitialDelay, msg);
    }
}

void SomeIpManager::handleInitialWaitPhaseOver(ServiceState* serviceState) {
    if(serviceState->phase != ServiceState::SdPhase::INITIAL_WAIT_PHASE) {
        // Not in initial wait phase
        // For example, when receiving an offer before the wait phase is over
        // return without progression
        return;
    }
    executeSdForServiceState(serviceState);
    // progress to repetition phase
    handleNextRepetitionPhase(serviceState);
}

void SomeIpManager::handleNextRepetitionPhase(ServiceState* serviceState) {
    if(serviceState->numRepetitions != 0 && serviceState->phase != ServiceState::SdPhase::REPETITION_PHASE) {
        // We already had a repetition but are not in repetition phase
        // For example, when receiving an offer before the repetition phase is over
        // return without progression
        return;
    }
    if(serviceState->numRepetitions > 0) {
        // not the first operation, so send again
        executeSdForServiceState(serviceState);
    }
    if(_repetitionsMax > serviceState->numRepetitions) {
        // we have more repetitions to do
        serviceState->phase = ServiceState::SdPhase::REPETITION_PHASE;
        cMessage message = new cMessage(MSG_REPETITION);
        message->setContextPointer(serviceState);
        // Wait 2^(repetitionsMax-1) * repititionBaseDelay
        scheduleAt(simTime() + _repititionBaseDelay * pow(2, serviceState->numRepetitions-1), msg);
        serviceState->numRepetitions++;
    } else {
        // no (more) repetitions needed goto main phase
    }
}

void SomeIpManager::executeSdForServiceState(ServiceState* serviceState) {
    if(SubscriptionState* subState = dynamic_cast<SubscriptionState*>(serviceState)) {
        // send first find
        _sd->discover(subState->publisherServiceIdentifier);
    }
    else if(OfferState* offerState = dynamic_cast<OfferState*>(serviceState)) {
        // send first offer
        emit(_findResultSignal,new SomeIpDiscoveryNotification(offerState->serviceOffering));
    }
}

} /* end namespace SOA4CoRE */
