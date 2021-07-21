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

#include "soqosmw/base/EndpointDescription.h"
#include "soqosmw/servicemanager/LocalServiceManager.h"
#include "soqosmw/endpoints/subscriber/realtime/avb/AVBSubscriberEndpoint.h"
#include "soqosmw/endpoints/subscriber/standard/tcp/TCPSubscriberEndpoint.h"
#include "soqosmw/endpoints/subscriber/standard/udp/UDPSubscriberEndpoint.h"
#include "soqosmw/endpoints/subscriber/someip/tcp/SOMEIPTCPSubscriberEndpoint.h"
#include "soqosmw/endpoints/subscriber/someip/udp/SOMEIPUDPSubscriberEndpoint.h"
#include "soqosmw/endpoints/publisher/realtime/avb/AVBPublisherEndpoint.h"
#include "soqosmw/endpoints/publisher/standard/tcp/TCPPublisherEndpoint.h"
#include "soqosmw/endpoints/publisher/standard/udp/UDPPublisherEndpoint.h"
#include "soqosmw/endpoints/publisher/someip/tcp/SOMEIPTCPPublisherEndpoint.h"
#include "soqosmw/endpoints/publisher/someip/udp/SOMEIPUDPPublisherEndpoint.h"
//INET
#include "inet/networklayer/common/L3AddressResolver.h"
//STD
#include <algorithm>
#include <cstring>
#include <iterator>
#include <stdio.h>


using namespace std;
using namespace inet;
using namespace CoRE4INET;

