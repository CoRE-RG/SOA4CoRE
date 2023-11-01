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

#include <soa4core/endpoints/publisher/ip/tcp/TCPPublisherEndpoint.h>
#include <soa4core/endpoints/publisher/ip/udp/UDPMcastPublisherEndpoint.h>
#include <soa4core/endpoints/publisher/ip/udp/UDPPublisherEndpoint.h>
#include <soa4core/endpoints/subscriber/ip/tcp/TCPSubscriberEndpoint.h>
#include <soa4core/endpoints/subscriber/ip/udp/UDPMcastSubscriberEndpoint.h>
#include <soa4core/endpoints/subscriber/ip/udp/UDPSubscriberEndpoint.h>
#include "soa4core/manager/qos/QoSManager.h"
#include "soa4core/discovery/SomeIpDiscoveryNotification.h"
#include "soa4core/discovery/someip/SomeIpSD.h" // TODO REMOVE!
#include "soa4core/endpoints/subscriber/realtime/avb/AVBSubscriberEndpoint.h"
#include "soa4core/endpoints/publisher/realtime/avb/AVBPublisherEndpoint.h"
#include <algorithm>
//INET
#include <inet/networklayer/common/L3Address.h>

using namespace std;
using namespace inet;
using namespace CoRE4INET;

namespace SOA4CoRE {

Define_Module(QoSManager);

void QoSManager::initialize(int stage)
{
    Manager::initialize(stage);
    if (stage == inet::INITSTAGE_APPLICATION_LAYER) {
        _requestID = 0;
        if(!(_qosnp =
               dynamic_cast<QoSNegotiationProtocol*>(getParentModule()->getSubmodule(
                       par("qosnpmoduleName"))))) {
            throw cRuntimeError("No QoSNegotiationProtocol module found.");
        }
        if (!(_sd = dynamic_cast<IServiceDiscovery*>(getParentModule()->getSubmodule(par("sdmoduleName"))))) {
            throw cRuntimeError("No IServiceDiscovery found.");
        }
        if (cSimpleModule* sd = dynamic_cast<cSimpleModule*>(_sd)) {
            sd->subscribe("serviceOfferSignal",this);
        } else {
            throw cRuntimeError("Service discovery serviceOfferSignal could not be subscribed.");
        }

        if (SomeIpSD* someIpSD = dynamic_cast<SomeIpSD*>(_sd)) {
            someIpSD->subscribe("serviceFindSignal", this);
        }
        _findResultSignal = omnetpp::cComponent::registerSignal("findResultSignal");
    }
}

void QoSManager::handleMessage(cMessage *msg) {
    // Nothing to do
}

void QoSManager::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj, cObject *details) {
    if (!strcmp(getSignalName(signalID),"serviceOfferSignal")) {
        subscribeOfferedService(obj);
    } else if (!strcmp(getSignalName(signalID),"serviceFindSignal")) {
        lookForService(obj);
    } else {
        throw cRuntimeError("Unknown signal.");
    }
}

void QoSManager::discoverService(ServiceIdentifier publisherServiceIdentifier, ServiceBase* subscriberApplication) {
    Subscriber* subscriberApplication_ = nullptr;
    if (!(subscriberApplication_ = dynamic_cast<Subscriber*>(subscriberApplication))) {
        throw cRuntimeError("Subscriber application must be of type Subscriber.");
    }
    _pendingRequests.push_back(std::make_pair(publisherServiceIdentifier.getServiceId(), subscriberApplication_->getQoSGroup()));
    _sd->discover(publisherServiceIdentifier);
}

SubscriberEndpointBase* QoSManager::createConnectionSpecificSubscriberEndpoint(ConnectionSpecificInformation* csi,
        SubscriberConnector* subscriberConnector) {
    SubscriberEndpointBase* sub = nullptr;
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
    case ConnectionType::ct_udp_mcast:
        sub = createUDPMcastSubscriberEndpoint(csi, subscriberConnector);
        break;
    case ConnectionType::ct_http:
        throw cRuntimeError("The HTTP connection is not yet available");
        break;
    default:
        throw cRuntimeError("Unknown connection type for subscriber.");
        break;
    }
    return sub;
}

