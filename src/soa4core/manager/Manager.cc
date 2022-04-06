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

#include "soa4core/manager/Manager.h"
#include "soa4core/endpoints/subscriber/realtime/avb/AVBSubscriberEndpoint.h"
#include "soa4core/endpoints/subscriber/standard/tcp/TCPSubscriberEndpoint.h"
#include "soa4core/endpoints/subscriber/standard/udp/UDPSubscriberEndpoint.h"
#include "soa4core/endpoints/subscriber/someip/tcp/SOMEIPTCPSubscriberEndpoint.h"
#include "soa4core/endpoints/subscriber/someip/udp/SOMEIPUDPSubscriberEndpoint.h"
#include "soa4core/endpoints/publisher/realtime/avb/AVBPublisherEndpoint.h"
#include "soa4core/endpoints/publisher/standard/tcp/TCPPublisherEndpoint.h"
#include "soa4core/endpoints/publisher/standard/udp/UDPPublisherEndpoint.h"
#include "soa4core/endpoints/publisher/someip/tcp/SOMEIPTCPPublisherEndpoint.h"
#include "soa4core/endpoints/publisher/someip/udp/SOMEIPUDPPublisherEndpoint.h"
#include "soa4core/applications/publisher/base/Publisher.h"
//INET
#include <inet/networklayer/common/L3AddressResolver.h>
//STD
#include <algorithm>
#include <cstring>
#include <iterator>
#include <stdio.h>


using namespace std;
using namespace inet;
using namespace CoRE4INET;