namespace SOQoSMW {

Define_Module(LocalServiceManager);

LocalServiceManager::LocalServiceManager() {
    _requestID = 0;
}

LocalServiceManager::~LocalServiceManager() {
}

void LocalServiceManager::initialize(int stage) {

    if (stage == INITSTAGE_APPLICATION_LAYER) {
        handleParameterChange(nullptr);
        //TODO Make discoveries interchangeable
        _sd = dynamic_cast<IServiceDiscovery*>(getParentModule()->getSubmodule(
                   par("sdmoduleName")));
        dynamic_cast<StaticServiceDiscovery*>(_sd)->subscribe("serviceFoundSignal",this);
        _qosnp =
               dynamic_cast<QoSNegotiationProtocol*>(getParentModule()->getSubmodule(
                       par("qosnpmoduleName")));
        _lsr =
               dynamic_cast<IServiceRegistry*>(getParentModule()->getSubmodule(
                       par("lsrmoduleName")));
       WATCH_MAP(_publisherConnectors);
       WATCH_MAP(_subscriberConnectors);
    }
}

void LocalServiceManager::handleMessage(cMessage *msg) {
    delete msg;
}

void LocalServiceManager::handleParameterChange(const char* parname) {

    //read UDP Common Parameters
    if (!parname || !strcmp(parname, "localAddress")) {
        const char* localAddr = par("localAddress");
        _localAddress = L3AddressResolver().resolve(localAddr);
    }

}

void LocalServiceManager::registerPublisherService(uint32_t publisherServiceId,
        QoSPolicyMap& qosPolicies,
        SOQoSMWApplicationBase* executingApplication) {
    Enter_Method("LSM:registerPublisherService()");

    PublisherConnector* publisherConnector = getPublisherConnector(publisherServiceId);
    if (publisherConnector != nullptr) {
        addPublisherServiceToConnector(publisherConnector, qosPolicies, executingApplication);
    } else {
        publisherConnector = createPublisherConnector(qosPolicies, executingApplication);
        //save the writer so that new endpoints can be connected to the application.
        _publisherConnectors[publisherServiceId] = publisherConnector;
    }
    _lsr->addPublisherService(new QoSService(
            publisherServiceId,
            inet::L3Address(dynamic_cast<LocalAddressQoSPolicy*>(qosPolicies[QoSPolicyNames::LocalAddress])->getValue().c_str()),
            dynamic_cast<LocalPortQoSPolicy*>(qosPolicies[QoSPolicyNames::LocalPort])->getValue(),
            qosPolicies));
}

void LocalServiceManager::addPublisherServiceToConnector(PublisherConnector* publisherConnector, QoSPolicyMap& qosPolicies, SOQoSMWApplicationBase* executingApplication) {
    if(publisherConnector->addApplication(executingApplication)){
        publisherConnector->setQos(qosPolicies);
    } else {
        throw cRuntimeError("This Publisher service already exists on this host...");
    }
}

PublisherConnector* LocalServiceManager::createPublisherConnector(QoSPolicyMap& qosPolicies, SOQoSMWApplicationBase* executingApplication) {
    // create a connector for the publisher
    // 1. Find the factory object;
    cModuleType *moduleType = cModuleType::get("soqosmw.connector.pubsub.writer.PublisherConnector");
    // 2. Create the module;
    int vectorsize = _publisherConnectors.size();
    PublisherConnector *module = dynamic_cast<PublisherConnector*> (moduleType->create("publisherConnectors", this->getParentModule(), vectorsize + 1, vectorsize));
    // 3. Set up its parameters and gate sizes as needed;
    module->addApplication(executingApplication);
    module->setQos(qosPolicies);
    module->finalizeParameters();
    // 4. Tell the (possibly compound) module to recursively create its internal submodules and connections;
    module->buildInside();
    module->callInitialize();
    // 5. Schedule activation message(s) for the new simple module(s).
    module->scheduleStart(simTime());
    return module;
}

PublisherConnector* LocalServiceManager::getPublisherConnector(uint32_t publisherServiceId) {
    //TODO Maybe getPublisherConnectorForName should be made public ...
    return getPublisherConnectorForServiceId(publisherServiceId);
}


void LocalServiceManager::registerSubscriberService(
            uint32_t publisherServiceId,
            QoSPolicyMap& qosPolicies,
            SOQoSMWApplicationBase* executingApplication)
{
    Enter_Method("LSM:registerSubscriberService()");

    SubscriberConnector* subscriberConnector = getSubscriberConnectorForServiceId(publisherServiceId);
    if (subscriberConnector != nullptr && equalQoSMap(qosPolicies, subscriberConnector->getQos())) {
        addSubscriberServiceToConnector(subscriberConnector, qosPolicies, executingApplication);
    } else {
        subscriberConnector = createSubscriberConnector(qosPolicies, executingApplication);
        // save the reader so that new endpoints can be connected to the application.
        _subscriberConnectors[publisherServiceId] = subscriberConnector;
    }
}

void LocalServiceManager::subscribeQoSService(IServiceIdentifier& publisherServiceIdentifier, QoSPolicyMap& qosPolicyMap) {
    //check if publisher exists in the network and start the negotiation with a request.
    if (IService* service = _lsr->getService(dynamic_cast<ServiceIdentifier&>(publisherServiceIdentifier))) {
        Request* request = createNegotiationRequest(service, qosPolicyMap);
        //create qos broker for the request
        _qosnp->createQoSBroker(request);
    } else {
        QoSService qosService = QoSService(
                publisherServiceIdentifier.getServiceId(),
                inet::L3Address(dynamic_cast<LocalAddressQoSPolicy*>(qosPolicyMap[QoSPolicyNames::LocalAddress])->getValue().c_str()),
                dynamic_cast<LocalPortQoSPolicy*>(qosPolicyMap[QoSPolicyNames::LocalPort])->getValue(),
                qosPolicyMap);
        if (_pendingRequestsMap.count(publisherServiceIdentifier.getServiceId())) {
            _pendingRequestsMap[publisherServiceIdentifier.getServiceId()].push_back(qosService);
        } else {
            std::list<QoSService> qosServices;
            qosServices.push_back(qosService);
            _pendingRequestsMap[publisherServiceIdentifier.getServiceId()] = qosServices;
        }
        _sd->discover(dynamic_cast<ServiceIdentifier&>(publisherServiceIdentifier));
    }
}

void LocalServiceManager::subscribeService(IServiceIdentifier& publisherServiceIdentifier) {
    // not sufficient for QoS service subscribing
    // TODO check if better method is possible
}

void LocalServiceManager::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj, cObject *details) {
    IService *service = dynamic_cast<IService*>(obj);
    for (auto &qosService : _pendingRequestsMap[service->getServiceId()]) {
        Request* request = createNegotiationRequest(service, qosService.getQoSPolicyMap());
        //create qos broker for the request
        _qosnp->createQoSBroker(request);
    }
    _pendingRequestsMap.erase(service->getServiceId());
    _lsr->addPublisherService(service);
}

