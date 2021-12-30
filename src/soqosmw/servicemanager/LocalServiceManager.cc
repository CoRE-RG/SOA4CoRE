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
#include "soqosmw/service/qosservice/QoSService.h"
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

void LocalServiceManager::registerPublisherService(QoSService qosService, SOQoSMWApplicationBase* executingApplication) {
    Enter_Method("LSM:registerPublisherService()");

    PublisherConnector* publisherConnector = getPublisherConnectorForServiceId(qosService.getServiceId());
    if (publisherConnector != nullptr) {
        addPublisherServiceToConnector(publisherConnector, executingApplication);
    } else {
        publisherConnector = createPublisherConnector(qosService, executingApplication);
        //save the writer so that new endpoints can be connected to the application.
        _publisherConnectors[qosService.getServiceId()] = publisherConnector;
    }
    _lsr->addPublisherService(qosService);
}

void LocalServiceManager::addPublisherServiceToConnector(PublisherConnector* publisherConnector, SOQoSMWApplicationBase* executingApplication) {
    if(!(publisherConnector->addApplication(executingApplication))){
        throw cRuntimeError("This Publisher service already exists on this host...");
    }
}

PublisherConnector* LocalServiceManager::createPublisherConnector(QoSService qosService, SOQoSMWApplicationBase* executingApplication) {
    // create a connector for the publisher
    // 1. Find the factory object;
    cModuleType *moduleType = cModuleType::get("soqosmw.connector.pubsub.writer.PublisherConnector");
    // 2. Create the module;
    int vectorsize = _publisherConnectors.size();
    PublisherConnector *module = dynamic_cast<PublisherConnector*> (moduleType->create("publisherConnectors", this->getParentModule(), vectorsize + 1, vectorsize));
    // 3. Set up its parameters and gate sizes as needed;
    module->addApplication(executingApplication);
    module->setQoSService(qosService);
    module->finalizeParameters();
    // 4. Tell the (possibly compound) module to recursively create its internal submodules and connections;
    module->buildInside();
    module->callInitialize();
    // 5. Schedule activation message(s) for the new simple module(s).
    module->scheduleStart(simTime());
    return module;
}

void LocalServiceManager::registerSubscriberService(QoSService qosService, SOQoSMWApplicationBase* executingApplication)
{
    Enter_Method("LSM:registerSubscriberService()");

    std::vector<SubscriberConnector*> subscriberConnectors = getSubscriberConnectorsForServiceId(qosService.getServiceId());
    SubscriberConnector* subscriberConnector = nullptr;
    if(!subscriberConnectors.empty()) {
        for (SubscriberConnector* connector : subscriberConnectors) {
            QoSService connectedQosService = connector->getQoSService();
            if (QoSGroups* qosGroup = connectedQosService.getCommonQoSGroup(qosService)) {
                subscriberConnector = connector;
                addSubscriberServiceToConnector(subscriberConnector, executingApplication);
                delete qosGroup;
                break;
            }
        }
    }
    if (!subscriberConnector) {
        subscriberConnector = createSubscriberConnector(qosService, executingApplication);
        subscriberConnectors = _subscriberConnectors[qosService.getServiceId()];
        subscriberConnectors.push_back(subscriberConnector);
        // save the reader so that new endpoints can be connected to the application.
        _subscriberConnectors[qosService.getServiceId()] = subscriberConnectors;
    }
}

void LocalServiceManager::subscribeService(QoSServiceIdentifier publisherServiceIdentifier, QoSService qosService) {
    throw cRuntimeError("LocalServiceManager has no implementation of subscribeService. \n"
            "Override or use ServiceManager which implements subscribeService");
}

void LocalServiceManager::addServiceToPendingRequestsMap(QoSServiceIdentifier publisherServiceIdentifier, QoSService qosService) {
    if (_pendingRequestsMap.count(publisherServiceIdentifier.getServiceId())) {
        _pendingRequestsMap[publisherServiceIdentifier.getServiceId()].push_back(qosService);
    } else {
        std::list<QoSService> qosServices;
        qosServices.push_back(qosService);
        _pendingRequestsMap[publisherServiceIdentifier.getServiceId()] = qosServices;
    }
}

