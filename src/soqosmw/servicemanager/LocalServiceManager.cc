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

#include <soqosmw/service/applicationinformation/ApplicationInformation.h>
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
       // WATCH_MAP(_subscriberConnectors); TODO is it possible to watch a map with list as value?
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

void LocalServiceManager::registerPublisherService(PublisherApplicationInformation publisherApplicationInformation, SOQoSMWApplicationBase* executingApplication) {
    Enter_Method("LSM:registerPublisherService()");

    PublisherConnector* publisherConnector = getPublisherConnectorForServiceId(publisherApplicationInformation.getServiceId());
    if (!publisherConnector) {
        publisherConnector = createPublisherConnector(publisherApplicationInformation, executingApplication);
        _publisherConnectors[publisherApplicationInformation.getServiceId()] = publisherConnector;
        _lsr->addPublisherService(publisherApplicationInformation);
    } else {
        throw cRuntimeError("There can not be multiple publisher services with same ID");
    }
}

PublisherConnector* LocalServiceManager::createPublisherConnector(PublisherApplicationInformation publisherApplicationInformation, SOQoSMWApplicationBase* executingApplication) {
    // create a connector for the publisher
    // 1. Find the factory object;
    cModuleType *moduleType = cModuleType::get("soqosmw.connector.pubsub.writer.PublisherConnector");
    // 2. Create the module;
    int vectorsize = _publisherConnectors.size();
    PublisherConnector *module = dynamic_cast<PublisherConnector*> (moduleType->create("publisherConnectors", this->getParentModule(), vectorsize + 1, vectorsize));
    // 3. Set up its parameters and gate sizes as needed;
    module->setApplication(executingApplication);
    module->setPublisherApplicationInformation(publisherApplicationInformation);
    module->finalizeParameters();
    // 4. Tell the (possibly compound) module to recursively create its internal submodules and connections;
    module->buildInside();
    module->callInitialize();
    // 5. Schedule activation message(s) for the new simple module(s).
    module->scheduleStart(simTime());
    return module;
}

void LocalServiceManager::registerSubscriberService(SubscriberApplicationInformation subscriberApplicationInformation, SOQoSMWApplicationBase* executingApplication)
{
    Enter_Method("LSM:registerSubscriberService()");

    std::vector<SubscriberConnector*> subscriberConnectors = getSubscriberConnectorsForServiceId(subscriberApplicationInformation.getServiceId());
    SubscriberConnector* subscriberConnector = nullptr;
    if(!subscriberConnectors.empty()) {
        for (SubscriberConnector* connector : subscriberConnectors) {
            SubscriberApplicationInformation connectedSubscriberApplicationInformation = connector->getSubscriberApplicationInformation();
            if (connectedSubscriberApplicationInformation.getQoSGroup() == subscriberApplicationInformation.getQoSGroup()) {
                subscriberConnector = connector;
                addSubscriberServiceToConnector(subscriberConnector, executingApplication);
                break;
            }
        }
    }
    if (!subscriberConnector) {
        subscriberConnector = createSubscriberConnector(subscriberApplicationInformation, executingApplication);
        subscriberConnectors = _subscriberConnectors[subscriberApplicationInformation.getServiceId()];
        subscriberConnectors.push_back(subscriberConnector);
        // save the reader so that new endpoints can be connected to the application.
        _subscriberConnectors[subscriberApplicationInformation.getServiceId()] = subscriberConnectors;
    }
}

void LocalServiceManager::subscribeService(QoSServiceIdentifier publisherServiceIdentifier, SubscriberApplicationInformation subscriberApplicationInformation) {
    throw cRuntimeError("LocalServiceManager has no implementation of subscribeService. \n"
            "Override or use ServiceManager which implements subscribeService");
}

void LocalServiceManager::addSubscriberToPendingRequestsMap(QoSServiceIdentifier publisherServiceIdentifier, SubscriberApplicationInformation subscriberApplicationInformation) {
    if (_pendingRequestsMap.count(publisherServiceIdentifier.getServiceId())) {
        _pendingRequestsMap[publisherServiceIdentifier.getServiceId()].push_back(subscriberApplicationInformation);
    } else {
        std::list<SubscriberApplicationInformation> subscriberApplicationInformations;
        subscriberApplicationInformations.push_back(subscriberApplicationInformation);
        _pendingRequestsMap[publisherServiceIdentifier.getServiceId()] = subscriberApplicationInformations;
    }
}

void LocalServiceManager::addSubscriberServiceToConnector(SubscriberConnector* subscriberConnector, SOQoSMWApplicationBase* executingApplication) {
    if(!(subscriberConnector->addApplication(executingApplication))){
        throw cRuntimeError("This Subscriber service already exists on this host...");
    }
}