void LocalServiceManager::addSubscriberServiceToConnector(SubscriberConnector* subscriberConnector, QoSPolicyMap& qosPolicies, SOQoSMWApplicationBase* executingApplication) {
    if(!(subscriberConnector->addApplication(executingApplication))){
        throw cRuntimeError("This Subscriber service already exists on this host...");
    }
}

Request* LocalServiceManager::createNegotiationRequest(IService* publisherService, QoSPolicyMap qosPolicies) {
    EndpointDescription subscriber(publisherService->getServiceId(), _localAddress, _qosnp->getProtocolPort());
    EndpointDescription publisher(publisherService->getServiceId(), publisherService->getAddress(), _qosnp->getProtocolPort());
    Request *request = new Request(_requestID++, subscriber, publisher, qosPolicies, nullptr);
    return request;
}

SubscriberConnector* LocalServiceManager::createSubscriberConnector(QoSPolicyMap& qosPolicies, SOQoSMWApplicationBase* executingApplication) {
    // create a connector for the subscriber
    // 1. Find the factory object;
    cModuleType *moduleType = cModuleType::get("soqosmw.connector.pubsub.reader.SubscriberConnector");
    // 2. Create the module;
    int vectorsize  = _subscriberConnectors.size();
    SubscriberConnector *module = dynamic_cast<SubscriberConnector*> (moduleType->create("subscriberConnectors", this->getParentModule(), vectorsize + 1, vectorsize));
    // 3. Set up its parameters and gate sizes as needed;
    module->addApplication(executingApplication);
    module->setQos(qosPolicies);
    module->finalizeParameters();
    // 4. Tell the (possibly compound) module to recursively create its internal submodules and connections;
    module->buildInside();
    module->callInitialize();
    // 5. Schedule activation message(s) for the new simple module(s).
    module->scheduleStart(simTime());
    return module;
}

SubscriberConnector* LocalServiceManager::getSubscriberConnector(uint32_t publisherServiceId) {
    return getSubscriberConnectorForServiceId(publisherServiceId);
}

PublisherConnector* LocalServiceManager::getPublisherConnectorForServiceId(
        uint32_t publisherServiceId) {
    auto connector = _publisherConnectors.find(publisherServiceId);
    if(connector != _publisherConnectors.end()){
        return connector->second;
    }
    return nullptr;
}


SubscriberConnector* LocalServiceManager::getSubscriberConnectorForServiceId(
        uint32_t publisherServiceId) {
    auto connector = _subscriberConnectors.find(publisherServiceId);
    if(connector != _subscriberConnectors.end()){
        return connector->second;
    }
    return nullptr;
}

PublisherEndpointBase* LocalServiceManager::createOrFindPublisherFor(
        uint32_t publisherServiceId, int qos) {

    PublisherEndpointBase* pub = nullptr;

    pub = findPublisherLike(publisherServiceId, qos);
    if(!pub){
        PublisherConnector* connector = getPublisherConnector(publisherServiceId);

        if(connector){
            //create according endpoint
            switch(qos){
            case QoSGroups::RT:
                pub = createAVBPublisherEndpoint(qos, connector);
                break;
            case QoSGroups::STD_TCP:
                pub = createTCPPublisherEndpoint(qos, connector);
                break;
            case QoSGroups::STD_UDP:
                pub = createUDPPublisherEndpoint(qos, connector);
                break;
            case QoSGroups::WEB:
                throw cRuntimeError("The web QoS Group is not yet available");
                break;
            //TODO SOMEIP case QoSGroups::SOMEIP for TCP
            case QoSGroups::SOMEIP:
                pub = createSOMEIPPublisherEndpoint(qos, connector);
                break;
            default:
                throw cRuntimeError("Unknown connection type.");
                break;
            }
            pub->finalizeParameters();
            // 4. Tell the (possibly compound) module to recursively create its internal submodules and connections;
            pub->buildInside();
            pub->callInitialize();
            // 5. Schedule activation message(s) for the new simple module(s).
            pub->scheduleStart(simTime());
        } else {
            throw cRuntimeError("Found no connector for the requested publisher so it is not needed.");
        }
    }
    return pub;
}