void LocalServiceManager::addSubscriberServiceToConnector(SubscriberConnector* subscriberConnector, SOQoSMWApplicationBase* executingApplication) {
    if(!(subscriberConnector->addApplication(executingApplication))){
        throw cRuntimeError("This Subscriber service already exists on this host...");
    }
}

SubscriberConnector* LocalServiceManager::createSubscriberConnector(QoSService qosService, SOQoSMWApplicationBase* executingApplication) {
    // create a connector for the subscriber
    // 1. Find the factory object;
    cModuleType *moduleType = cModuleType::get("soqosmw.connector.pubsub.reader.SubscriberConnector");
    // 2. Create the module;
    int vectorsize  = _subscriberConnectors.size();
    SubscriberConnector *module = dynamic_cast<SubscriberConnector*> (moduleType->create("subscriberConnectors", this->getParentModule(), vectorsize + 1, vectorsize));
    // 3. Set up its parameters and gate sizes as needed;
    module->addApplication(executingApplication);
    module->setQoSService(qosService);
    module->finalizeParameters();
    // 4. Tell the (possibly compound) module to recursively create its internal submodules and connections;
    module->buildInside();
    module->callInitialize();
    // 5. Schedule activation message(s) for the new simple module(s).
    module->scheduleStart(simTime());
    return module;
}

PublisherConnector* LocalServiceManager::getPublisherConnectorForServiceId(
        uint32_t publisherServiceId) {
    PublisherConnector* publisherConnector = nullptr;
    auto connector = _publisherConnectors.find(publisherServiceId);
    if(connector != _publisherConnectors.end()){
        publisherConnector = connector->second;
    }
    return publisherConnector;
}


std::vector<SubscriberConnector*> LocalServiceManager::getSubscriberConnectorsForServiceId(
        uint32_t publisherServiceId) {
    vector<SubscriberConnector*> subscriberConnectors;
    auto connector = _subscriberConnectors.find(publisherServiceId);
    if(connector != _subscriberConnectors.end()){
        vector<SubscriberConnector*> subscriberConnectors = connector->second;
        subscriberConnectors = connector->second;
    }
    return subscriberConnectors;
}

