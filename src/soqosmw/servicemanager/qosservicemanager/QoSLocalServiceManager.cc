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
#include "soqosmw/discovery/someipservicediscovery/SomeIpSDFindRequest.h"
#include "soqosmw/discovery/someipservicediscovery/SomeIpSDFindResult.h"
#include "soqosmw/service/publisherapplicationinformation/PublisherApplicationInformationNotification.h"
#include <algorithm>

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

void QoSLocalServiceManager::handleMessage(cMessage *msg) {
    // Nothing to do
}

void QoSLocalServiceManager::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj, cObject *details) {
    if (!strcmp(getSignalName(signalID),"serviceOfferSignal")) {
        subscribeOfferedService(obj);
    } else if (!strcmp(getSignalName(signalID),"serviceFindSignal")) {
        lookForService(obj);
    } else {
        throw cRuntimeError("Unknown signal.");
    }
}

void QoSLocalServiceManager::subscribeOfferedService(cObject* obj) {
    if (PublisherApplicationInformationNotification* publisherApplicationInformationNotification = dynamic_cast<PublisherApplicationInformationNotification*>(obj)) {
        PublisherApplicationInformation offeredService = publisherApplicationInformationNotification->getPublisherApplicationInformation();
        std::list<SubscriberApplicationInformation> subscriberApplicationInformationToBeRemoved;
        for (SubscriberApplicationInformation subscriberApplicationInformation : _pendingRequestsMap[offeredService.getServiceId()]) {
            if (offeredService.containsQoSGroup(subscriberApplicationInformation.getQoSGroup())) {
                Request* request = createNegotiationRequest(offeredService, subscriberApplicationInformation.getQoSGroup());
                //create qos broker for the request
                _qosnp->createQoSBroker(request);
                subscriberApplicationInformationToBeRemoved.push_back(subscriberApplicationInformation);
            }
        }
        for (SubscriberApplicationInformation subscriberApplicationInformation : subscriberApplicationInformationToBeRemoved) {
            _pendingRequestsMap[subscriberApplicationInformation.getServiceId()].remove(subscriberApplicationInformation);
            if(!_pendingRequestsMap[subscriberApplicationInformation.getServiceId()].size()) {
                _pendingRequestsMap.erase(subscriberApplicationInformation.getServiceId());
            }
        }
        _lsr->addPublisherService(offeredService);
        delete obj;
    } else {
        throw cRuntimeError("Given object is not type of PublisherApplicationInformation.");
    }
}

// Publisher-side
void QoSLocalServiceManager::lookForService(cObject* obj) {
    SomeIpSDFindRequest* someIpSDFindRequest = dynamic_cast<SomeIpSDFindRequest*>(obj);
    QoSServiceIdentifier serviceIdentifier = QoSServiceIdentifier(someIpSDFindRequest->getServiceId(),
            someIpSDFindRequest->getInstanceId());
    if (_lsr->containsService(serviceIdentifier)){
        PublisherApplicationInformation foundPublisherApplicationInformation = _lsr->getService(serviceIdentifier);
        SomeIpSDFindResult* someIpSDFindResult = new SomeIpSDFindResult(
                someIpSDFindRequest->getRemoteAddress(),
                foundPublisherApplicationInformation
        );
        delete(someIpSDFindRequest);
        emit(_findResultSignal,someIpSDFindResult);
    }
}

void QoSLocalServiceManager::subscribeService(QoSServiceIdentifier publisherServiceIdentifier, SubscriberApplicationInformation subscriberApplicationInformation) {
    //check if publisher is already discovered, and if so, start the negotiation with a request.
    bool serviceFound = false;
    if (_lsr->containsService(publisherServiceIdentifier)) {
        PublisherApplicationInformation publisherApplicationInformation = _lsr->getService(publisherServiceIdentifier);
        if (publisherApplicationInformation.containsQoSGroup(subscriberApplicationInformation.getQoSGroup())) {
            Request* request = createNegotiationRequest(publisherApplicationInformation, subscriberApplicationInformation.getQoSGroup());
            //create qos broker for the request
            _qosnp->createQoSBroker(request);
            serviceFound = true;
        }
    }
    if (!serviceFound) {
        LocalServiceManager::addSubscriberToPendingRequestsMap(publisherServiceIdentifier, subscriberApplicationInformation);
        _sd->discover(publisherServiceIdentifier);
    }
}

Request* QoSLocalServiceManager::createNegotiationRequest(PublisherApplicationInformation publisherApplicationInformation, QoSGroup qosGroup) {
    EndpointDescription subscriber(publisherApplicationInformation.getServiceId(), _localAddress, _qosnp->getProtocolPort());
    EndpointDescription publisher(publisherApplicationInformation.getServiceId(), publisherApplicationInformation.getAddress(), _qosnp->getProtocolPort());
    Request *request = new Request(_requestID++, subscriber, publisher, qosGroup, nullptr);
    return request;
}

} /* end namespace SOQoSMW */
