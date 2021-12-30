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

#include "soqosmw/discovery/someipservicediscovery/SomeIpSDFindResult.h"
#include "soqosmw/servicemanager/someipservicemanager/SomeIpLocalServiceManager.h"
#include "soqosmw/discovery/someipservicediscovery/SomeIpSDSubscriptionInformation.h"
#include "soqosmw/endpoints/publisher/someip/udp/SOMEIPUDPPublisherEndpoint.h"
#include <algorithm>
namespace SOQoSMW {

Define_Module(SomeIpLocalServiceManager);

void SomeIpLocalServiceManager::initialize(int stage) {
    LocalServiceManager::initialize(stage);
    if (stage == inet::INITSTAGE_APPLICATION_LAYER) {
        _qosnpAvailable = true;
        if(!(_qosnp =
               dynamic_cast<QoSNegotiationProtocol*>(getParentModule()->getSubmodule(
                       par("qosnpmoduleName"))))) {
            _qosnpAvailable = false;
        }

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

void SomeIpLocalServiceManager::handleMessage(cMessage *msg) {
    // Does nothing here
}

// Subscriber-side
void SomeIpLocalServiceManager::processAcknowledgedSubscription(cObject* obj) {
    QoSService* someIpService = dynamic_cast<QoSService*>(obj);
    _pendingRequestsMap.erase(someIpService->getServiceId());
}

void SomeIpLocalServiceManager::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj, cObject *details) {
    if (!strcmp(getSignalName(signalID),"serviceFindSignal")) {
        lookForService(obj);
    } else if(!strcmp(getSignalName(signalID),"serviceOfferSignal")) {
        addToLocalServiceRegistry(obj);
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
void SomeIpLocalServiceManager::subscribeService(QoSServiceIdentifier publisherServiceIdentifier, QoSService qosService) {
    bool serviceIsKnown = false;
    bool serviceFound = false;
    serviceIsKnown = _lsr->containsService(publisherServiceIdentifier);
    if (_qosnpAvailable && serviceIsKnown) {
        //Request* request = createNegotiationRequest(service);
        //_qosnp->createQoSBroker(request);
    } else if (serviceIsKnown) {
        QoSService publisherService = _lsr->getService(publisherServiceIdentifier);
        std::vector<QoSGroups> publisherQoSGroups = publisherService.getQosGroups();
        QoSGroups qosGroup = qosService.getQosGroups()[0];
        if (std::find(publisherQoSGroups.begin(), publisherQoSGroups.end(), qosGroup) != publisherQoSGroups.end()) {
            publisherService = getQoSServiceCopyWithGivenQoSGroupOnly(publisherService, qosGroup);
            subscribeServiceIfThereIsAPendingRequest(new QoSService(publisherService));
            serviceFound = true;
        }
    }
    if (!serviceFound) {
        LocalServiceManager::addServiceToPendingRequestsMap(publisherServiceIdentifier, qosService);
        _sd->discover(publisherServiceIdentifier);
    }
}

// Publisher-side
void SomeIpLocalServiceManager::lookForService(cObject* obj) {
    SomeIpSDFindRequest* someIpSDFindRequest = dynamic_cast<SomeIpSDFindRequest*>(obj);
    ServiceIdentifier serviceIdentifier = ServiceIdentifier(someIpSDFindRequest->getServiceId());
    if (QoSService* service = dynamic_cast<QoSService*>(_lsr->getService(serviceIdentifier))) {
        IPProtocolId ipProtocolId = getIPProtocolId(service);
        SomeIpSDFindResult* someIpSDFindResult = new SomeIpSDFindResult(
                someIpSDFindRequest->getServiceId(),
                someIpSDFindRequest->getInstanceId(),
                someIpSDFindRequest->getRemoteAddress(),
                service,
                ipProtocolId
        );
        delete(someIpSDFindRequest);
        emit(_findResultSignal,someIpSDFindResult);
    }
}

// Subscriber-side
void SomeIpLocalServiceManager::addToLocalServiceRegistry(cObject* obj) {
    QoSService* qosService = dynamic_cast<QoSService*>(obj);
    if (!qosService) {
        throw cRuntimeError("Service is not of type QoSService.");
    }
    _lsr->addPublisherService(qosService);
}

// Subscriber-side
void SomeIpLocalServiceManager::subscribeServiceIfThereIsAPendingRequest(cObject* obj) {
    QoSService* qosService = dynamic_cast<QoSService*>(obj);
    createSubscriberEndpoint(qosService);
    if (_pendingRequestsMap.count(qosService->getServiceId())) {
        for(std::list<QoSService>::const_iterator it = _pendingRequestsMap[qosService->getServiceId()].begin(); it != _pendingRequestsMap[qosService->getServiceId()].end(); ++it){
            SomeIpSDSubscriptionInformation someIpSDSubscriptionInformation = SomeIpSDSubscriptionInformation(
                    qosService->getServiceId(),
                    qosService->getInstanceId(),
                    it->getAddress(),
                    it->getPort(),
                    qosService->getAddress()
            );
            emit(_subscribeSignal,&someIpSDSubscriptionInformation);
        }
    }
}

// Publisher-side
void SomeIpLocalServiceManager::acknowledgeSubscription(cObject* obj) {
    QoSService& qosService = *dynamic_cast<QoSService*>(obj);
    ServiceIdentifier serviceIdentifier = ServiceIdentifier(qosService.getServiceId());
    if (IService* service = _lsr->getService(serviceIdentifier)) {
        SomeIpSDSubscriptionInformation someIpSDSubscriptionInformation = SomeIpSDSubscriptionInformation(
                qosService.getServiceId(),
                qosService.getInstanceId(),
                service->getAddress(),
                service->getPort(),
                qosService.getAddress()
        );
        //TODO Should become possible for other QoS classes too, currently limited to SOME/IP UDP
        PublisherEndpointBase* pub = createOrFindPublisherFor(qosService.getServiceId(),QoSGroups::SOMEIP_UDP);
        if(!pub) {
            throw cRuntimeError("No Publisher was created.");
        }
        SOMEIPUDPPublisherEndpoint* someipUdpPublisherEndpoint = dynamic_cast<SOMEIPUDPPublisherEndpoint*>(pub);
        if(!(someipUdpPublisherEndpoint)) {
            throw cRuntimeError("Endpoint is no SOMEIPUDPPublisherEndpoint.");
        }
        ConnectionSpecificInformation* info = pub->getConnectionSpecificInformation();
        if (!info) {
            throw cRuntimeError("No ConnectionSpecificInformation received.");
        }
        CSI_SOMEIP_UDP* subConnection = new CSI_SOMEIP_UDP();
        subConnection->setAddress(qosService.getAddress().str().c_str());
        subConnection->setPort(qosService.getPort());
        switch (info->getConnectionType()) {
        case ConnectionType::ct_someip_udp:
            someipUdpPublisherEndpoint->addRemote(subConnection);
            break;
        default:
            throw cRuntimeError("Unknown connection type. Only SOME/IP is currently supported");
        }
        delete subConnection;
        delete info;
        delete qosService;
        emit(_subscribeAckSignal,&someIpSDSubscriptionInformation);
    }
}

// Subscriber-side
Request* SomeIpLocalServiceManager::createNegotiationRequest(QoSService publisherService) {
    EndpointDescription subscriber(publisherService.getServiceId(), _localAddress, _qosnp->getProtocolPort());
    EndpointDescription publisher(publisherService.getServiceId(), publisherService.getAddress(), _qosnp->getProtocolPort());
    Request *request = new Request(_requestID++, subscriber, publisher, publisherService, nullptr);
    return request;
}

// Subscriber-side
void SomeIpLocalServiceManager::createSubscriberEndpoint(IService* service) {
    ConnectionSpecificInformation* connectionlessCSI = new CSI_SOMEIP_UDP();
    SubscriberEndpointBase* sub = createOrFindSubscriberFor(service->getServiceId(), connectionlessCSI);
    delete connectionlessCSI;
    if(!sub) {
        throw cRuntimeError("No subscriber was created...");
    }
}

IPProtocolId SomeIpLocalServiceManager::getIPProtocolId(QoSService* service) {
    QoSGroups qosGroup = dynamic_cast<QoSGroup*>(service->getQoSPolicyMap()[QoSPolicyNames::QoSGroup])->getValue();
    IPProtocolId ipProtocolId = IPProtocolId::IP_PROT_ICMP;
    switch (qosGroup) {
        case QoSGroups::SOMEIP_TCP:
            ipProtocolId = IPProtocolId::IP_PROT_TCP;
            break;
        case QoSGroups::SOMEIP_UDP:
            ipProtocolId = IPProtocolId::IP_PROT_UDP;
            break;
        default:
            throw cRuntimeError("Unknown QoS group.");
    }
    return ipProtocolId;
}

QoSService SomeIpLocalServiceManager::getQoSServiceCopyWithGivenQoSGroupOnly(QoSService qosService, QoSGroups qosGroup) {
    std::vector<QoSGroups> qosGroups;
    qosGroups.push_back(qosGroup);
    return QoSService(qosService.getServiceId(), qosService.getAddress(),
            qosService.getInstanceId(), qosGroups, qosService.getTCPPort(), qosService.getUDPPort(),
            qosService.getStreamId(), qosService.getSrClass(), qosService.getFramesize(),
            qosService.getIntervalFrames());
}

} /* end namespace SOQoSMW */