SubscriberEndpointBase* LocalServiceManager::createOrFindSubscriberFor(
        uint32_t publisherServiceId, ConnectionSpecificInformation* csi) {

    SubscriberEndpointBase* sub = nullptr;
    auto group = getQoSGroupForConnectionType(csi->getConnectionType());

    sub = findSubscriberLike(publisherServiceId, group);
    if(!sub){
        SubscriberConnector* connector = findSubscriberConnectorLike(publisherServiceId, group);

        if(connector){
            //create according endpoint
            switch(csi->getConnectionType()){
            case ConnectionType::ct_avb:
                sub = createAVBSubscriberEndpoint(csi, connector);
                break;
            case ConnectionType::ct_tcp:
                sub = createTCPSubscriberEndpoint(csi, connector);
                break;
            case ConnectionType::ct_udp:
                sub = createUDPSubscriberEndpoint(csi, connector);
                break;
            case ConnectionType::ct_http:
                throw cRuntimeError("The HTTP connection is not yet available");
                break;
            //TODO SOMEIP case ConnectionType::SOMEIP for TCP
            case ConnectionType::ct_someip:
                sub = createSOMEIPSubscriberEndpoint(csi, connector);
                break;
            default:
                throw cRuntimeError("Unknown connection type.");
                break;
            }
            sub->finalizeParameters();
            // 4. Tell the (possibly compound) module to recursively create its internal submodules and connections;
            sub->buildInside();
            sub->callInitialize();
            // 5. Schedule activation message(s) for the new simple module(s).
            sub->scheduleStart(simTime());
        } else {
            throw cRuntimeError("Found no connector for the requested subscriber so it is not needed.");
        }
    }
    return sub;
}

int LocalServiceManager::getQoSGroupForConnectionType(int type){
    switch(type){
    case ConnectionType::ct_avb:
        return QoSGroups::RT;
        break;
    case ConnectionType::ct_tcp:
        return QoSGroups::STD_TCP;
        break;
    case ConnectionType::ct_udp:
        return QoSGroups::STD_UDP;
        break;
    case ConnectionType::ct_http:
        return QoSGroups::WEB;
        break;
    //TODO SOMEIP for TCP
    case ConnectionType::ct_someip:
        return QoSGroups::SOMEIP;
        break;
    default:
        throw cRuntimeError("Unknown connection type.");
        break;
    }
    //return QoSGroup::RT;
}

PublisherEndpointBase* LocalServiceManager::findPublisherLike(
        uint32_t publisherServiceId, int qos) {

    // find fitting connectors
    PublisherConnector* connector = getPublisherConnector(publisherServiceId);
    if(connector){
        for (auto endpoint: connector->getEndpoints()){
            if(endpoint->getQos() == qos){
                return dynamic_cast<PublisherEndpointBase*>(endpoint);
            }
        }
    }

    return nullptr;
}

SubscriberConnector* LocalServiceManager::findSubscriberConnectorLike(
        uint32_t publisherServiceId, int qos) {

    // find fitting connectors
    SubscriberConnector* connector = getSubscriberConnector(publisherServiceId);
    if(connector){
        // we allready have a service subscribing to the data
        QoSGroup* conQoS = dynamic_cast<QoSGroup*>(connector->getQos()[QoSPolicyNames::QoSGroup]);
        if(qos == conQoS->getValue()){
            // same qos as well so try to add and return
            return connector;
        }
    }

    return nullptr;
}

SubscriberEndpointBase* LocalServiceManager::findSubscriberLike(
        uint32_t publisherServiceId, int qos) {

    // find fitting connectors
    SubscriberConnector* connector = findSubscriberConnectorLike(publisherServiceId, qos);
    if(connector){
        for (auto endpoint: connector->getEndpoints()){
            if(endpoint->getQos() == qos){
                return dynamic_cast<SubscriberEndpointBase*>(endpoint);
            }
        }
    }

    return nullptr;
}