PublisherEndpointBase* QoSManager::createQoSSpecificPublisherEndpoint(QoSGroup qosGroup, PublisherConnector* publisherConnector) {
    PublisherEndpointBase* pub = nullptr;
    //create according endpoint
    switch(qosGroup){
    case QoSGroup::RT:
        pub = createAVBPublisherEndpoint(qosGroup, publisherConnector);
        break;
    case QoSGroup::IP_TCP:
        pub = createTCPPublisherEndpoint(qosGroup, publisherConnector);
        break;
    case QoSGroup::IP_UDP:
        pub = createUDPPublisherEndpoint(qosGroup, publisherConnector);
        break;
    case QoSGroup::IP_UDP_MCAST:
        pub = createUDPMcastPublisherEndpoint(qosGroup, publisherConnector);
        break;
    case QoSGroup::WEB:
        throw cRuntimeError("The web QoS Group is not yet available");
        break;
    default:
        throw cRuntimeError("Unknown connection type for publisher.");
        break;
    }
    return pub;
}

QoSGroup QoSManager::getQoSGroupForConnectionType(ConnectionType connectionType){
    switch(connectionType){
    case ConnectionType::ct_avb:
        return QoSGroup::RT;
        break;
    case ConnectionType::ct_tcp:
        return QoSGroup::IP_TCP;
        break;
    case ConnectionType::ct_udp:
        return QoSGroup::IP_UDP;
        break;
    case ConnectionType::ct_udp_mcast:
        return QoSGroup::IP_UDP_MCAST;
        break;
    case ConnectionType::ct_http:
        return QoSGroup::WEB;
        break;
    default:
        throw cRuntimeError("Unknown connection type.");
        break;
    }
}

Request* QoSManager::createNegotiationRequest(Registry::ServiceId serviceId, inet::L3Address publisherAddress, QoSGroup qosGroup) {
    EndpointDescription subscriber(serviceId, _localAddress, _qosnp->getProtocolPort());
    EndpointDescription publisher(serviceId, publisherAddress, _qosnp->getProtocolPort());
    Request *request = new Request(_requestID++, subscriber, publisher, qosGroup, nullptr);
    return request;
}

// Subscriber-side
void QoSManager::subscribeOfferedService(cObject* obj) {
    DiscoveryNotification* discoveryNotification = nullptr;
    if (!(discoveryNotification = dynamic_cast<DiscoveryNotification*>(obj))) {
        throw cRuntimeError("Given object is not of type DiscoveryNotification.");
    }
    std::list<std::pair<Registry::ServiceId, QoSGroup>> pendingRequestsToBeRemoved;
    Registry::ServiceId offeredServiceId = discoveryNotification->getServiceId();
    inet::L3Address discoveredAddress = discoveryNotification->getAddress();
    for (std::pair<Registry::ServiceId, QoSGroup> pendingRequest : _pendingRequests) {
        Registry::ServiceId requestedServiceId = pendingRequest.first;
        QoSGroup requestedQosGroup = pendingRequest.second;
        if (offeredServiceId == requestedServiceId) {
            Request* request = createNegotiationRequest(offeredServiceId, discoveredAddress, requestedQosGroup);
            //create qos broker for the request
            _qosnp->createQoSBroker(request);
            pendingRequestsToBeRemoved.push_back(std::make_pair(requestedServiceId, requestedQosGroup));
        }
    }
    for (std::pair<Registry::ServiceId, QoSGroup> pendingRequestToBeRemoved : pendingRequestsToBeRemoved) {
        _pendingRequests.remove(pendingRequestToBeRemoved);
    }
    delete obj;
}

