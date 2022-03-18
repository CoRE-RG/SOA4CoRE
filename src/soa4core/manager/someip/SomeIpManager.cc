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

#include "soa4core/applicationinformation/publisher/PublisherApplicationInformationNotification.h"
#include "soa4core/applicationinformation/subscriber/SubscriberApplicationInformationNotification.h"
#include "soa4core/discovery/someip/SomeIpSD.h"
#include "soa4core/discovery/someip/SomeIpSDAcknowledgeSubscription.h"
#include "soa4core/discovery/someip/SomeIpSDFindRequest.h"
#include "soa4core/discovery/someip/SomeIpSDFindResult.h"
#include "soa4core/discovery/someip/SomeIpSDSubscriptionInformation.h"
#include "soa4core/manager/someip/SomeIpManager.h"
#include "soa4core/endpoints/publisher/someip/udp/SOMEIPUDPPublisherEndpoint.h"
#include "soa4core/endpoints/publisher/someip/tcp/SOMEIPTCPPublisherEndpoint.h"
//STD
#include <algorithm>

namespace SOA4CoRE {

Define_Module(SomeIpManager);

void SomeIpManager::initialize(int stage) {
    Manager::initialize(stage);
    if (stage == inet::INITSTAGE_APPLICATION_LAYER) {
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

void SomeIpManager::handleMessage(cMessage *msg) {
    // Does nothing here
}

// Subscriber-side
void SomeIpManager::processAcknowledgedSubscription(cObject* obj) {
    PublisherApplicationInformationNotification* publisherApplicationInformationNotification = dynamic_cast<PublisherApplicationInformationNotification*>(obj);
    PublisherApplicationInformation publisherApplicationInformation = publisherApplicationInformationNotification->getPublisherApplicationInformation();
    std::list<SubscriberApplicationInformation> subscriberApplicationInformationsToBeRemoved;
    if (_pendingRequestsMap.count(publisherApplicationInformation.getServiceId())) {
        for(std::list<SubscriberApplicationInformation>::const_iterator it = _pendingRequestsMap[publisherApplicationInformation.getServiceId()].begin(); it != _pendingRequestsMap[publisherApplicationInformation.getServiceId()].end(); ++it){
            if (publisherApplicationInformation.containsQoSGroup(it->getQoSGroup())) {
                createSubscriberEndpoint(publisherApplicationInformation, it->getQoSGroup());
                subscriberApplicationInformationsToBeRemoved.push_back(*it);
            }
        }
        for (SubscriberApplicationInformation subscriberApplicationInformation : subscriberApplicationInformationsToBeRemoved) {
            _pendingRequestsMap[subscriberApplicationInformation.getServiceId()].remove(subscriberApplicationInformation);
            if (_pendingRequestsMap[subscriberApplicationInformation.getServiceId()].empty()) {
                _pendingRequestsMap.erase(subscriberApplicationInformation.getServiceId());
            }
        }
    }
    delete obj;
}

void SomeIpManager::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj, cObject *details) {
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
void SomeIpManager::subscribeService(ServiceIdentifier publisherServiceIdentifier, SubscriberApplicationInformation subscriberApplicationInformation) {
    bool serviceFound = false;
    if (_lsr->containsService(publisherServiceIdentifier)) {
        PublisherApplicationInformation publisherService = _lsr->getService(publisherServiceIdentifier);
        if (publisherService.containsQoSGroup(subscriberApplicationInformation.getQoSGroup())) {
            PublisherApplicationInformationNotification* publisherApplicationInformationNotification =
                    new PublisherApplicationInformationNotification(publisherService);
            subscribeServiceIfThereIsAPendingRequest(publisherApplicationInformationNotification);
            serviceFound = true;
        } else {
            throw cRuntimeError("The service id = %d is not provided with the requested QoS group = %d",
                    publisherServiceIdentifier.getServiceId(), subscriberApplicationInformation.getQoSGroup());
        }
    }
    if (!serviceFound) {
        Manager::addSubscriberToPendingRequestsMap(publisherServiceIdentifier, subscriberApplicationInformation);
        _sd->discover(publisherServiceIdentifier);
    }
}

// Publisher-side
void SomeIpManager::lookForService(cObject* obj) {
    SomeIpSDFindRequest* someIpSDFindRequest = dynamic_cast<SomeIpSDFindRequest*>(obj);
    ServiceIdentifier serviceIdentifier = ServiceIdentifier(someIpSDFindRequest->getServiceId(),
            someIpSDFindRequest->getInstanceId());
    if (_lsr->containsService(serviceIdentifier)){
        PublisherApplicationInformation foundPublisherApplicationInformation = _lsr->getService(serviceIdentifier);
        if(foundPublisherApplicationInformation.getAddress() == _localAddress) {
            SomeIpSDFindResult* someIpSDFindResult = new SomeIpSDFindResult(
                    someIpSDFindRequest->getRemoteAddress(),
                    foundPublisherApplicationInformation
            );
            emit(_findResultSignal,someIpSDFindResult);
        }
    }
    delete(someIpSDFindRequest);
}

// Subscriber-side
void SomeIpManager::addToLocalServiceRegistry(cObject* obj) {
    PublisherApplicationInformationNotification* publisherApplicationInformationNotification = dynamic_cast<PublisherApplicationInformationNotification*>(obj);
    PublisherApplicationInformation publisherApplicationInformation = publisherApplicationInformationNotification->getPublisherApplicationInformation();
    _lsr->addPublisherService(publisherApplicationInformation);
}

// Subscriber-side
void SomeIpManager::subscribeServiceIfThereIsAPendingRequest(cObject* obj) {
    PublisherApplicationInformationNotification* publisherApplicationInformationNotification = dynamic_cast<PublisherApplicationInformationNotification*>(obj);
    PublisherApplicationInformation publisherApplicationInformation = publisherApplicationInformationNotification->getPublisherApplicationInformation();
            dynamic_cast<PublisherApplicationInformationNotification*>(obj)->getPublisherApplicationInformation();
    if (_pendingRequestsMap.count(publisherApplicationInformation.getServiceId())) {
        for(std::list<SubscriberApplicationInformation>::const_iterator it = _pendingRequestsMap[publisherApplicationInformation.getServiceId()].begin(); it != _pendingRequestsMap[publisherApplicationInformation.getServiceId()].end(); ++it){
            if (publisherApplicationInformation.containsQoSGroup(it->getQoSGroup())) {
                SomeIpSDSubscriptionInformation* someIpSDSubscriptionInformation = new SomeIpSDSubscriptionInformation(
                        publisherApplicationInformation.getAddress(),
                        *it
                );
                emit(_subscribeSignal,someIpSDSubscriptionInformation);
            }
        }
    }
    delete obj;
}

// Publisher-side
void SomeIpManager::acknowledgeSubscription(cObject* obj) {
    SubscriberApplicationInformationNotification* subscriberApplicationInformationNotification = dynamic_cast<SubscriberApplicationInformationNotification*>(obj);
    SubscriberApplicationInformation subscriberApplicationInformation = subscriberApplicationInformationNotification->getSubscriberApplicationInformation();
    ServiceIdentifier serviceIdentifier = ServiceIdentifier(subscriberApplicationInformation.getServiceId(), subscriberApplicationInformation.getInstanceId());
    if (_lsr->containsService(serviceIdentifier)) {
        PublisherApplicationInformation publisherApplicationInformation = _lsr->getService(serviceIdentifier);
        if (publisherApplicationInformation.containsQoSGroup(subscriberApplicationInformation.getQoSGroup())) {
            SomeIpSDAcknowledgeSubscription* someIpSDAcknowledgeSubscription = new SomeIpSDAcknowledgeSubscription(
                    subscriberApplicationInformation.getAddress(),
                    publisherApplicationInformation,
                    subscriberApplicationInformation.getQoSGroup()
            );
            switch (subscriberApplicationInformation.getQoSGroup()) {
                case QoSGroup::SOMEIP_TCP: {
                    PublisherEndpointBase* publisherEndpointBase = createOrFindPublisherEndpoint(subscriberApplicationInformation.getServiceId(),QoSGroup::SOMEIP_TCP);
                    if(!publisherEndpointBase) {
                        throw cRuntimeError("No SOME/IP TCP Publisher was created.");
                    }
                    SOMEIPTCPPublisherEndpoint* someipTcpPublisherEndpoint = dynamic_cast<SOMEIPTCPPublisherEndpoint*>(publisherEndpointBase);
                    if(!(someipTcpPublisherEndpoint)) {
                        throw cRuntimeError("Endpoint is no SOMEIPTCPPublisherEndpoint.");
                    }
                    break;
                }
                case QoSGroup::SOMEIP_UDP: {
                    PublisherEndpointBase* publisherEndpointBase = createOrFindPublisherEndpoint(subscriberApplicationInformation.getServiceId(),QoSGroup::SOMEIP_UDP);
                    if(!publisherEndpointBase) {
                        throw cRuntimeError("No SOME/IP UDP Publisher was created.");
                    }
                    SOMEIPUDPPublisherEndpoint* someipUdpPublisherEndpoint = dynamic_cast<SOMEIPUDPPublisherEndpoint*>(publisherEndpointBase);
                    if(!(someipUdpPublisherEndpoint)) {
                        throw cRuntimeError("Endpoint is no SOMEIPUDPPublisherEndpoint.");
                    }
                    CSI_SOMEIP_UDP* csi_udp_someip = new CSI_SOMEIP_UDP();
                    csi_udp_someip->setAddress(subscriberApplicationInformation.getAddress().str().c_str());
                    csi_udp_someip->setPort(subscriberApplicationInformation.getUDPPort());
                    someipUdpPublisherEndpoint->addRemote(csi_udp_someip);
                    delete csi_udp_someip;
                    break;
                }
                default:
                    throw cRuntimeError("Unknown QoS group");
            }
            emit(_subscribeAckSignal,someIpSDAcknowledgeSubscription);
        }
    }
    delete obj;
}

// Subscriber-side
void SomeIpManager::createSubscriberEndpoint(PublisherApplicationInformation publisherApplicationInformation, QoSGroup qosGroup) {
    SubscriberEndpointBase* sub = nullptr;
    switch (qosGroup) {
        case QoSGroup::SOMEIP_TCP: {
            CSI_SOMEIP_TCP* csi_someip_tcp = new CSI_SOMEIP_TCP();
            csi_someip_tcp->setAddress(publisherApplicationInformation.getAddress().str().c_str());
            csi_someip_tcp->setPort(publisherApplicationInformation.getTCPPort());
            sub = createOrFindSubscriberEndpoint(publisherApplicationInformation.getServiceId(), csi_someip_tcp);
            delete csi_someip_tcp;
            break;
        }
        case QoSGroup::SOMEIP_UDP: {
            CSI_SOMEIP_UDP* csi_someip_udp = new CSI_SOMEIP_UDP();
            csi_someip_udp->setAddress(publisherApplicationInformation.getAddress().str().c_str());
            csi_someip_udp->setPort(publisherApplicationInformation.getUDPPort());
            sub = createOrFindSubscriberEndpoint(publisherApplicationInformation.getServiceId(), csi_someip_udp);
            delete csi_someip_udp;
            break;
        }
        default:
            throw cRuntimeError("Unknown QoS group.");
    }

    if(!sub) {
        throw cRuntimeError("No subscriber was created...");
    }
}

IPProtocolId SomeIpManager::getIPProtocolId(QoSGroup qosGroup) {
    IPProtocolId ipProtocolId = IPProtocolId::IP_PROT_ICMP;
    switch (qosGroup) {
        case QoSGroup::SOMEIP_TCP:
            ipProtocolId = IPProtocolId::IP_PROT_TCP;
            break;
        case QoSGroup::SOMEIP_UDP:
            ipProtocolId = IPProtocolId::IP_PROT_UDP;
            break;
        default:
            throw cRuntimeError("Unknown QoS group.");
    }
    return ipProtocolId;
}

void SomeIpManager::addSubscriberToPendingSubscriptionsMap(ServiceIdentifier publisherServiceIdentifier, SubscriberApplicationInformation subscriberApplicationInformation, SubscriptionState subscriptionState) {
    if (_pendingSubscriptionsMap.count(publisherServiceIdentifier.getServiceId())) {
        _pendingSubscriptionsMap[publisherServiceIdentifier.getServiceId()].push_back(std::make_pair(subscriberApplicationInformation,subscriptionState));
    } else {
        std::list<std::pair<SubscriberApplicationInformation, SubscriptionState>> subscriberApplicationInformations_subscriptionState;
        subscriberApplicationInformations_subscriptionState.push_back(std::make_pair(subscriberApplicationInformation,subscriptionState));
        _pendingSubscriptionsMap[publisherServiceIdentifier.getServiceId()] = subscriberApplicationInformations_subscriptionState;
    }
}

} /* end namespace SOA4CoRE */