SubscriberConnector* LocalServiceManager::createSubscriberConnector(SubscriberApplicationInformation subscriberApplicationInformation, SOQoSMWApplicationBase* executingApplication) {
    // create a connector for the subscriber
    // 1. Find the factory object;
    cModuleType *moduleType = cModuleType::get("soqosmw.connector.pubsub.reader.SubscriberConnector");
    // 2. Create the module;
    int vectorsize  = _subscriberConnectors.size();
    SubscriberConnector *module = dynamic_cast<SubscriberConnector*> (moduleType->create("subscriberConnectors", this->getParentModule(), vectorsize + 1, vectorsize));
    // 3. Set up its parameters and gate sizes as needed;
    module->addApplication(executingApplication);
    module->setSubscriberApplicationInformation(subscriberApplicationInformation);
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
        subscriberConnectors = connector->second;
    }
    return subscriberConnectors;
}

PublisherEndpointBase* LocalServiceManager::createOrFindPublisherFor(
        uint32_t publisherServiceId, QoSGroup qosGroup) {

    PublisherEndpointBase* pub = nullptr;

    pub = findPublisherLike(publisherServiceId, qosGroup);
    if(!pub){
        PublisherConnector* connector = getPublisherConnectorForServiceId(publisherServiceId);

        if(connector){
            //create according endpoint
            switch(qosGroup){
            case QoSGroup::RT:
                pub = createAVBPublisherEndpoint(qosGroup, connector);
                break;
            case QoSGroup::STD_TCP:
                pub = createTCPPublisherEndpoint(qosGroup, connector);
                break;
            case QoSGroup::STD_UDP:
                pub = createUDPPublisherEndpoint(qosGroup, connector);
                break;
            case QoSGroup::WEB:
                throw cRuntimeError("The web QoS Group is not yet available");
                break;
            case QoSGroup::SOMEIP_TCP:
                pub = createSomeIpTCPPublisherEndpoint(qosGroup, connector);
                break;
            case QoSGroup::SOMEIP_UDP:
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
    QoSGroup qosGroup = getQoSGroupForConnectionType(ConnectionType(csi->getConnectionType()));
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

QoSGroup LocalServiceManager::getQoSGroupForConnectionType(ConnectionType connectionType){
    switch(connectionType){
    case ConnectionType::ct_avb:
        return QoSGroup::RT;
        break;
    case ConnectionType::ct_tcp:
        return QoSGroup::STD_TCP;
        break;
    case ConnectionType::ct_udp:
        return QoSGroup::STD_UDP;
        break;
    case ConnectionType::ct_http:
        return QoSGroup::WEB;
        break;
    case ConnectionType::ct_someip_tcp:
        return QoSGroup::SOMEIP_TCP;
        break;
    case ConnectionType::ct_someip_udp:
        return QoSGroup::SOMEIP_UDP;
        break;
    default:
        throw cRuntimeError("Unknown connection type.");
        break;
    }
}

PublisherEndpointBase* LocalServiceManager::findPublisherLike(
        uint32_t publisherServiceId, QoSGroup qosGroup) {

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
        uint32_t publisherServiceId, QoSGroup qosGroup) {

    // find fitting connectors
    SubscriberConnector* subscriberConnector = nullptr;
    std::vector<SubscriberConnector*> subscriberconnectors = getSubscriberConnectorsForServiceId(publisherServiceId);
    for (SubscriberConnector* connector : subscriberconnectors) {
        SubscriberApplicationInformation subscriberApplicationInformation = connector->getSubscriberApplicationInformation();
        if (subscriberApplicationInformation.getQoSGroup() == qosGroup) {
            subscriberConnector = connector;
            break;
        }
    }
    return subscriberConnector;
}

SubscriberEndpointBase* LocalServiceManager::findSubscriberLike(
        uint32_t publisherServiceId, QoSGroup qosGroup) {

    // find fitting connectors
    SubscriberConnector* connector = findSubscriberConnectorLike(publisherServiceId, qosGroup);
    return connector->getEndpoint();
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
        ret->setSubscriberConnector(connector);
        connector->setEndpoint(ret);
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
        string localAddr = connector->getSubscriberApplicationInformation().getAddress().str();
        tcpEndpoint->par("localAddress").setStringValue(localAddr);
        int localPort = connector->getSubscriberApplicationInformation().getTCPPort();
        tcpEndpoint->par("localPort").setIntValue(localPort);
        tcpEndpoint->par("remoteAddress").setStringValue(csi_tcp->getAddress());
        tcpEndpoint->par("remotePort").setIntValue(csi_tcp->getPort());

        // cast back.
        ret = dynamic_cast<SubscriberEndpointBase*>(tcpEndpoint);
        //connect endpoint to the reader
        ret->setSubscriberConnector(connector);
        connector->setEndpoint(ret);
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
        string localAddr = connector->getSubscriberApplicationInformation().getAddress().str();
        udpEndpoint->par("localAddress").setStringValue(localAddr);
        int localPort = connector->getSubscriberApplicationInformation().getUDPPort();
        udpEndpoint->par("localPort").setIntValue(localPort);

        // cast back.
        ret = dynamic_cast<SubscriberEndpointBase*>(udpEndpoint);
        //connect endpoint to the reader
        ret->setSubscriberConnector(connector);
        connector->setEndpoint(ret);
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
        string localAddr = connector->getSubscriberApplicationInformation().getAddress().str();
        someipTcpEndpoint->par("localAddress").setStringValue(localAddr);
        int localPort = connector->getSubscriberApplicationInformation().getTCPPort();
        someipTcpEndpoint->par("localPort").setIntValue(localPort);
        someipTcpEndpoint->par("remoteAddress").setStringValue(csi_someip->getAddress());
        someipTcpEndpoint->par("remotePort").setIntValue(csi_someip->getPort());

        // cast back.
        ret = dynamic_cast<SubscriberEndpointBase*>(someipTcpEndpoint);
        //connect endpoint to the reader
        ret->setSubscriberConnector(connector);
        connector->setEndpoint(ret);
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
        string localAddr = connector->getSubscriberApplicationInformation().getAddress().str();
        someipUdpEndpoint->par("localAddress").setStringValue(localAddr);
        int localPort = connector->getSubscriberApplicationInformation().getUDPPort();
        someipUdpEndpoint->par("localPort").setIntValue(localPort);

        // cast back.
        ret = dynamic_cast<SubscriberEndpointBase*>(someipUdpEndpoint);
        //connect endpoint to the reader
        ret->setSubscriberConnector(connector);
        connector->setEndpoint(ret);
    }

    return ret;
}

PublisherEndpointBase* LocalServiceManager::createAVBPublisherEndpoint(
        QoSGroup qosGroup,
        PublisherConnector* connector) {

    PublisherEndpointBase* ret = nullptr;

    if(qosGroup == QoSGroup::RT){
        // 1. Find the factory object;
        cModuleType * moduleType = cModuleType::get(
                    "soqosmw.endpoints.publisher.realtime.avb.AVBPublisherEndpoint");
        // 2. Create the module;
        AVBPublisherEndpoint* avbEndpoint =
                            dynamic_cast<AVBPublisherEndpoint*>(
                                    moduleType->create("publisherEndpoints", this->getParentModule(), _publisherEndpointCount + 1, _publisherEndpointCount));
        _publisherEndpointCount++;
        // 3. Set up its parameters and gate sizes as needed;
        auto streamID = connector->getPublisherApplicationInformation().getStreamId();
        avbEndpoint->par("streamID").setIntValue(streamID);

        auto srClass = connector->getPublisherApplicationInformation().getSrClass();
        string strSrClass;
        switch(srClass){
        case SR_CLASS::A: strSrClass = "A"; break;
        case SR_CLASS::B: strSrClass = "B"; break;
        default: break;
        }
        avbEndpoint->par("srClass").setStringValue(strSrClass);

        auto intervalFrames = connector->getPublisherApplicationInformation().getIntervalFrames();
        avbEndpoint->par("intervalFrames").setIntValue(intervalFrames);

        auto vlanID = connector->getPublisherApplicationInformation().getIntervalFrames();
        avbEndpoint->par("vlan_id").setIntValue(vlanID);

        auto payload = connector->getPublisherApplicationInformation().getFramesize();
        avbEndpoint->par("payload").setIntValue(payload);

        // cast back.
        ret = dynamic_cast<PublisherEndpointBase*>(avbEndpoint);
        //connect endpoint to the reader
        ret->setPublisherConnector(connector);
        connector->addEndpoint(ret);
    }

    return ret;
}

PublisherEndpointBase* LocalServiceManager::createTCPPublisherEndpoint(
        QoSGroup qosGroup,
        PublisherConnector* connector) {

    PublisherEndpointBase* ret = nullptr;

    if(qosGroup == QoSGroup::STD_TCP){
        // 1. Find the factory object;
        cModuleType * moduleType = cModuleType::get(
                    "soqosmw.endpoints.publisher.standard.tcp.TCPPublisherEndpoint");
        // 2. Create the module;
        TCPPublisherEndpoint* tcpEndpoint =
                            dynamic_cast<TCPPublisherEndpoint*>(
                                    moduleType->create("publisherEndpoints", this->getParentModule(), _publisherEndpointCount + 1, _publisherEndpointCount));
        _publisherEndpointCount++;
        // 3. Set up its parameters and gate sizes as needed;
        string localAddr = connector->getPublisherApplicationInformation().getAddress().str();
        tcpEndpoint->par("localAddress").setStringValue(localAddr);
        int localPort = connector->getPublisherApplicationInformation().getTCPPort();
        tcpEndpoint->par("localPort").setIntValue(localPort);

        // cast back.
        ret = dynamic_cast<PublisherEndpointBase*>(tcpEndpoint);
        //connect endpoint to the reader
        ret->setPublisherConnector(connector);
        connector->addEndpoint(ret);
    }

    return ret;
}

PublisherEndpointBase* LocalServiceManager::createUDPPublisherEndpoint(
        QoSGroup qosGroup,
        PublisherConnector* connector) {

    PublisherEndpointBase* ret = nullptr;

    if(qosGroup == QoSGroup::STD_UDP){
        // 1. Find the factory object;
        cModuleType * moduleType = cModuleType::get(
                    "soqosmw.endpoints.publisher.standard.udp.UDPPublisherEndpoint");
        // 2. Create the module;
        UDPPublisherEndpoint* udpEndpoint =
                            dynamic_cast<UDPPublisherEndpoint*>(
                                    moduleType->create("publisherEndpoints", this->getParentModule(), _publisherEndpointCount + 1, _publisherEndpointCount));
        _publisherEndpointCount++;
        // 3. Set up its parameters and gate sizes as needed;
        string localAddr = connector->getPublisherApplicationInformation().getAddress().str();
        udpEndpoint->par("localAddress").setStringValue(localAddr);
        int localPort = connector->getPublisherApplicationInformation().getUDPPort();
        udpEndpoint->par("localPort").setIntValue(localPort);

        // cast back.
        ret = dynamic_cast<PublisherEndpointBase*>(udpEndpoint);
        //connect endpoint to the reader
        ret->setPublisherConnector(connector);
        connector->addEndpoint(ret);
    }

    return ret;
}

PublisherEndpointBase* LocalServiceManager::createSomeIpTCPPublisherEndpoint(
        QoSGroup qosGroup,
        PublisherConnector* connector) {

    PublisherEndpointBase* ret = nullptr;

    if(qosGroup == QoSGroup::SOMEIP_TCP){
        // 1. Find the factory object;
        cModuleType * moduleType = cModuleType::get(
                    "soqosmw.endpoints.publisher.someip.tcp.SOMEIPTCPPublisherEndpoint");
        // 2. Create the module;
        SOMEIPTCPPublisherEndpoint* someipTcpEndpoint =
                            dynamic_cast<SOMEIPTCPPublisherEndpoint*>(
                                    moduleType->create("publisherEndpoints", this->getParentModule(), _publisherEndpointCount + 1, _publisherEndpointCount));
        _publisherEndpointCount++;
        // 3. Set up its parameters and gate sizes as needed;
        string localAddr = connector->getPublisherApplicationInformation().getAddress().str();
        someipTcpEndpoint->par("localAddress").setStringValue(localAddr);
        int localPort = connector->getPublisherApplicationInformation().getTCPPort();
        someipTcpEndpoint->par("localPort").setIntValue(localPort);

        // cast back.
        ret = dynamic_cast<PublisherEndpointBase*>(someipTcpEndpoint);
        //connect endpoint to the reader
        ret->setPublisherConnector(connector);
        connector->addEndpoint(ret);
    }
    return ret;
}

PublisherEndpointBase* LocalServiceManager::createSomeIpUDPPublisherEndpoint(
        QoSGroup qosGroup,
        PublisherConnector* connector) {

    PublisherEndpointBase* ret = nullptr;

    if(qosGroup == QoSGroup::SOMEIP_UDP){
        // 1. Find the factory object;
        cModuleType * moduleType = cModuleType::get(
                    "soqosmw.endpoints.publisher.someip.udp.SOMEIPUDPPublisherEndpoint");
        // 2. Create the module;
        SOMEIPUDPPublisherEndpoint* someipUdpEndpoint =
                            dynamic_cast<SOMEIPUDPPublisherEndpoint*>(
                                    moduleType->create("publisherEndpoints", this->getParentModule(), _publisherEndpointCount + 1, _publisherEndpointCount));
        _publisherEndpointCount++;
        // 3. Set up its parameters and gate sizes as needed;
        string localAddr = connector->getPublisherApplicationInformation().getAddress().str();
        someipUdpEndpoint->par("localAddress").setStringValue(localAddr);
        int localPort = connector->getPublisherApplicationInformation().getUDPPort();
        someipUdpEndpoint->par("localPort").setIntValue(localPort);

        // cast back.
        ret = dynamic_cast<PublisherEndpointBase*>(someipUdpEndpoint);
        //connect endpoint to the reader
        ret->setPublisherConnector(connector);
        connector->addEndpoint(ret);
    }
    return ret;
}

} /* end namespace  */
