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

#include <base/EndpointDescription.h>
#include <servicemanager/LocalServiceManager.h>
#include <algorithm>
#include <cstring>
#include <iterator>

#include <inet/networklayer/common/L3AddressResolver.h>

#include "soqosmw/endpoints/subscriber/realtime/avb/AVBSubscriberEndpoint.h"
#include "soqosmw/endpoints/subscriber/standard/tcp/TCPSubscriberEndpoint.h"
#include "soqosmw/endpoints/subscriber/standard/udp/UDPSubscriberEndpoint.h"
#include "soqosmw/endpoints/publisher/realtime/avb/AVBPublisherEndpoint.h"
#include "soqosmw/endpoints/publisher/standard/tcp/TCPPublisherEndpoint.h"
#include "soqosmw/endpoints/publisher/standard/udp/UDPPublisherEndpoint.h"

using namespace std;
using namespace inet;
using namespace CoRE4INET;

namespace soqosmw {
#define LSM_INIT_STAGE_1 1
#define LSM_INIT_STAGE 14

Define_Module(LocalServiceManager);

LocalServiceManager::LocalServiceManager() {
    _requestID = 0;
}

LocalServiceManager::~LocalServiceManager() {
}

void LocalServiceManager::initialize(int stage) {

    if(stage == LSM_INIT_STAGE_1) {
    }
    if (stage == LSM_INIT_STAGE) {
        handleParameterChange(nullptr);
        _sd = dynamic_cast<StaticServiceDiscovery*>(getParentModule()->getSubmodule(
                   par("sdmoduleName")));

       _qosnp =
               dynamic_cast<QoSNegotiationProtocol*>(getParentModule()->getSubmodule(
                       par("qosnpmoduleName")));
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

ConnectorBase* LocalServiceManager::registerPublisherService(std::string& publisherPath,
        QoSPolicyMap& qosPolicies,
            SOQoSMWApplicationBase* executingApplication) {
    Enter_Method("LSM:registerPublisherService()");

    // check if the publisher already exists.
    auto iter = _publisherConnectors.find(publisherPath);
    if(iter != _publisherConnectors.end()){
        if(iter->second->addApplication(executingApplication)){
            iter->second->setQos(qosPolicies);
            return iter->second;
        } else {
            throw cRuntimeError("This Publisher Service already exists on this host...");
        }
    }

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

    //save the writer so that new endpoints can be connected to the application.
    _publisherConnectors[publisherPath] = module;

    return module;
}

ConnectorBase* LocalServiceManager::registerSubscriberService(std::string& subscriberPath,
            std::string& publisherPath,
            QoSPolicyMap& qosPolicies,
            SOQoSMWApplicationBase* executingApplication)
{
    Enter_Method("LSM:registerSubscriberService()");

    // check if the subscriber already exists.
    auto iter = _subscriberConnectors.find(publisherPath);
    if(iter != _subscriberConnectors.end()){
        // we allready have a service subscribing to the data
        if(equalQoSMap(qosPolicies, iter->second->getQos())){
            // same qos as well so try to add and return
            if(iter->second->addApplication(executingApplication)){
                return iter->second;
            } else {
                throw cRuntimeError("This Subscriber service already exists on this host...");
            }
        }
    }
    // otherwise we need a new negotiation!

    //check if publisher exists in the network and start the negotiation with a request.
    if (_sd->contains(publisherPath)) {
        //create the request
        EndpointDescription local(subscriberPath, _localAddress,
                _qosnp->getProtocolPort());

        EndpointDescription remote(publisherPath, _sd->discover(publisherPath),
                _qosnp->getProtocolPort());
        Request * request = new Request(_requestID++, local, remote,
                qosPolicies, nullptr);

        //create qos broker for the request
        _qosnp->createQoSBroker(request);

    } else {
        throw cRuntimeError(
                "The publisher you are requesting is unknown and has no entry in the ServiceRegistry.");
    }

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

    // save the reader so that new endpoints can be connected to the application.
    _subscriberConnectors[publisherPath] = module;

    return module;
}

PublisherConnector* LocalServiceManager::getPublisherConnectorForName(
        std::string& publisherPath) {
    auto connector = _publisherConnectors.find(publisherPath);
    if(connector != _publisherConnectors.end()){
        return connector->second;
    }
    return nullptr;
}


SubscriberConnector* LocalServiceManager::getSubscriberConnectorForName(
        std::string& publisherPath) {
    auto connector = _subscriberConnectors.find(publisherPath);
    if(connector != _subscriberConnectors.end()){
        return connector->second;
    }
    return nullptr;
}

PublisherEndpointBase* LocalServiceManager::createOrFindPublisherFor(
        std::string& publisherPath, int qos) {

    PublisherEndpointBase* pub = nullptr;

    pub = findPublisherLike(publisherPath, qos);
    if(!pub){
        PublisherConnector* connector = getPublisherConnectorForName(publisherPath);

        if(connector){
            //create according endpoint
            switch(qos){
            case QoSGroups::RT:
                pub = createAVBPublisherEndpoint(publisherPath, qos, connector);
                break;
            case QoSGroups::STD_TCP:
                pub = createTCPPublisherEndpoint(publisherPath, qos, connector);
                break;
            case QoSGroups::STD_UDP:
                pub = createUDPPublisherEndpoint(publisherPath, qos, connector);
                break;
            case QoSGroups::WEB:
                throw cRuntimeError("The web QoS Group is not yet available");
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
        std::string& publisherPath, ConnectionSpecificInformation* csi) {

    SubscriberEndpointBase* sub = nullptr;
    auto group = getQoSGroupForConnectionType(csi->getConnectionType());

    sub = findSubscriberLike(publisherPath, group);
    if(!sub){
        SubscriberConnector* connector = findSubscriberConnectorLike(publisherPath, group);

        if(connector){
            //create according endpoint
            switch(csi->getConnectionType()){
            case ConnectionType::ct_avb:
                sub = createAVBSubscriberEndpoint(publisherPath, csi, connector);
                break;
            case ConnectionType::ct_tcp:
                sub = createTCPSubscriberEndpoint(publisherPath, csi, connector);
                break;
            case ConnectionType::ct_udp:
                sub = createUDPSubscriberEndpoint(publisherPath, csi, connector);
                break;
            case ConnectionType::ct_http:
                throw cRuntimeError("The HTTP connection is not yet available");
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
    default:
        throw cRuntimeError("Unknown connection type.");
        break;
    }
    //return QoSGroup::RT;
}

PublisherEndpointBase* LocalServiceManager::findPublisherLike(
        std::string& publisherPath, int qos) {

    // find fitting connectors
    PublisherConnector* connector = getPublisherConnectorForName(publisherPath);
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
        std::string& publisherPath, int qos) {

    // find fitting connectors
    SubscriberConnector* connector = getSubscriberConnectorForName(publisherPath);
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
        std::string& publisherPath, int qos) {

    // find fitting connectors
    SubscriberConnector* connector = findSubscriberConnectorLike(publisherPath, qos);
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
        std::string& publisherPath, ConnectionSpecificInformation* csi,
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
        avbEndpoint->par("streamID").setLongValue(csi_avb->getStreamID());
        avbEndpoint->par("vlan_id").setLongValue(csi_avb->getVlanID());

        // cast back.
        ret = dynamic_cast<SubscriberEndpointBase*>(avbEndpoint);
        //connect endpoint to the reader
        ret->setConnector(connector);
        connector->addEndpoint(ret);
    }

    return ret;

}

SubscriberEndpointBase* LocalServiceManager::createTCPSubscriberEndpoint(
        std::string& publisherPath, ConnectionSpecificInformation* csi,
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
        tcpEndpoint->par("localPort").setLongValue(localPort);
        tcpEndpoint->par("remoteAddress").setStringValue(csi_tcp->getAddress());
        tcpEndpoint->par("remotePort").setLongValue(csi_tcp->getPort());

        // cast back.
        ret = dynamic_cast<SubscriberEndpointBase*>(tcpEndpoint);
        //connect endpoint to the reader
        ret->setConnector(connector);
        connector->addEndpoint(ret);
    }

    return ret;
}

SubscriberEndpointBase* LocalServiceManager::createUDPSubscriberEndpoint(
        std::string& publisherPath, ConnectionSpecificInformation* csi,
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
        udpEndpoint->par("localPort").setLongValue(localPort);

        // cast back.
        ret = dynamic_cast<SubscriberEndpointBase*>(udpEndpoint);
        //connect endpoint to the reader
        ret->setConnector(connector);
        connector->addEndpoint(ret);
    }

    return ret;
}

PublisherEndpointBase* LocalServiceManager::createAVBPublisherEndpoint(
        std::string& publisherPath, int qos,
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
        avbEndpoint->par("streamID").setLongValue(streamID);

        auto srClass = (dynamic_cast<SRClassQoSPolicy*>(connector->getQos()[QoSPolicyNames::SRClass]))->getValue();
        string strSrClass;
        switch(srClass){
        case SR_CLASS::A: strSrClass = "A"; break;
        case SR_CLASS::B: strSrClass = "B"; break;
        default: break;
        }
        avbEndpoint->par("srClass").setStringValue(strSrClass);

        auto intervalFrames = (dynamic_cast<IntervalFramesQoSPolicy*>(connector->getQos()[QoSPolicyNames::IntervalFrames]))->getValue();
        avbEndpoint->par("intervalFrames").setLongValue(intervalFrames);

        auto vlanID = (dynamic_cast<IntervalFramesQoSPolicy*>(connector->getQos()[QoSPolicyNames::IntervalFrames]))->getValue();
        avbEndpoint->par("vlan_id").setLongValue(vlanID);

        auto payload = (dynamic_cast<FramesizeQoSPolicy*>(connector->getQos()[QoSPolicyNames::Framesize]))->getValue();
        avbEndpoint->par("payload").setLongValue(payload);

        // cast back.
        ret = dynamic_cast<PublisherEndpointBase*>(avbEndpoint);
        //connect endpoint to the reader
        ret->setConnector(connector);
        connector->addEndpoint(ret);
    }

    return ret;
}

PublisherEndpointBase* LocalServiceManager::createTCPPublisherEndpoint(
        std::string& publisherPath, int qos,
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
        tcpEndpoint->par("localPort").setLongValue(localPort);

        // cast back.
        ret = dynamic_cast<PublisherEndpointBase*>(tcpEndpoint);
        //connect endpoint to the reader
        ret->setConnector(connector);
        connector->addEndpoint(ret);
    }

    return ret;
}

PublisherEndpointBase* LocalServiceManager::createUDPPublisherEndpoint(
        std::string& publisherPath, int qos,
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
        udpEndpoint->par("localPort").setLongValue(localPort);

        // cast back.
        ret = dynamic_cast<PublisherEndpointBase*>(udpEndpoint);
        //connect endpoint to the reader
        ret->setConnector(connector);
        connector->addEndpoint(ret);
    }

    return ret;
}

} /* end namespace  */