// Publisher-side
void QoSManager::lookForService(cObject* obj) {
    DiscoveryNotification* discoveryNotification = nullptr;
    if (!(discoveryNotification = dynamic_cast<DiscoveryNotification*>(obj))) {
        throw cRuntimeError("Notification must be of type DiscoveryNotification");
    }
    if (PublisherConnector* publisherConnector = _lsr->getPublisherConnector(discoveryNotification->getServiceId())) {
        if(Publisher* publisher = dynamic_cast<Publisher*>(publisherConnector->getApplication())) {
            SomeIpDiscoveryNotification* someIpDiscoveryNotification = new SomeIpDiscoveryNotification(publisher->getServiceId(),
                    discoveryNotification->getAddress(), publisher->getInstanceId(), publisher->getQoSGroups(), QoSGroup::UNDEFINED,
                    publisher->getTcpPort(), publisher->getUdpPort()
            );
            emit(_findResultSignal,someIpDiscoveryNotification);
        }
    }
    delete(discoveryNotification);
}

SubscriberEndpointBase* QoSManager::createAVBSubscriberEndpoint(
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

SubscriberEndpointBase* QoSManager::createTCPSubscriberEndpoint(
        ConnectionSpecificInformation* csi,
        SubscriberConnector* subscriberConnector) {
    SubscriberEndpointBase* ret = nullptr;

    CSI_TCP* csi_tcp = dynamic_cast<CSI_TCP*>(csi);

    if(csi_tcp){
        // 1. Find the factory object;
        cModuleType * moduleType = cModuleType::get(
                    "soa4core.endpoints.subscriber.ip.tcp.TCPSubscriberEndpoint");
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

SubscriberEndpointBase* QoSManager::createUDPSubscriberEndpoint(
        ConnectionSpecificInformation* csi,
        SubscriberConnector* subscriberConnector) {

    SubscriberEndpointBase* ret = nullptr;

    CSI_UDP* csi_udp = dynamic_cast<CSI_UDP*>(csi);

    if(csi_udp){
        // 1. Find the factory object;
        cModuleType * moduleType = cModuleType::get(
                    "soa4core.endpoints.subscriber.ip.udp.UDPSubscriberEndpoint");
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

SubscriberEndpointBase* QoSManager::createUDPMcastSubscriberEndpoint(
        ConnectionSpecificInformation* csi,
        SubscriberConnector* subscriberConnector) {

    SubscriberEndpointBase* ret = nullptr;

    CSI_UDP_MCAST* csi_udp = dynamic_cast<CSI_UDP_MCAST*>(csi);

    if(csi_udp){
        // 1. Find the factory object;
        cModuleType * moduleType = cModuleType::get(
                    "soa4core.endpoints.subscriber.ip.udp.UDPMcastSubscriberEndpoint");
        // 2. Create the module;
        UDPMcastSubscriberEndpoint* udpEndpoint =
                            dynamic_cast<UDPMcastSubscriberEndpoint*>(
                                    moduleType->create("subscriberEndpoints", this->getParentModule(), _subscriberEndpointCount + 1, _subscriberEndpointCount));
        _subscriberEndpointCount++;
        // 3. Set up its parameters and gate sizes as needed;
        string localAddr = subscriberConnector->getAddress().str();
        udpEndpoint->par("localAddress").setStringValue(localAddr);
        int localPort = subscriberConnector->getUdpPort();
        udpEndpoint->par("localPort").setIntValue(localPort);
        string mcastAddr = csi_udp->getDestAddress();
        udpEndpoint->par("mcastDestAddress").setStringValue(mcastAddr);
        int mcastPort = csi_udp->getDestPort();
        udpEndpoint->par("mcastDestPort").setIntValue(mcastPort);

        // cast back.
        ret = dynamic_cast<SubscriberEndpointBase*>(udpEndpoint);
        //connect endpoint to the reader
        ret->setSubscriberConnector(subscriberConnector);
        subscriberConnector->setEndpoint(ret);
    }

    return ret;
}

PublisherEndpointBase* QoSManager::createAVBPublisherEndpoint(
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

        auto vlanID = publisherApplication->getVlanId();
        if(vlanID >= 0) {
            avbEndpoint->par("vlan_id").setIntValue(vlanID);
        }
        auto pcp = publisherApplication->getPcp();
        if(pcp >= 0) {
            avbEndpoint->par("pcp").setIntValue(pcp);
        }

        auto payload = publisherApplication->getPayloadMax();
        size_t framesize = payload + ETHER_MAC_FRAME_BYTES + ETHER_8021Q_TAG_BYTES;
        if (framesize < MIN_ETHERNET_FRAME_BYTES) {
            framesize = MIN_ETHERNET_FRAME_BYTES;
        } else if (framesize > MAX_ETHERNET_FRAME_BYTES) {
            throw cRuntimeError ("Reserved framesize does not fit into one Ethernet frame.");
        }
        avbEndpoint->par("payload").setIntValue(payload);

        // cast back.
        ret = dynamic_cast<PublisherEndpointBase*>(avbEndpoint);
        //connect endpoint to the reader
        ret->setPublisherConnector(publisherConnector);
        publisherConnector->addEndpoint(ret);
    }

    return ret;
}

PublisherEndpointBase* QoSManager::createTCPPublisherEndpoint(
        QoSGroup qosGroup,
        PublisherConnector* publisherConnector) {

    PublisherEndpointBase* ret = nullptr;

    if(qosGroup == QoSGroup::IP_TCP){
        // 1. Find the factory object;
        cModuleType * moduleType = cModuleType::get(
                    "soa4core.endpoints.publisher.ip.tcp.TCPPublisherEndpoint");
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
        tcpEndpoint->par("pcp").setIntValue(publisherApplication->getPcp());
        tcpEndpoint->par("vlan_id").setIntValue(publisherApplication->getVlanId());

        // cast back.
        ret = dynamic_cast<PublisherEndpointBase*>(tcpEndpoint);
        //connect endpoint to the reader
        ret->setPublisherConnector(publisherConnector);
        publisherConnector->addEndpoint(ret);
    }

    return ret;
}

PublisherEndpointBase* QoSManager::createUDPPublisherEndpoint(
        QoSGroup qosGroup,
        PublisherConnector* publisherConnector) {

    PublisherEndpointBase* ret = nullptr;

    if(qosGroup == QoSGroup::IP_UDP){
        // 1. Find the factory object;
        cModuleType * moduleType = cModuleType::get(
                    "soa4core.endpoints.publisher.ip.udp.UDPPublisherEndpoint");
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
        udpEndpoint->par("pcp").setIntValue(publisherApplication->getPcp());
        udpEndpoint->par("vlan_id").setIntValue(publisherApplication->getVlanId());

        // cast back.
        ret = dynamic_cast<PublisherEndpointBase*>(udpEndpoint);
        //connect endpoint to the reader
        ret->setPublisherConnector(publisherConnector);
        publisherConnector->addEndpoint(ret);
    }

    return ret;
}

PublisherEndpointBase* QoSManager::createUDPMcastPublisherEndpoint(
        QoSGroup qosGroup,
        PublisherConnector* publisherConnector) {

    PublisherEndpointBase* ret = nullptr;

    if(qosGroup == QoSGroup::IP_UDP_MCAST){
        // 1. Find the factory object;
        cModuleType * moduleType = cModuleType::get(
                    "soa4core.endpoints.publisher.ip.udp.UDPMcastPublisherEndpoint");
        // 2. Create the module;
        UDPMcastPublisherEndpoint* udpEndpoint =
                            dynamic_cast<UDPMcastPublisherEndpoint*>(
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
        string mcastAddr = publisherApplication->getMcastDestAddr();
        udpEndpoint->par("mcastDestAddress").setStringValue(mcastAddr);
        int mcastPort = publisherApplication->getMcastDestPort();
        udpEndpoint->par("mcastDestPort").setIntValue(mcastPort);
        udpEndpoint->par("pcp").setIntValue(publisherApplication->getPcp());
        udpEndpoint->par("vlan_id").setIntValue(publisherApplication->getVlanId());

        // cast back.
        ret = dynamic_cast<PublisherEndpointBase*>(udpEndpoint);
        //connect endpoint to the reader
        ret->setPublisherConnector(publisherConnector);
        publisherConnector->addEndpoint(ret);
    }

    return ret;
}

} /* end namespace SOA4CoRE */
