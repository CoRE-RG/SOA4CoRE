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

#include "QoSLocalServiceManager.h"
#include <inet/networklayer/common/L3Address.h>
#include <soqosmw/discovery/someipservicediscovery/SomeIpSD.h>

namespace SOQoSMW {

Define_Module(QoSLocalServiceManager);

void QoSLocalServiceManager::initialize(int stage)
{
    LocalServiceManager::initialize(stage);
    if (stage == inet::INITSTAGE_APPLICATION_LAYER) {
        if(!(_qosnp =
               dynamic_cast<QoSNegotiationProtocol*>(getParentModule()->getSubmodule(
                       par("qosnpmoduleName"))))) {
            throw cRuntimeError("No QoSNegotiationProtocol module found.");
        }
        if (!(_sd = dynamic_cast<IServiceDiscovery*>(getParentModule()->getSubmodule(par("sdmoduleName"))))) {
            throw cRuntimeError("No IServiceDiscovery found.");
        }
        if (dynamic_cast<SomeIpSD*>(_sd)) {
            throw cRuntimeError("QoSLocalServiceManager does not work with SOME/IP ServiceDiscovery.");
        }
        if (cSimpleModule* sd = dynamic_cast<cSimpleModule*>(_sd)) {
            sd->subscribe("serviceFoundSignal",this);
        } else {
            throw cRuntimeError("Service discovery serviceFoundSignal could not be subscribed.");
        }

    }
}

void QoSLocalServiceManager::handleMessage(cMessage *msg) {
    // Nothing to do
}

void QoSLocalServiceManager::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj, cObject *details) {
    if (!strcmp(getSignalName(signalID),"serviceFoundSignal")) {
        subscribeFoundService(obj);
    } else {
        throw cRuntimeError("Unknown signal.");
    }
}

void QoSLocalServiceManager::subscribeFoundService(cObject* obj) {
    if (IService *service = dynamic_cast<IService*>(obj)) {
        for (QoSService& qosService : _pendingRequestsMap[service->getServiceId()]) {
            Request* request = createNegotiationRequest(service, qosService.getQoSPolicyMap());
            //create qos broker for the request
            _qosnp->createQoSBroker(request);
        }
        _pendingRequestsMap.erase(service->getServiceId());
        _lsr->addPublisherService(service);
    } else {
        throw cRuntimeError("Given object is not a type of IService.");
    }
}

void QoSLocalServiceManager::subscribeService(IServiceIdentifier& publisherServiceIdentifier, QoSPolicyMap& qosPolicyMap, uint16_t instanceId) {
    //check if publisher is already discovered, and if so, start the negotiation with a request.
    if (IService* service = _lsr->getService(dynamic_cast<ServiceIdentifier&>(publisherServiceIdentifier))) {
        Request* request = createNegotiationRequest(service, qosPolicyMap);
        //create qos broker for the request
        _qosnp->createQoSBroker(request);
    } else {
        LocalServiceManager::addServiceToPendingRequestsMap(publisherServiceIdentifier, qosPolicyMap, instanceId);
        _sd->discover(dynamic_cast<ServiceIdentifier&>(publisherServiceIdentifier));
    }
}

Request* QoSLocalServiceManager::createNegotiationRequest(IService* publisherService, QoSPolicyMap qosPolicies) {
    EndpointDescription subscriber(publisherService->getServiceId(), _localAddress, _qosnp->getProtocolPort());
    EndpointDescription publisher(publisherService->getServiceId(), publisherService->getAddress(), _qosnp->getProtocolPort());
    Request *request = new Request(_requestID++, subscriber, publisher, qosPolicies, nullptr);
    return request;
}

} /* end namespace SOQoSMW */