SubscriberEndpointBase* LocalServiceManager::createAVBSubscriberEndpoint(
        ConnectionSpecificInformation* csi,
        SubscriberConnector* connector) {
    SubscriberEndpointBase* ret = nullptr;

    CSI_AVB* csi_avb = dynamic_cast<CSI_AVB*>(csi);

    if(csi_avb){
        // 1. Find the factory object;
        cModuleType * moduleType = cModuleType::get(
                    "soqosmw.endpoints.subscriber.realtime.avb.AVBSubscriberEndpoint");
        // 2. Create the module;
        AVBSubscriberEndpoint* avbEndpoint =
                            dynamic_cast<AVBSubscriberEndpoint*>(
                                    moduleType->create("subscriberEndpoints", this->getParentModule(), _subscriberEndpointCount + 1, _subscriberEndpointCount));
        _subscriberEndpointCount++;
        // 3. Set up its parameters and gate sizes as needed;
        avbEndpoint->par("updateInterval").setDoubleValue(par("updateInterval"));
        avbEndpoint->par("retryInterval").setDoubleValue(par("retryInterval"));
        avbEndpoint->par("streamID").setIntValue(csi_avb->getStreamID());
        avbEndpoint->par("vlan_id").setIntValue(csi_avb->getVlanID());

        // cast back.
        ret = dynamic_cast<SubscriberEndpointBase*>(avbEndpoint);
        //connect endpoint to the reader
        ret->setConnector(connector);
        connector->addEndpoint(ret);
    }

    return ret;

}

SubscriberEndpointBase* LocalServiceManager::createTCPSubscriberEndpoint(
        ConnectionSpecificInformation* csi,
        SubscriberConnector* connector) {
    SubscriberEndpointBase* ret = nullptr;

    CSI_TCP* csi_tcp = dynamic_cast<CSI_TCP*>(csi);

    if(csi_tcp){
        // 1. Find the factory object;
        cModuleType * moduleType = cModuleType::get(
                    "soqosmw.endpoints.subscriber.standard.tcp.TCPSubscriberEndpoint");
        // 2. Create the module;
        TCPSubscriberEndpoint* tcpEndpoint =
                            dynamic_cast<TCPSubscriberEndpoint*>(
                                    moduleType->create("subscriberEndpoints", this->getParentModule(), _subscriberEndpointCount + 1, _subscriberEndpointCount));
        _subscriberEndpointCount++;
        // 3. Set up its parameters and gate sizes as needed;
        string localAddr = (dynamic_cast<LocalAddressQoSPolicy*>(connector->getQos()[QoSPolicyNames::LocalAddress]))->getValue();
        tcpEndpoint->par("localAddress").setStringValue(localAddr);
        int localPort = (dynamic_cast<LocalPortQoSPolicy*>(connector->getQos()[QoSPolicyNames::LocalPort]))->getValue();
        tcpEndpoint->par("localPort").setIntValue(localPort);
        tcpEndpoint->par("remoteAddress").setStringValue(csi_tcp->getAddress());
        tcpEndpoint->par("remotePort").setIntValue(csi_tcp->getPort());

        // cast back.
        ret = dynamic_cast<SubscriberEndpointBase*>(tcpEndpoint);
        //connect endpoint to the reader
        ret->setConnector(connector);
        connector->addEndpoint(ret);
    }

    return ret;
}

SubscriberEndpointBase* LocalServiceManager::createUDPSubscriberEndpoint(
        ConnectionSpecificInformation* csi,
        SubscriberConnector* connector) {

    SubscriberEndpointBase* ret = nullptr;

    CSI_UDP* csi_udp = dynamic_cast<CSI_UDP*>(csi);

    if(csi_udp){
        // 1. Find the factory object;
        cModuleType * moduleType = cModuleType::get(
                    "soqosmw.endpoints.subscriber.standard.udp.UDPSubscriberEndpoint");
        // 2. Create the module;
        UDPSubscriberEndpoint* udpEndpoint =
                            dynamic_cast<UDPSubscriberEndpoint*>(
                                    moduleType->create("subscriberEndpoints", this->getParentModule(), _subscriberEndpointCount + 1, _subscriberEndpointCount));
        _subscriberEndpointCount++;
        // 3. Set up its parameters and gate sizes as needed;
        string localAddr = (dynamic_cast<LocalAddressQoSPolicy*>(connector->getQos()[QoSPolicyNames::LocalAddress]))->getValue();
        udpEndpoint->par("localAddress").setStringValue(localAddr);
        int localPort = (dynamic_cast<LocalPortQoSPolicy*>(connector->getQos()[QoSPolicyNames::LocalPort]))->getValue();
        udpEndpoint->par("localPort").setIntValue(localPort);

        // cast back.
        ret = dynamic_cast<SubscriberEndpointBase*>(udpEndpoint);
        //connect endpoint to the reader
        ret->setConnector(connector);
        connector->addEndpoint(ret);
    }

    return ret;
}

