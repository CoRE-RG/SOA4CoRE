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

#include <soqosmw/discovery/someipservicediscovery/SomeIpSDFindResult.h>
#include "soqosmw/servicemanager/someipservicemanager/SomeIpLocalServiceManager.h"
#include "soqosmw/discovery/someipservicediscovery/SomeIpSDSubscriptionInformation.h"
#include "soqosmw/endpoints/publisher/someip/udp/SOMEIPUDPPublisherEndpoint.h"

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

        if (!(_sd = dynamic_cast<IServiceDiscovery*>(getParentModule()->getSubmodule("sdmoduleName")))) {
            throw cRuntimeError("No IServiceDiscovery found.");
        }
        if(SomeIpSD* sd = dynamic_cast<SomeIpSD*>(_sd)) {
            sd->subscribe("serviceFoundSignal",this);
            sd->subscribe("serviceFindSignal",this);
            sd->subscribe("serviceOfferSignal",this);
            sd->subscribe("subscribeEventGroupSignal",this);
            sd->subscribe("subscribeEventGroupAckSignal",this);
        }

        _findResultSignal = omnetpp::cComponent::registerSignal("findResultSignal");
        _subscribeSignal = omnetpp::cComponent::registerSignal("subscribeSignal");
        _subscribeAckSignal = omnetpp::cComponent::registerSignal("subscribeAckSignal");
    }
}

void SomeIpLocalServiceManager::handleMessage(cMessage *msg) {
    // Does nothing here
}


void SomeIpLocalServiceManager::processAcknowledgedSubscription(cObject* obj) {
    QoSService& someIpService = *dynamic_cast<QoSService*>(obj);
    _pendingOffersMap.erase(someIpService.getServiceId());
}

void SomeIpLocalServiceManager::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj, cObject *details) {
    if (!strcmp(getSignalName(signalID),"serviceFindSignal")) {
        lookForService(obj);
    } else if(!strcmp(getSignalName(signalID),"serviceOfferSignal")) {
        addToPendingOffers(obj);
        subscribeServiceIfThereIsAPendingRequest(obj);
    } else if(!strcmp(getSignalName(signalID),"subscribeEventGroupSignal")){
        acknowledgeSubscription(obj);
    } else if (!strcmp(getSignalName(signalID),"subscribeEventGroupAckSignal")) {
        processAcknowledgedSubscription(obj);
    } else if (!strcmp(getSignalName(signalID),"serviceFoundSignal")) { //TODO makes no sense, service is already found after offer
        subscribeFoundService(obj);
    }
}

void SomeIpLocalServiceManager::subscribeFoundService(cObject* obj) {
    //TODO Need to be removed, because subscription is already done with subscribeServiceIfThereIsAPendingRequest
    /*
    if (IService *service = dynamic_cast<IService*>(obj)) {
        for (QoSService& qosService : _pendingRequestsMap[service->getServiceId()]) {
            if (_qosnpAvailable) {
                Request* request = createNegotiationRequest(service, qosService.getQoSPolicyMap());
                //create qos broker for the request
                _qosnp->createQoSBroker(request);
            } else {
                //TODO start subscription
            }
        }
        _pendingRequestsMap.erase(service->getServiceId());
        _lsr->addPublisherService(service);
    } else {
        throw cRuntimeError("Given object is not a type of IService.");
    }
    */
}

void SomeIpLocalServiceManager::subscribeService(IServiceIdentifier& publisherServiceIdentifier, QoSPolicyMap& qosPolicyMap, uint16_t instanceId) {
    IService* service = _lsr->getService(dynamic_cast<ServiceIdentifier&>(publisherServiceIdentifier));
    if (_qosnpAvailable && service) {
        Request* request = createNegotiationRequest(service, qosPolicyMap);
        _qosnp->createQoSBroker(request);
    } else if (service) {
        ConnectionSpecificInformation* connectionlessCSI = new CSI_SOMEIP();
        SubscriberEndpointBase* sub = createOrFindSubscriberFor(service->getServiceId(),connectionlessCSI);
        delete connectionlessCSI;
        if(!sub) {
            throw cRuntimeError("No subscriber was created...");
        }
        subscribeServiceIfThereIsAPendingRequest(service);
    } else {
        LocalServiceManager::addServiceToPendingRequestsMap(publisherServiceIdentifier, qosPolicyMap, instanceId);
        _sd->discover(dynamic_cast<ServiceIdentifier&>(publisherServiceIdentifier));
    }
}