namespace SOA4CoRE {

Define_Module(Manager);

Manager::Manager() {
    _requestID = 0;
}

Manager::~Manager() {
}

void Manager::initialize(int stage) {

    if (stage == INITSTAGE_APPLICATION_LAYER) {
        handleParameterChange(nullptr);
        _lsr =
               dynamic_cast<Registry*>(getParentModule()->getSubmodule(
                       par("srmoduleName")));
    }
}

void Manager::handleMessage(cMessage *msg) {
    delete msg;
}

void Manager::handleParameterChange(const char* parname) {

    //read UDP Common Parameters
    if (!parname || !strcmp(parname, "localAddress")) {
        const char* localAddr = par("localAddress");
        _localAddress = L3AddressResolver().resolve(localAddr);
    }

}

PublisherConnector* Manager::registerPublisherService(ServiceBase* publisherApplication) {
    Enter_Method("LSM:registerPublisherService()");

    Publisher* publisherApplication_ = nullptr;
    if(!(publisherApplication_ = dynamic_cast<Publisher*>(publisherApplication))){
        throw cRuntimeError("Given publisher application must be of type Publisher");
    }
    if (_lsr->getPublisherConnector(publisherApplication_->getServiceId())) {
        throw cRuntimeError("There can not be multiple publisher services with same ID");
    }

    PublisherConnector* publisherConnector = createPublisherConnector(publisherApplication_);
    _lsr->addPublisherServiceConnector(publisherApplication_->getServiceId(), publisherConnector);
    return publisherConnector;
}

PublisherConnector* Manager::createPublisherConnector(Publisher* publisherApplication) {
    // create a connector for the publisher
    // 1. Find the factory object;
    cModuleType *moduleType = cModuleType::get("soa4core.connector.publisher.PublisherConnector");
    // 2. Create the module;
    int vectorsize = _lsr->getPublisherConnectorsCount();
    PublisherConnector *module = dynamic_cast<PublisherConnector*> (moduleType->create("publisherConnectors", this->getParentModule(), vectorsize + 1, vectorsize));
    // 3. Set up its parameters and gate sizes as needed;
    module->setApplication(publisherApplication);
    module->finalizeParameters();
    // 4. Tell the (possibly compound) module to recursively create its internal submodules and connections;
    module->buildInside();
    module->callInitialize();
    // 5. Schedule activation message(s) for the new simple module(s).
    module->scheduleStart(simTime());
    return module;
}

SubscriberConnector* Manager::registerSubscriberServiceAndSubscribeService(ServiceIdentifier publisherServiceIdentifier, ServiceBase* subscriberApplication)
{
    Enter_Method("LSM:registerSubscriberService()");
    Subscriber* subscriberApplication_ = nullptr;
    SubscriberConnector* validSubscriberConnector = nullptr;
    if(!(subscriberApplication_ = dynamic_cast<Subscriber*>(subscriberApplication))){
        throw cRuntimeError("Given subscriber application must be of type Subscriber");
    }
    std::list<SubscriberConnector*> subscriberConnectors = _lsr->getSubscriberConnectors(publisherServiceIdentifier.getServiceId());
    for (SubscriberConnector* subscriberConnector : subscriberConnectors) {
        std::vector<ServiceBase*> subscriberApplications = subscriberConnector->getApplications();
        Subscriber* connectedSubscriberApplication = nullptr;
        if (subscriberApplications.empty()) {
            throw cRuntimeError("Empty subscriber connector without applications!");
        }
        if (!(connectedSubscriberApplication = dynamic_cast<Subscriber*>(subscriberApplications[0]))) {
            throw cRuntimeError("The subscriber application must be of type Subscriber");
        }
        if (connectedSubscriberApplication->getQoSGroup() == subscriberApplication_->getQoSGroup()) {
            validSubscriberConnector = subscriberConnector;
            break;
        }
    }

    if (!validSubscriberConnector) {
        validSubscriberConnector = createSubscriberConnector(subscriberApplication_);
        _lsr->addSubscriberServiceConnector(publisherServiceIdentifier.getServiceId(), validSubscriberConnector);
        discoverService(publisherServiceIdentifier, subscriberApplication_);
    } else {
        addSubscriberServiceToConnector(validSubscriberConnector, subscriberApplication_);
    }
    return validSubscriberConnector;
}

void Manager::discoverService(ServiceIdentifier publisherServiceIdentifier, ServiceBase* subscriberApplication) {
    throw cRuntimeError("Manager has no implementation of discoverService. \n"
            "Use other Manager (e.g. QoSManager, SomeIpManager which implements discoverService");
}

void Manager::addSubscriberServiceToConnector(SubscriberConnector* subscriberConnector, ServiceBase* subscriberApplication) {
    if(!(subscriberConnector->addApplication(subscriberApplication))){
        throw cRuntimeError("This Subscriber service already exists on this host...");
    }
}

SubscriberConnector* Manager::createSubscriberConnector(ServiceBase* subscriberApplication) {
    // create a connector for the subscriber
    // 1. Find the factory object;
    cModuleType *moduleType = cModuleType::get("soa4core.connector.subscriber.SubscriberConnector");
    // 2. Create the module;
    int vectorsize  = _lsr->getSubscriberConnectorsCount();
    SubscriberConnector *module = dynamic_cast<SubscriberConnector*> (moduleType->create("subscriberConnectors", this->getParentModule(), vectorsize + 1, vectorsize));
    // 3. Set up its parameters and gate sizes as needed;
    module->setAddress(subscriberApplication->getAddress());
    module->setTcpPort(subscriberApplication->getTcpPort());
    module->setUdpPort(subscriberApplication->getUdpPort());
    module->addApplication(subscriberApplication);
    module->finalizeParameters();
    // 4. Tell the (possibly compound) module to recursively create its internal submodules and connections;
    module->buildInside();
    module->callInitialize();
    // 5. Schedule activation message(s) for the new simple module(s).
    module->scheduleStart(simTime());
    return module;
}

PublisherEndpointBase* Manager::createOrFindPublisherEndpoint(
        uint32_t publisherServiceId, QoSGroup qosGroup) {

    PublisherEndpointBase* pub = nullptr;

    pub = findPublisherEndpoint(publisherServiceId, qosGroup);
    if(!pub){
        PublisherConnector* publisherConnector = _lsr->getPublisherConnector(publisherServiceId);
        if(publisherConnector){
            //create according endpoint
            switch(qosGroup){
            case QoSGroup::RT:
                pub = createAVBPublisherEndpoint(qosGroup, publisherConnector);
                break;
            case QoSGroup::STD_TCP:
                pub = createTCPPublisherEndpoint(qosGroup, publisherConnector);
                break;
            case QoSGroup::STD_UDP:
                pub = createUDPPublisherEndpoint(qosGroup, publisherConnector);
                break;
            case QoSGroup::WEB:
                throw cRuntimeError("The web QoS Group is not yet available");
                break;
            case QoSGroup::SOMEIP_TCP:
                pub = createSomeIpTCPPublisherEndpoint(qosGroup, publisherConnector);
                break;
            case QoSGroup::SOMEIP_UDP:
                pub = createSomeIpUDPPublisherEndpoint(qosGroup, publisherConnector);
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

SubscriberEndpointBase* Manager::createOrFindSubscriberEndpoint(
        uint32_t publisherServiceId, ConnectionSpecificInformation* csi) {

    SubscriberEndpointBase* sub = nullptr;
    QoSGroup qosGroup = getQoSGroupForConnectionType(ConnectionType(csi->getConnectionType()));
    sub = findSubscriberEndpoint(publisherServiceId, qosGroup);
    if(!sub){
        SubscriberConnector* subscriberConnector = nullptr;
        std::list<SubscriberConnector*> subscriberConnectors = _lsr->getSubscriberConnectors(publisherServiceId);
        for (SubscriberConnector* subscriberConnector_ : subscriberConnectors) {
            std::vector<ServiceBase*> subscriberApplications = subscriberConnector_->getApplications();
            if (subscriberApplications.empty()) {
                throw cRuntimeError("Found empty subscriber connector without subscriber applications.");
            }
            Subscriber* subscriberApplication = nullptr;
            if (!(subscriberApplication = dynamic_cast<Subscriber*>(subscriberApplications[0]))) {
                throw cRuntimeError("The subscriber application must be of the type Subscriber.");
            }

            if (subscriberApplication->getQoSGroup() == qosGroup) {
                subscriberConnector = subscriberConnector_;
                break;
            }
        }

        if(subscriberConnector){
            //create according endpoint
            switch(csi->getConnectionType()){
            case ConnectionType::ct_avb:
                sub = createAVBSubscriberEndpoint(csi, subscriberConnector);
                break;
            case ConnectionType::ct_tcp:
                sub = createTCPSubscriberEndpoint(csi, subscriberConnector);
                break;
            case ConnectionType::ct_udp:
                sub = createUDPSubscriberEndpoint(csi, subscriberConnector);
                break;
            case ConnectionType::ct_http:
                throw cRuntimeError("The HTTP connection is not yet available");
                break;
            case ConnectionType::ct_someip_tcp:
                sub = createSomeIpTCPSubscriberEndpoint(csi, subscriberConnector);
                break;
            case ConnectionType::ct_someip_udp:
                sub = createSomeIpUDPSubscriberEndpoint(csi, subscriberConnector);
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

QoSGroup Manager::getQoSGroupForConnectionType(ConnectionType connectionType){
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

PublisherEndpointBase* Manager::findPublisherEndpoint(
        uint32_t publisherServiceId, QoSGroup qosGroup) {

    // find fitting publisher connector
    PublisherConnector* publisherConnector = _lsr->getPublisherConnector(publisherServiceId);
    PublisherEndpointBase* publisherEndpointBase = nullptr;
    if(publisherConnector){
        for (auto endpoint: publisherConnector->getEndpoints()){
            if(endpoint->getQos() == qosGroup){
                publisherEndpointBase = dynamic_cast<PublisherEndpointBase*>(endpoint);
                break;
            }
        }
    }

    return publisherEndpointBase;
}

SubscriberEndpointBase* Manager::findSubscriberEndpoint(
        uint32_t publisherServiceId, QoSGroup qosGroup) {

    SubscriberEndpointBase* subscriberEndpoint = nullptr;
    // find fitting subscriber connector
    std::list<SubscriberConnector*> subscriberConnectors = _lsr->getSubscriberConnectors(publisherServiceId);
    for (SubscriberConnector* subscriberConnector : subscriberConnectors) {
        std::vector<ServiceBase*> subscriberApplications = subscriberConnector->getApplications();
        if (subscriberApplications.empty()) {
            throw cRuntimeError("Found empty subscriber connector without subscriber applications.");
        }
        Subscriber* subscriberApplication = nullptr;
        if (!(subscriberApplication = dynamic_cast<Subscriber*>(subscriberApplications[0]))) {
            throw cRuntimeError("The subscriber application must be of the type Subscriber.");
        }

        if (subscriberApplication->getQoSGroup() == qosGroup && (subscriberEndpoint = subscriberConnector->getEndpoint())) {
            break;
        }
    }
    return subscriberEndpoint;
}

SubscriberEndpointBase* Manager::createAVBSubscriberEndpoint(
        ConnectionSpecificInformation* csi,
        SubscriberConnector* subscriberConnector) {
    SubscriberEndpointBase* ret = nullptr;

    CSI_AVB* csi_avb = dynamic_cast<CSI_AVB*>(csi);

    if(csi_avb){
        // 1. Find the factory object;
        cModuleType * moduleType = cModuleType::get(
                    "soa4core.endpoints.subscriber.realtime.avb.AVBSubscriberEndpoint");
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
        ret->setSubscriberConnector(subscriberConnector);
        subscriberConnector->setEndpoint(ret);
    }

    return ret;

}

SubscriberEndpointBase* Manager::createTCPSubscriberEndpoint(
        ConnectionSpecificInformation* csi,
        SubscriberConnector* subscriberConnector) {
    SubscriberEndpointBase* ret = nullptr;

    CSI_TCP* csi_tcp = dynamic_cast<CSI_TCP*>(csi);

    if(csi_tcp){
        // 1. Find the factory object;
        cModuleType * moduleType = cModuleType::get(
                    "soa4core.endpoints.subscriber.standard.tcp.TCPSubscriberEndpoint");
        // 2. Create the module;
        TCPSubscriberEndpoint* tcpEndpoint =
                            dynamic_cast<TCPSubscriberEndpoint*>(
                                    moduleType->create("subscriberEndpoints", this->getParentModule(), _subscriberEndpointCount + 1, _subscriberEndpointCount));
        _subscriberEndpointCount++;
        // 3. Set up its parameters and gate sizes as needed;
        string localAddr = subscriberConnector->getAddress().str();
        tcpEndpoint->par("localAddress").setStringValue(localAddr);
        int localPort = subscriberConnector->getTcpPort();
        tcpEndpoint->par("localPort").setIntValue(localPort);
        tcpEndpoint->par("remoteAddress").setStringValue(csi_tcp->getAddress());
        tcpEndpoint->par("remotePort").setIntValue(csi_tcp->getPort());

        // cast back.
        ret = dynamic_cast<SubscriberEndpointBase*>(tcpEndpoint);
        //connect endpoint to the reader
        ret->setSubscriberConnector(subscriberConnector);
        subscriberConnector->setEndpoint(ret);
    }

    return ret;
}

SubscriberEndpointBase* Manager::createUDPSubscriberEndpoint(
        ConnectionSpecificInformation* csi,
        SubscriberConnector* subscriberConnector) {

    SubscriberEndpointBase* ret = nullptr;

    CSI_UDP* csi_udp = dynamic_cast<CSI_UDP*>(csi);

    if(csi_udp){
        // 1. Find the factory object;
        cModuleType * moduleType = cModuleType::get(
                    "soa4core.endpoints.subscriber.standard.udp.UDPSubscriberEndpoint");
        // 2. Create the module;
        UDPSubscriberEndpoint* udpEndpoint =
                            dynamic_cast<UDPSubscriberEndpoint*>(
                                    moduleType->create("subscriberEndpoints", this->getParentModule(), _subscriberEndpointCount + 1, _subscriberEndpointCount));
        _subscriberEndpointCount++;
        // 3. Set up its parameters and gate sizes as needed;
        string localAddr = subscriberConnector->getAddress().str();
        udpEndpoint->par("localAddress").setStringValue(localAddr);
        int localPort = subscriberConnector->getUdpPort();
        udpEndpoint->par("localPort").setIntValue(localPort);

        // cast back.
        ret = dynamic_cast<SubscriberEndpointBase*>(udpEndpoint);
        //connect endpoint to the reader
        ret->setSubscriberConnector(subscriberConnector);
        subscriberConnector->setEndpoint(ret);
    }

    return ret;
}


SubscriberEndpointBase* Manager::createSomeIpTCPSubscriberEndpoint(
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

SubscriberEndpointBase* Manager::createSomeIpUDPSubscriberEndpoint(
        ConnectionSpecificInformation* csi,
        SubscriberConnector* subscriberConnector) {

    SubscriberEndpointBase* ret = nullptr;

    CSI_SOMEIP_UDP* csi_someip = dynamic_cast<CSI_SOMEIP_UDP*>(csi);

    if(csi_someip){
        // 1. Find the factory object;
        cModuleType * moduleType = cModuleType::get(
                    "soa4core.endpoints.subscriber.someip.udp.SOMEIPUDPSubscriberEndpoint");
        // 2. Create the module;
        SOMEIPUDPSubscriberEndpoint* someipUdpEndpoint =
                            dynamic_cast<SOMEIPUDPSubscriberEndpoint*>(
                                    moduleType->create("subscriberEndpoints", this->getParentModule(), _subscriberEndpointCount + 1, _subscriberEndpointCount));
        _subscriberEndpointCount++;
        // 3. Set up its parameters and gate sizes as needed;
        string localAddr = subscriberConnector->getAddress().str();
        someipUdpEndpoint->par("localAddress").setStringValue(localAddr);
        int localPort = subscriberConnector->getUdpPort();
        someipUdpEndpoint->par("localPort").setIntValue(localPort);

        // cast back.
        ret = dynamic_cast<SubscriberEndpointBase*>(someipUdpEndpoint);
        //connect endpoint to the reader
        ret->setSubscriberConnector(subscriberConnector);
        subscriberConnector->setEndpoint(ret);
    }

    return ret;
}

PublisherEndpointBase* Manager::createAVBPublisherEndpoint(
        QoSGroup qosGroup,
        PublisherConnector* publisherConnector) {

    PublisherEndpointBase* ret = nullptr;

    if(qosGroup == QoSGroup::RT){
        // 1. Find the factory object;
        cModuleType * moduleType = cModuleType::get(
                    "soa4core.endpoints.publisher.realtime.avb.AVBPublisherEndpoint");
        // 2. Create the module;
        AVBPublisherEndpoint* avbEndpoint =
                            dynamic_cast<AVBPublisherEndpoint*>(
                                    moduleType->create("publisherEndpoints", this->getParentModule(), _publisherEndpointCount + 1, _publisherEndpointCount));
        _publisherEndpointCount++;
        // 3. Set up its parameters and gate sizes as needed;
        Publisher* publisherApplication = nullptr;
        if (!(publisherApplication = dynamic_cast<Publisher*>(publisherConnector->getApplication()))) {
            throw cRuntimeError("The publisher application is expected to be of the type Publisher.");
        }
        auto streamID = publisherApplication->getStreamId();
        avbEndpoint->par("streamID").setIntValue(streamID);

        auto srClass = publisherApplication->getSrClass();
        string strSrClass;
        switch(srClass){
        case SR_CLASS::A: strSrClass = "A"; break;
        case SR_CLASS::B: strSrClass = "B"; break;
        default: break;
        }
        avbEndpoint->par("srClass").setStringValue(strSrClass);

        auto intervalFrames = publisherApplication->getIntervalFrames();
        avbEndpoint->par("intervalFrames").setIntValue(intervalFrames);

        auto vlanID = publisherApplication->getIntervalFrames();
        avbEndpoint->par("vlan_id").setIntValue(vlanID);

        auto payload = publisherApplication->getFramesize();
        avbEndpoint->par("payload").setIntValue(payload);

        // cast back.
        ret = dynamic_cast<PublisherEndpointBase*>(avbEndpoint);
        //connect endpoint to the reader
        ret->setPublisherConnector(publisherConnector);
        publisherConnector->addEndpoint(ret);
    }

    return ret;
}

PublisherEndpointBase* Manager::createTCPPublisherEndpoint(
        QoSGroup qosGroup,
        PublisherConnector* publisherConnector) {

    PublisherEndpointBase* ret = nullptr;

    if(qosGroup == QoSGroup::STD_TCP){
        // 1. Find the factory object;
        cModuleType * moduleType = cModuleType::get(
                    "soa4core.endpoints.publisher.standard.tcp.TCPPublisherEndpoint");
        // 2. Create the module;
        TCPPublisherEndpoint* tcpEndpoint =
                            dynamic_cast<TCPPublisherEndpoint*>(
                                    moduleType->create("publisherEndpoints", this->getParentModule(), _publisherEndpointCount + 1, _publisherEndpointCount));
        _publisherEndpointCount++;
        Publisher* publisherApplication = nullptr;
        if (!(publisherApplication = dynamic_cast<Publisher*>(publisherConnector->getApplication()))) {
            throw cRuntimeError("The publisher application is expected to be of the type Publisher.");
        }
        // 3. Set up its parameters and gate sizes as needed;
        string localAddr = publisherApplication->getAddress().str();
        tcpEndpoint->par("localAddress").setStringValue(localAddr);
        int localPort = publisherApplication->getTcpPort();
        tcpEndpoint->par("localPort").setIntValue(localPort);

        // cast back.
        ret = dynamic_cast<PublisherEndpointBase*>(tcpEndpoint);
        //connect endpoint to the reader
        ret->setPublisherConnector(publisherConnector);
        publisherConnector->addEndpoint(ret);
    }

    return ret;
}

PublisherEndpointBase* Manager::createUDPPublisherEndpoint(
        QoSGroup qosGroup,
        PublisherConnector* publisherConnector) {

    PublisherEndpointBase* ret = nullptr;

    if(qosGroup == QoSGroup::STD_UDP){
        // 1. Find the factory object;
        cModuleType * moduleType = cModuleType::get(
                    "soa4core.endpoints.publisher.standard.udp.UDPPublisherEndpoint");
        // 2. Create the module;
        UDPPublisherEndpoint* udpEndpoint =
                            dynamic_cast<UDPPublisherEndpoint*>(
                                    moduleType->create("publisherEndpoints", this->getParentModule(), _publisherEndpointCount + 1, _publisherEndpointCount));
        _publisherEndpointCount++;
        Publisher* publisherApplication = nullptr;
        if (!(publisherApplication = dynamic_cast<Publisher*>(publisherConnector->getApplication()))) {
            throw cRuntimeError("The publisher application is expected to be of the type Publisher.");
        }
        // 3. Set up its parameters and gate sizes as needed;
        string localAddr = publisherApplication->getAddress().str();
        udpEndpoint->par("localAddress").setStringValue(localAddr);
        int localPort = publisherApplication->getUdpPort();
        udpEndpoint->par("localPort").setIntValue(localPort);

        // cast back.
        ret = dynamic_cast<PublisherEndpointBase*>(udpEndpoint);
        //connect endpoint to the reader
        ret->setPublisherConnector(publisherConnector);
        publisherConnector->addEndpoint(ret);
    }

    return ret;
}

PublisherEndpointBase* Manager::createSomeIpTCPPublisherEndpoint(
        QoSGroup qosGroup,
        PublisherConnector* publisherConnector) {

    PublisherEndpointBase* ret = nullptr;

    if(qosGroup == QoSGroup::SOMEIP_TCP){
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
        ret = dynamic_cast<PublisherEndpointBase*>(someipTcpEndpoint);
        //connect endpoint to the reader
        ret->setPublisherConnector(publisherConnector);
        publisherConnector->addEndpoint(ret);
    }
    return ret;
}

PublisherEndpointBase* Manager::createSomeIpUDPPublisherEndpoint(
        QoSGroup qosGroup,
        PublisherConnector* publisherConnector) {

    PublisherEndpointBase* ret = nullptr;

    if(qosGroup == QoSGroup::SOMEIP_UDP){
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
        ret = dynamic_cast<PublisherEndpointBase*>(someipUdpEndpoint);
        //connect endpoint to the reader
        ret->setPublisherConnector(publisherConnector);
        publisherConnector->addEndpoint(ret);
    }
    return ret;
}

} /* end namespace  */