SubscriberEndpointBase* LocalServiceManager::createSOMEIPSubscriberEndpoint(
        ConnectionSpecificInformation* csi,
        SubscriberConnector* connector) {

    SubscriberEndpointBase* ret = nullptr;

    CSI_SOMEIP* csi_someip = dynamic_cast<CSI_SOMEIP*>(csi);

    if(csi_someip){
        // 1. Find the factory object;
        cModuleType * moduleType = cModuleType::get(
                    "soqosmw.endpoints.subscriber.someip.udp.SOMEIPUDPSubscriberEndpoint");
        // 2. Create the module;
        SOMEIPUDPSubscriberEndpoint* someipUdpEndpoint =
                            dynamic_cast<SOMEIPUDPSubscriberEndpoint*>(
                                    moduleType->create("subscriberEndpoints", this->getParentModule(), _subscriberEndpointCount + 1, _subscriberEndpointCount));
        _subscriberEndpointCount++;
        // 3. Set up its parameters and gate sizes as needed;
        string localAddr = (dynamic_cast<LocalAddressQoSPolicy*>(connector->getQos()[QoSPolicyNames::LocalAddress]))->getValue();
        someipUdpEndpoint->par("localAddress").setStringValue(localAddr);
        int localPort = (dynamic_cast<LocalPortQoSPolicy*>(connector->getQos()[QoSPolicyNames::LocalPort]))->getValue();
        someipUdpEndpoint->par("localPort").setIntValue(localPort);

        // cast back.
        ret = dynamic_cast<SubscriberEndpointBase*>(someipUdpEndpoint);
        //connect endpoint to the reader
        ret->setConnector(connector);
        connector->addEndpoint(ret);
    }

    return ret;
}

PublisherEndpointBase* LocalServiceManager::createAVBPublisherEndpoint(
        int qos,
        PublisherConnector* connector) {

    PublisherEndpointBase* ret = nullptr;

    if(qos == QoSGroups::RT){
        // 1. Find the factory object;
        cModuleType * moduleType = cModuleType::get(
                    "soqosmw.endpoints.publisher.realtime.avb.AVBPublisherEndpoint");
        // 2. Create the module;
        AVBPublisherEndpoint* avbEndpoint =
                            dynamic_cast<AVBPublisherEndpoint*>(
                                    moduleType->create("publisherEndpoints", this->getParentModule(), _publisherEndpointCount + 1, _publisherEndpointCount));
        _publisherEndpointCount++;
        // 3. Set up its parameters and gate sizes as needed;
        auto streamID = (dynamic_cast<StreamIDQoSPolicy*>(connector->getQos()[QoSPolicyNames::StreamID]))->getValue();
        avbEndpoint->par("streamID").setIntValue(streamID);

        auto srClass = (dynamic_cast<SRClassQoSPolicy*>(connector->getQos()[QoSPolicyNames::SRClass]))->getValue();
        string strSrClass;
        switch(srClass){
        case SR_CLASS::A: strSrClass = "A"; break;
        case SR_CLASS::B: strSrClass = "B"; break;
        default: break;
        }
        avbEndpoint->par("srClass").setStringValue(strSrClass);

        auto intervalFrames = (dynamic_cast<IntervalFramesQoSPolicy*>(connector->getQos()[QoSPolicyNames::IntervalFrames]))->getValue();
        avbEndpoint->par("intervalFrames").setIntValue(intervalFrames);

        auto vlanID = (dynamic_cast<IntervalFramesQoSPolicy*>(connector->getQos()[QoSPolicyNames::IntervalFrames]))->getValue();
        avbEndpoint->par("vlan_id").setIntValue(vlanID);

        auto payload = (dynamic_cast<FramesizeQoSPolicy*>(connector->getQos()[QoSPolicyNames::Framesize]))->getValue();
        avbEndpoint->par("payload").setIntValue(payload);

        // cast back.
        ret = dynamic_cast<PublisherEndpointBase*>(avbEndpoint);
        //connect endpoint to the reader
        ret->setConnector(connector);
        connector->addEndpoint(ret);
    }

    return ret;
}