void SomeIpLocalServiceManager::lookForService(cObject* obj) {
    SomeIpSDFindRequest* someIpSDFindRequest = dynamic_cast<SomeIpSDFindRequest*>(obj);
    ServiceIdentifier serviceIdentifier = ServiceIdentifier(someIpSDFindRequest->getServiceId());
    if (IService* service = _lsr->getService(serviceIdentifier)) {
        SomeIpSDFindResult* someIpSDFindResult = new SomeIpSDFindResult(
                someIpSDFindRequest->getServiceId(),
                someIpSDFindRequest->getInstanceId(),
                someIpSDFindRequest->getRemoteAddress(),
                service
        );
        delete(someIpSDFindRequest);
        emit(_findResultSignal,someIpSDFindResult);
    }
}

void SomeIpLocalServiceManager::addToPendingOffers(cObject* obj) {
    QoSService& qosService = *dynamic_cast<QoSService*>(obj);
    if (_pendingOffersMap.count(qosService.getServiceId())) {
        _pendingOffersMap[qosService.getServiceId()].push_back(qosService);
    } else {
        std::list<QoSService> qosServices;
        qosServices.push_back(qosService);
        _pendingOffersMap[qosService.getServiceId()] = qosServices;
    }
}

void SomeIpLocalServiceManager::subscribeServiceIfThereIsAPendingRequest(cObject* obj) {
    QoSService& qosService = *dynamic_cast<QoSService*>(obj);
    if (_pendingRequestsMap.count(qosService.getServiceId())) {
        for(std::list<QoSService>::const_iterator it = _pendingRequestsMap[qosService.getServiceId()].begin(); it != _pendingRequestsMap[qosService.getServiceId()].end(); ++it){
            SomeIpSDSubscriptionInformation someIpSDSubscriptionInformation = SomeIpSDSubscriptionInformation(
                    qosService.getServiceId(),
                    qosService.getInstanceId(),
                    it->getAddress(),
                    it->getPort(),
                    qosService.getAddress()
            );
            emit(_subscribeSignal,&someIpSDSubscriptionInformation);
        }
    }
}

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
        //TODO Should become possible for other QoS classes too, currently limited to SOME/IP
        PublisherEndpointBase* pub = createOrFindPublisherFor(qosService.getServiceId(),QoSGroups::SOMEIP);
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
        CSI_SOMEIP* subConnection = new CSI_SOMEIP();
        subConnection->setAddress(qosService.getAddress().str().c_str());
        subConnection->setPort(qosService.getPort());
        switch (info->getConnectionType()) {
        case ConnectionType::ct_someip:
            someipUdpPublisherEndpoint->addRemote(subConnection);
            break;
        default:
            throw cRuntimeError("Unknown connection type. Only SOME/IP is currently supported");
        }
        delete subConnection;
        emit(_subscribeAckSignal,&someIpSDSubscriptionInformation);
    }
}

Request* SomeIpLocalServiceManager::createNegotiationRequest(IService* publisherService, QoSPolicyMap qosPolicies) {
    EndpointDescription subscriber(publisherService->getServiceId(), _localAddress, _qosnp->getProtocolPort());
    EndpointDescription publisher(publisherService->getServiceId(), publisherService->getAddress(), _qosnp->getProtocolPort());
    Request *request = new Request(_requestID++, subscriber, publisher, qosPolicies, nullptr);
    return request;
}

} /* end namespace SOQoSMW */