PublisherEndpointBase* LocalServiceManager::createOrFindPublisherFor(
        uint32_t publisherServiceId, QoSGroups qosGroup) {

    PublisherEndpointBase* pub = nullptr;

    pub = findPublisherLike(publisherServiceId, qosGroup);
    if(!pub){
        PublisherConnector* connector = getPublisherConnectorForServiceId(publisherServiceId);

        if(connector){
            //create according endpoint
            switch(qosGroup){
            case QoSGroups::RT:
                pub = createAVBPublisherEndpoint(qosGroup, connector);
                break;
            case QoSGroups::STD_TCP:
                pub = createTCPPublisherEndpoint(qosGroup, connector);
                break;
            case QoSGroups::STD_UDP:
                pub = createUDPPublisherEndpoint(qosGroup, connector);
                break;
            case QoSGroups::WEB:
                throw cRuntimeError("The web QoS Group is not yet available");
                break;
            case QoSGroups::SOMEIP_TCP:
                pub = createSomeIpTCPPublisherEndpoint(qosGroup, connector);
                break;
            case QoSGroups::SOMEIP_UDP:
                pub = createSomeIpUDPPublisherEndpoint(qosGroup, connector);
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
    QoSGroups qosGroup = getQoSGroupForConnectionType(csi->getConnectionType());

    sub = findSubscriberLike(publisherServiceId, qosGroup);
    if(!sub){
        SubscriberConnector* connector = findSubscriberConnectorLike(publisherServiceId, qosGroup);

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
            case ConnectionType::ct_someip_tcp:
                sub = createSomeIpTCPSubscriberEndpoint(csi, connector);
                break;
            case ConnectionType::ct_someip_udp:
                sub = createSomeIpUDPSubscriberEndpoint(csi, connector);
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

QoSGroups LocalServiceManager::getQoSGroupForConnectionType(int connectionType){
    switch(connectionType){
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
    case ConnectionType::ct_someip_tcp:
        return QoSGroups::SOMEIP_TCP;
        break;
    case ConnectionType::ct_someip_udp:
        return QoSGroups::SOMEIP_UDP;
        break;
    default:
        throw cRuntimeError("Unknown connection type.");
        break;
    }
}

PublisherEndpointBase* LocalServiceManager::findPublisherLike(
        uint32_t publisherServiceId, QoSGroups qosGroup) {

    // find fitting connectors
    PublisherConnector* connector = getPublisherConnectorForServiceId(publisherServiceId);
    if(connector){
        for (auto endpoint: connector->getEndpoints()){
            if(endpoint->getQos() == qosGroup){
                return dynamic_cast<PublisherEndpointBase*>(endpoint);
            }
        }
    }

    return nullptr;
}

SubscriberConnector* LocalServiceManager::findSubscriberConnectorLike(
        uint32_t publisherServiceId, QoSGroups qosGroup) {

    // find fitting connectors
    SubscriberConnector* subscriberConnector = nullptr;
    std::vector<SubscriberConnector*> subscriberconnectors = getSubscriberConnectorsForServiceId(publisherServiceId);
    for (SubscriberConnector* connector : subscriberconnectors) {
        QoSService qosService = connector->getQoSService();
        if (connector->getQoSService().containsQoSGroup(qosGroup)) {
            subscriberConnector = connector;
            break;
        }
    }
    return subscriberConnector;
}

SubscriberEndpointBase* LocalServiceManager::findSubscriberLike(
        uint32_t publisherServiceId, QoSGroups qosGroup) {

    // find fitting connectors
    SubscriberConnector* connector = findSubscriberConnectorLike(publisherServiceId, qosGroup);
    if(connector){
        for (auto endpoint: connector->getEndpoints()){
            if(endpoint->getQos() == qosGroup){
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
        string localAddr = connector->getQoSService().getAddress().str();
        tcpEndpoint->par("localAddress").setStringValue(localAddr);
        int localPort = connector->getQoSService().getTCPPort();
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
        string localAddr = connector->getQoSService().getAddress().str();
        udpEndpoint->par("localAddress").setStringValue(localAddr);
        int localPort = connector->getQoSService().getUDPPort();
        udpEndpoint->par("localPort").setIntValue(localPort);

        // cast back.
        ret = dynamic_cast<SubscriberEndpointBase*>(udpEndpoint);
        //connect endpoint to the reader
        ret->setConnector(connector);
        connector->addEndpoint(ret);
    }

    return ret;
}


SubscriberEndpointBase* LocalServiceManager::createSomeIpTCPSubscriberEndpoint(
        ConnectionSpecificInformation* csi,
        SubscriberConnector* connector) {
    SubscriberEndpointBase* ret = nullptr;

    CSI_SOMEIP_TCP* csi_someip = dynamic_cast<CSI_SOMEIP_TCP*>(csi);

    if(csi_someip){
        // 1. Find the factory object;
        cModuleType * moduleType = cModuleType::get(
                    "soqosmw.endpoints.subscriber.someip.tcp.SOMEIPTCPSubscriberEndpoint");
        // 2. Create the module;
        SOMEIPTCPSubscriberEndpoint* someipTcpEndpoint =
                            dynamic_cast<SOMEIPTCPSubscriberEndpoint*>(
                                    moduleType->create("subscriberEndpoints", this->getParentModule(), _subscriberEndpointCount + 1, _subscriberEndpointCount));
        _subscriberEndpointCount++;
        // 3. Set up its parameters and gate sizes as needed;
        string localAddr = connector->getQoSService().getAddress().str();
        someipTcpEndpoint->par("localAddress").setStringValue(localAddr);
        int localPort = connector->getQoSService().getTCPPort();
        someipTcpEndpoint->par("localPort").setIntValue(localPort);
        someipTcpEndpoint->par("remoteAddress").setStringValue(csi_someip->getAddress());
        someipTcpEndpoint->par("remotePort").setIntValue(csi_someip->getPort());

        // cast back.
        ret = dynamic_cast<SubscriberEndpointBase*>(someipTcpEndpoint);
        //connect endpoint to the reader
        ret->setConnector(connector);
        connector->addEndpoint(ret);
    }

    return ret;
}

SubscriberEndpointBase* LocalServiceManager::createSomeIpUDPSubscriberEndpoint(
        ConnectionSpecificInformation* csi,
        SubscriberConnector* connector) {

    SubscriberEndpointBase* ret = nullptr;

    CSI_SOMEIP_UDP* csi_someip = dynamic_cast<CSI_SOMEIP_UDP*>(csi);

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
        string localAddr = connector->getQoSService().getAddress().str();
        someipUdpEndpoint->par("localAddress").setStringValue(localAddr);
        int localPort = connector->getQoSService().getUDPPort();
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
        auto streamID = connector->getQoSService().getStreamId();
        avbEndpoint->par("streamID").setIntValue(streamID);

        auto srClass = connector->getQoSService().getSrClass();
        string strSrClass;
        switch(srClass){
        case SR_CLASS::A: strSrClass = "A"; break;
        case SR_CLASS::B: strSrClass = "B"; break;
        default: break;
        }
        avbEndpoint->par("srClass").setStringValue(strSrClass);

        auto intervalFrames = connector->getQoSService().getIntervalFrames();
        avbEndpoint->par("intervalFrames").setIntValue(intervalFrames);

        auto vlanID = connector->getQoSService().getIntervalFrames();
        avbEndpoint->par("vlan_id").setIntValue(vlanID);

        auto payload = connector->getQoSService().getFramesize();
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
        string localAddr = connector->getQoSService().getAddress().str();
        tcpEndpoint->par("localAddress").setStringValue(localAddr);
        int localPort = connector->getQoSService().getTCPPort();
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
        string localAddr = connector->getQoSService().getAddress().str();
        udpEndpoint->par("localAddress").setStringValue(localAddr);
        int localPort = connector->getQoSService().getUDPPort();
        udpEndpoint->par("localPort").setIntValue(localPort);

        // cast back.
        ret = dynamic_cast<PublisherEndpointBase*>(udpEndpoint);
        //connect endpoint to the reader
        ret->setConnector(connector);
        connector->addEndpoint(ret);
    }

    return ret;
}

PublisherEndpointBase* LocalServiceManager::createSomeIpTCPPublisherEndpoint(
        int qos,
        PublisherConnector* connector) {

    PublisherEndpointBase* ret = nullptr;

    if(qos == QoSGroups::SOMEIP_TCP){
        // 1. Find the factory object;
        cModuleType * moduleType = cModuleType::get(
                    "soqosmw.endpoints.publisher.someip.tcp.SOMEIPTCPPublisherEndpoint");
        // 2. Create the module;
        SOMEIPTCPPublisherEndpoint* someipTcpEndpoint =
                            dynamic_cast<SOMEIPTCPPublisherEndpoint*>(
                                    moduleType->create("publisherEndpoints", this->getParentModule(), _publisherEndpointCount + 1, _publisherEndpointCount));
        _publisherEndpointCount++;
        // 3. Set up its parameters and gate sizes as needed;
        string localAddr = connector->getQoSService().getAddress().str();
        someipTcpEndpoint->par("localAddress").setStringValue(localAddr);
        int localPort = connector->getQoSService().getTCPPort();
        someipTcpEndpoint->par("localPort").setIntValue(localPort);

        // cast back.
        ret = dynamic_cast<PublisherEndpointBase*>(someipTcpEndpoint);
        //connect endpoint to the reader
        ret->setConnector(connector);
        connector->addEndpoint(ret);
    }
    return ret;
}

PublisherEndpointBase* LocalServiceManager::createSomeIpUDPPublisherEndpoint(
        int qos,
        PublisherConnector* connector) {

    PublisherEndpointBase* ret = nullptr;

    if(qos == QoSGroups::SOMEIP_UDP){
        // 1. Find the factory object;
        cModuleType * moduleType = cModuleType::get(
                    "soqosmw.endpoints.publisher.someip.udp.SOMEIPUDPPublisherEndpoint");
        // 2. Create the module;
        SOMEIPUDPPublisherEndpoint* someipUdpEndpoint =
                            dynamic_cast<SOMEIPUDPPublisherEndpoint*>(
                                    moduleType->create("publisherEndpoints", this->getParentModule(), _publisherEndpointCount + 1, _publisherEndpointCount));
        _publisherEndpointCount++;
        // 3. Set up its parameters and gate sizes as needed;
        string localAddr = connector->getQoSService().getAddress().str();
        someipUdpEndpoint->par("localAddress").setStringValue(localAddr);
        int localPort = connector->getQoSService().getUDPPort();
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