PublisherEndpointBase* LocalServiceManager::createTCPPublisherEndpoint(
        int qos,
        PublisherConnector* connector) {

    PublisherEndpointBase* ret = nullptr;

    if(qos == QoSGroups::STD_TCP){
        // 1. Find the factory object;
        cModuleType * moduleType = cModuleType::get(
                    "soqosmw.endpoints.publisher.standard.tcp.TCPPublisherEndpoint");
        // 2. Create the module;
        TCPPublisherEndpoint* tcpEndpoint =
                            dynamic_cast<TCPPublisherEndpoint*>(
                                    moduleType->create("publisherEndpoints", this->getParentModule(), _publisherEndpointCount + 1, _publisherEndpointCount));
        _publisherEndpointCount++;
        // 3. Set up its parameters and gate sizes as needed;
        string localAddr = (dynamic_cast<LocalAddressQoSPolicy*>(connector->getQos()[QoSPolicyNames::LocalAddress]))->getValue();
        tcpEndpoint->par("localAddress").setStringValue(localAddr);
        int localPort = (dynamic_cast<LocalPortQoSPolicy*>(connector->getQos()[QoSPolicyNames::LocalPort]))->getValue();
        tcpEndpoint->par("localPort").setIntValue(localPort);

        // cast back.
        ret = dynamic_cast<PublisherEndpointBase*>(tcpEndpoint);
        //connect endpoint to the reader
        ret->setConnector(connector);
        connector->addEndpoint(ret);
    }

    return ret;
}

PublisherEndpointBase* LocalServiceManager::createUDPPublisherEndpoint(
        int qos,
        PublisherConnector* connector) {

    PublisherEndpointBase* ret = nullptr;

    if(qos == QoSGroups::STD_UDP){
        // 1. Find the factory object;
        cModuleType * moduleType = cModuleType::get(
                    "soqosmw.endpoints.publisher.standard.udp.UDPPublisherEndpoint");
        // 2. Create the module;
        UDPPublisherEndpoint* udpEndpoint =
                            dynamic_cast<UDPPublisherEndpoint*>(
                                    moduleType->create("publisherEndpoints", this->getParentModule(), _publisherEndpointCount + 1, _publisherEndpointCount));
        _publisherEndpointCount++;
        // 3. Set up its parameters and gate sizes as needed;
        string localAddr = (dynamic_cast<LocalAddressQoSPolicy*>(connector->getQos()[QoSPolicyNames::LocalAddress]))->getValue();
        udpEndpoint->par("localAddress").setStringValue(localAddr);
        int localPort = (dynamic_cast<LocalPortQoSPolicy*>(connector->getQos()[QoSPolicyNames::LocalPort]))->getValue();
        udpEndpoint->par("localPort").setIntValue(localPort);

        // cast back.
        ret = dynamic_cast<PublisherEndpointBase*>(udpEndpoint);
        //connect endpoint to the reader
        ret->setConnector(connector);
        connector->addEndpoint(ret);
    }

    return ret;
}

PublisherEndpointBase* LocalServiceManager::createSOMEIPPublisherEndpoint(
        int qos,
        PublisherConnector* connector) {

    PublisherEndpointBase* ret = nullptr;

    if(qos == QoSGroups::SOMEIP){
        // 1. Find the factory object;
        cModuleType * moduleType = cModuleType::get(
                    "soqosmw.endpoints.publisher.someip.udp.SOMEIPUDPPublisherEndpoint");
        // 2. Create the module;
        SOMEIPUDPPublisherEndpoint* someipUdpEndpoint =
                            dynamic_cast<SOMEIPUDPPublisherEndpoint*>(
                                    moduleType->create("publisherEndpoints", this->getParentModule(), _publisherEndpointCount + 1, _publisherEndpointCount));
        _publisherEndpointCount++;
        // 3. Set up its parameters and gate sizes as needed;
        string localAddr = (dynamic_cast<LocalAddressQoSPolicy*>(connector->getQos()[QoSPolicyNames::LocalAddress]))->getValue();
        someipUdpEndpoint->par("localAddress").setStringValue(localAddr);
        int localPort = (dynamic_cast<LocalPortQoSPolicy*>(connector->getQos()[QoSPolicyNames::LocalPort]))->getValue();
        someipUdpEndpoint->par("localPort").setIntValue(localPort);

        // cast back.
        ret = dynamic_cast<PublisherEndpointBase*>(someipUdpEndpoint);
        //connect endpoint to the reader
        ret->setConnector(connector);
        connector->addEndpoint(ret);
    }

    return ret;
}

} /* end namespace  */
