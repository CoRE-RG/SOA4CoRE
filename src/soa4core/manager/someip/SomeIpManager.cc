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
    SomeIpDiscoveryNotification* someIpDiscoveryNotification = nullptr;
    if (!(someIpDiscoveryNotification = dynamic_cast<SomeIpDiscoveryNotification*>(obj))) {
        throw cRuntimeError("The discovery notification must be of type SomeIpDiscoveryNotification");
    }
    std::set<QoSGroup> qosGroups = someIpDiscoveryNotification->getQoSGroups();
    for (QoSGroup qosGroup : qosGroups) {
        if (_pendingSubscriptionsMap.count(someIpDiscoveryNotification->getServiceId())
            && _pendingSubscriptionsMap[someIpDiscoveryNotification->getServiceId()].count(qosGroup)
            && _pendingSubscriptionsMap[someIpDiscoveryNotification->getServiceId()][qosGroup] == SubscriptionState::WAITING_FOR_SUBACK) {
            createSubscriberEndpoint(someIpDiscoveryNotification, qosGroup);
            _pendingSubscriptionsMap[someIpDiscoveryNotification->getServiceId()].erase(qosGroup);
            if (_pendingSubscriptionsMap[someIpDiscoveryNotification->getServiceId()].empty()) {
                _pendingSubscriptionsMap.erase(someIpDiscoveryNotification->getServiceId());
            }
        }
    }
    delete obj;
}

