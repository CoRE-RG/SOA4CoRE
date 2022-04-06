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

#include "soa4core/discovery/SomeIpDiscoveryNotification.h"
#include "soa4core/discovery/someip/SomeIpSD.h"
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



void QoSManager::discoverService(ServiceIdentifier publisherServiceIdentifier, ServiceBase* subscriberApplication) {
    Subscriber* subscriberApplication_ = nullptr;
    if (!(subscriberApplication_ = dynamic_cast<Subscriber*>(subscriberApplication))) {
        throw cRuntimeError("Subscriber application must be of type Subscriber.");
    }
    _pendingRequests.push_back(std::make_pair(publisherServiceIdentifier.getServiceId(), subscriberApplication_->getQoSGroup()));
    _sd->discover(publisherServiceIdentifier);
}

Request* QoSManager::createNegotiationRequest(Registry::ServiceId serviceId, inet::L3Address publisherAddress, QoSGroup qosGroup) {
    EndpointDescription subscriber(serviceId, _localAddress, _qosnp->getProtocolPort());
    EndpointDescription publisher(serviceId, publisherAddress, _qosnp->getProtocolPort());
    Request *request = new Request(_requestID++, subscriber, publisher, qosGroup, nullptr);
    return request;
}

} /* end namespace SOA4CoRE */
