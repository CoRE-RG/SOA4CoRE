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

#include "soa4core/discovery/DiscoveryNotification.h"
#include "soa4core/discovery/someip/SomeIpSD.h"
#include "soa4core/discovery/someip/SomeIpSDFindRequest.h"
#include "soa4core/discovery/someip/SomeIpSDFindResult.h"
#include "soa4core/manager/qos/QoSManager.h"
//STD
#include <algorithm>
//INET
#include <inet/networklayer/common/L3Address.h>

namespace SOA4CoRE {

Define_Module(QoSManager);

void QoSManager::initialize(int stage)
{
    Manager::initialize(stage);
    if (stage == inet::INITSTAGE_APPLICATION_LAYER) {
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
    SomeIpSDFindRequest* someIpSDFindRequest = dynamic_cast<SomeIpSDFindRequest*>(obj);
    if (PublisherConnector* publisherConnector = _lsr->getPublisherConnector(someIpSDFindRequest->getServiceId())) {
        if(Publisher* publisher = dynamic_cast<Publisher*>(publisherConnector->getApplication())) {
            SomeIpSDFindResult* someIpSDFindResult = new SomeIpSDFindResult(
                            someIpSDFindRequest->getRemoteAddress(),
                            publisher
            );
            emit(_findResultSignal,someIpSDFindResult);
        }
    }
    delete(someIpSDFindRequest);
}

void QoSManager::subscribeService(ServiceIdentifier publisherServiceIdentifier, ServiceBase* subscriberApplication) {
    Subscriber* subscriberApplication_ = nullptr;
    if (!(subscriberApplication_ = dynamic_cast<Subscriber*>(subscriberApplication))) {
        throw cRuntimeError("The subscriber application must be of type Subscriber");
    }
    //check if publisher is already discovered, and if so, start the negotiation with a request.
    if (PublisherConnector* publisherConnector = _lsr->getPublisherConnector(publisherServiceIdentifier.getServiceId())) {
        Publisher* publisherApplication = nullptr;
        if (!(publisherApplication = dynamic_cast<Publisher*>(publisherConnector->getApplication()))) {
            throw cRuntimeError("The publisher application must be of type Subscriber");
        }
        Request* request = createNegotiationRequest(publisherApplicationInformation, subscriberApplication_->getQoSGroup());
        //create qos broker for the request
        _qosnp->createQoSBroker(request);
    }
    else {
        addSubscriberToPendingRequestsMap(publisherServiceIdentifier, subscriberApplication_);
        _sd->discover(publisherServiceIdentifier);
    }
}

Request* QoSManager::createNegotiationRequest(Registry::ServiceId serviceId, inet::L3Address publisherAddress, QoSGroup qosGroup) {
    EndpointDescription subscriber(serviceId, _localAddress, _qosnp->getProtocolPort());
    EndpointDescription publisher(serviceId, publisherAddress, _qosnp->getProtocolPort());
    Request *request = new Request(_requestID++, subscriber, publisher, qosGroup, nullptr);
    return request;
}

void QoSManager::addSubscriberToPendingRequestsMap(ServiceIdentifier publisherServiceIdentifier, Subscriber* subscriberApplication) {
    if (_pendingRequestsMap.count(publisherServiceIdentifier.getServiceId())) {
        _pendingRequestsMap[publisherServiceIdentifier.getServiceId()].push_back(subscriberApplication);
    } else {
        std::list<Subscriber*> subscriberApplications;
        subscriberApplications.push_back(subscriberApplication);
        _pendingRequestsMap[publisherServiceIdentifier.getServiceId()] = subscriberApplications;
    }
}

} /* end namespace SOA4CoRE */