void SomeIpManager::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj, cObject *details) {
    if (!strcmp(getSignalName(signalID),"serviceFindSignal")) {
        lookForService(obj);
    } else if(!strcmp(getSignalName(signalID),"serviceOfferSignal")) {
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
void SomeIpManager::discoverService(ServiceIdentifier publisherServiceIdentifier, ServiceBase* subscriberApplication) {
    Subscriber* subscriberApplication_ = nullptr;
    if (!(subscriberApplication_ = dynamic_cast<Subscriber*>(subscriberApplication))) {
        throw cRuntimeError("Subscriber application must be of type Subscriber.");
    }
    _pendingSubscriptionsMap[publisherServiceIdentifier.getServiceId()][subscriberApplication_->getQoSGroup()] = SubscriptionState::WAITING_FOR_OFFER;
    _sd->discover(publisherServiceIdentifier);
}

// Publisher-side
void SomeIpManager::lookForService(cObject* obj) {
    SomeIpDiscoveryNotification* someIpDiscoveryNotificationFindRequest = dynamic_cast<SomeIpDiscoveryNotification*>(obj);
    if (PublisherConnector* publisherConnector = _lsr->getPublisherConnector(someIpDiscoveryNotificationFindRequest->getServiceId())) {
        if(Publisher* publisher = dynamic_cast<Publisher*>(publisherConnector->getApplication())) {
            SomeIpDiscoveryNotification* someIpDiscoveryNotificationFindResponse = new SomeIpDiscoveryNotification(publisher->getServiceId(),
                    someIpDiscoveryNotificationFindRequest->getAddress(), publisher->getInstanceId(), publisher->getQoSGroups(),
                    QoSGroup::UNDEFINED, publisher->getTcpPort(), publisher->getUdpPort()
            );
            emit(_findResultSignal,someIpDiscoveryNotificationFindResponse);
        }
    }
    delete(someIpDiscoveryNotificationFindRequest);
}

// Subscriber-side
void SomeIpManager::subscribeServiceIfThereIsAPendingRequest(cObject* obj) {
    SomeIpDiscoveryNotification* someIpdiscoveryNotificationOffer = nullptr;
    if (!(someIpdiscoveryNotificationOffer = dynamic_cast<SomeIpDiscoveryNotification*>(obj))) {
        throw cRuntimeError("Notification must be of type SomeIpDiscoveryNotification.");
    }
    if (_pendingSubscriptionsMap.count(someIpdiscoveryNotificationOffer->getServiceId())) {
        std::set<QoSGroup> offeredQoSGroups = someIpdiscoveryNotificationOffer->getQoSGroups();
        std::list<SubscriberConnector*> subscriberConnectors = _lsr->getSubscriberConnectors(someIpdiscoveryNotificationOffer->getServiceId());

        for (QoSGroup offeredQoSGroup : offeredQoSGroups) {
            if (_pendingSubscriptionsMap.count(someIpdiscoveryNotificationOffer->getServiceId())
                && _pendingSubscriptionsMap[someIpdiscoveryNotificationOffer->getServiceId()].count(offeredQoSGroup)
                && _pendingSubscriptionsMap[someIpdiscoveryNotificationOffer->getServiceId()][offeredQoSGroup] == SubscriptionState::WAITING_FOR_OFFER) {
                _pendingSubscriptionsMap[someIpdiscoveryNotificationOffer->getServiceId()][offeredQoSGroup] = SubscriptionState::WAITING_FOR_SUBACK;
                // Find the appropriate connector to the QoSGroup
                for (SubscriberConnector* subscriberConnector: subscriberConnectors) {
                    std::vector<ServiceBase*> subscriberApplications = subscriberConnector->getApplications();
                    if (subscriberApplications.empty()) {
                        throw cRuntimeError("Empty subscriber connector without applications!.");
                    }
                    Subscriber* subscriberApplication = nullptr;
                    if (!(subscriberApplication = dynamic_cast<Subscriber*>(subscriberApplications[0]))) {
                        throw cRuntimeError("The subscriber application must be of the type Subscriber.");
                    }
                    if (subscriberApplication->getQoSGroup() == offeredQoSGroup) {
                        SomeIpDiscoveryNotification* someIpdiscoveryNotificationSubscription = new SomeIpDiscoveryNotification(someIpdiscoveryNotificationOffer->getServiceId(),
                                someIpdiscoveryNotificationOffer->getAddress(), someIpdiscoveryNotificationOffer->getInstanceId(), std::set<QoSGroup>{}, offeredQoSGroup, subscriberConnector->getTcpPort(),
                                subscriberConnector->getUdpPort()
                        );
                        emit(_subscribeSignal, someIpdiscoveryNotificationSubscription);
                        // Break here because there can be only one connector for a service ID and QoS Group pair
                        break;
                    }
                }
            }
        }
    }
    delete obj;
}

// Publisher-side
void SomeIpManager::acknowledgeSubscription(cObject* obj) {
    SomeIpDiscoveryNotification* someIpDiscoveryNotification = nullptr;
    if (!(someIpDiscoveryNotification = dynamic_cast<SomeIpDiscoveryNotification*>(obj))) {
        throw cRuntimeError("The discovery notification must be of type SomeIpDiscoveryNotification");
    }

    if (PublisherConnector* publisherConnector = _lsr->getPublisherConnector(someIpDiscoveryNotification->getServiceId())) {
        Publisher* publisher = nullptr;
        if ((publisher = dynamic_cast<Publisher*>(publisherConnector->getApplication()))) {
            throw cRuntimeError("The application must not null and must be of type Publisher");
        }
        if (std::find(publisher->getQoSGroups().begin(), publisher->getQoSGroups().end(), someIpDiscoveryNotification->getQoSGroup()) != publisher->getQoSGroups().end()) {
            SomeIpDiscoveryNotification* someIpDiscoveryNotificationAcknowledge = new SomeIpDiscoveryNotification(someIpDiscoveryNotification->getServiceId(), someIpDiscoveryNotification->getAddress(),
                    publisher->getInstanceId(), std::set<QoSGroup>{}, someIpDiscoveryNotification->getQoSGroup(), publisher->getTcpPort(), publisher->getUdpPort());
            switch (someIpDiscoveryNotification->getQoSGroup()) {
                case QoSGroup::SOMEIP_TCP: {
                    PublisherEndpointBase* publisherEndpointBase = createOrFindPublisherEndpoint(someIpDiscoveryNotification->getServiceId(), QoSGroup::SOMEIP_TCP);
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
                    PublisherEndpointBase* publisherEndpointBase = createOrFindPublisherEndpoint(someIpDiscoveryNotification->getServiceId(),QoSGroup::SOMEIP_UDP);
                    if(!publisherEndpointBase) {
                        throw cRuntimeError("No SOME/IP UDP Publisher was created.");
                    }
                    SOMEIPUDPPublisherEndpoint* someipUdpPublisherEndpoint = dynamic_cast<SOMEIPUDPPublisherEndpoint*>(publisherEndpointBase);
                    if(!(someipUdpPublisherEndpoint)) {
                        throw cRuntimeError("Endpoint is no SOMEIPUDPPublisherEndpoint.");
                    }
                    CSI_SOMEIP_UDP* csi_udp_someip = new CSI_SOMEIP_UDP();
                    csi_udp_someip->setAddress(someIpDiscoveryNotification->getAddress().str().c_str());
                    csi_udp_someip->setPort(someIpDiscoveryNotification->getUdpPort());
                    someipUdpPublisherEndpoint->addRemote(csi_udp_someip);
                    delete csi_udp_someip;
                    break;
                }
                default:
                    throw cRuntimeError("Unknown QoS group");
            }
            emit(_subscribeAckSignal,someIpDiscoveryNotificationAcknowledge);
        }
    }
    delete obj;
}

// Subscriber-side
void SomeIpManager::createSubscriberEndpoint(SomeIpDiscoveryNotification* someIpDiscoveryNotification, QoSGroup qosGroup) {
    SubscriberEndpointBase* sub = nullptr;
    switch (qosGroup) {
        case QoSGroup::SOMEIP_TCP: {
            CSI_SOMEIP_TCP* csi_someip_tcp = new CSI_SOMEIP_TCP();
            csi_someip_tcp->setAddress(someIpDiscoveryNotification->getAddress().str().c_str());
            csi_someip_tcp->setPort(someIpDiscoveryNotification->getTcpPort());
            sub = createOrFindSubscriberEndpoint(someIpDiscoveryNotification->getServiceId(), csi_someip_tcp);
            delete csi_someip_tcp;
            break;
        }
        case QoSGroup::SOMEIP_UDP: {
            CSI_SOMEIP_UDP* csi_someip_udp = new CSI_SOMEIP_UDP();
            csi_someip_udp->setAddress(someIpDiscoveryNotification->getAddress().str().c_str());
            csi_someip_udp->setPort(someIpDiscoveryNotification->getUdpPort());
            sub = createOrFindSubscriberEndpoint(someIpDiscoveryNotification->getServiceId(), csi_someip_udp);
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

} /* end namespace SOA4CoRE */
