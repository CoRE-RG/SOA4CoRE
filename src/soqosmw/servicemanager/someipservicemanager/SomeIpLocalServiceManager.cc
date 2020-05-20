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

#include <soqosmw/applications/publisherapp/someip/SomeIpPublisher.h>
#include <soqosmw/applications/subscriberapp/someip/SomeIpSubscriber.h>
#include <soqosmw/servicemanager/someipservicemanager/SomeIpLocalServiceManager.h>
#include <soqosmw/servicemanager/someipservicemanager/SubscriptionRelations.h>

namespace SOQoSMW {

Define_Module(SomeIpLocalServiceManager);

void SomeIpLocalServiceManager::initialize(int stage) {
    if (stage == inet::INITSTAGE_LOCAL) {
        cModule* module = getParentModule()->getSubmodule("sd");
        if((_someIpSD = dynamic_cast<SomeIpSD*>(module))){
        } else {
            throw cRuntimeError("No SOME/IP service discovery found.");
        }
        module = getParentModule()->getSubmodule("lsr");
        if((_someIpLSR = dynamic_cast<SomeIpLocalServiceRegistry*>(module))){
        } else {
            throw cRuntimeError("No SOME/IP local service registry found.");
        }
    }
}

void SomeIpLocalServiceManager::handleMessage(cMessage *msg) {
    // Does nothing here
}

void SomeIpLocalServiceManager::registerPublisherService(SomeIpPublisher *someIpPublisher) {
    _someIpLSR->registerPublisherService(someIpPublisher);
}

void SomeIpLocalServiceManager::registerSubscriberService(SomeIpSubscriber *someIpSubscriber) {
    _someIpLSR->registerSubscriberService(someIpSubscriber);
}

void SomeIpLocalServiceManager::discoverService(SomeIpSubscriber* someIpSubscriber) {
    SubscriptionRelations relations = SubscriptionRelations();
    // Look local for publisher service
    std::list<ISomeIpServiceApp*> publisherList = _someIpLSR->getPublisherService(someIpSubscriber->getServiceID());
    if (!publisherList.empty()) {
        for (ISomeIpServiceApp *someIpPublisher : publisherList) {
            dynamic_cast<SomeIpPublisher*>(someIpPublisher)->addSomeIpSubscriberDestinationInformartion(someIpSubscriber->getIpAddress(inet::L3Address::IPv4), someIpSubscriber->getPort());
            relations.addLocalSubscriptionRelation(dynamic_cast<SomeIpPublisher*>(someIpPublisher), SubscriptionState::SUBSCRIBED);
        }
    } else {
        // Look for already known remote publishers
        std::list<std::pair<inet::L3Address,uint16_t>> remotePublisherInfoList = _someIpLSR->getRemotePublisherEndpoints(someIpSubscriber->getServiceID());
        if (!remotePublisherInfoList.empty()) {
            for (std::pair<inet::L3Address,uint16_t> publisherInfo : remotePublisherInfoList) {
                _someIpSD->subscribeService(someIpSubscriber->getServiceID(), someIpSubscriber->getInstanceID(), publisherInfo.first, someIpSubscriber->getIpAddress(inet::L3Address::IPv4), someIpSubscriber->getPort());
                relations.addRemoteSubscriptionRelation(publisherInfo, SubscriptionState::SUBSCRIBING);
            }
        } else {
            // No publisher known locally or remotely
            _someIpSD->findService(someIpSubscriber->getServiceID(), someIpSubscriber->getInstanceID());
        }
    }
    if(someIpSubscriber->getServiceID() == 0x0001 || someIpSubscriber->getServiceID() == 0x0005) {

    }
    _subscriptionRelations[someIpSubscriber->getServiceID()].insert({someIpSubscriber, relations});
}

std::list<ISomeIpServiceApp*> SomeIpLocalServiceManager::lookLocalForPublisherService(uint16_t serviceID) {
    return _someIpLSR->getPublisherService(serviceID);
}

void SomeIpLocalServiceManager::addRemotePublisher(uint16_t serviceID, inet::L3Address publisherIP, uint16_t publisherPort) {
    _someIpLSR->registerRemotePublisherService(serviceID, publisherIP, publisherPort);
    auto it = _subscriptionRelations.find(serviceID);
    if (it != _subscriptionRelations.end()) {
        for (auto &relations : it->second) {
            SubscriptionState subscriptionState = relations.second.getRemoteRelation(std::make_pair(publisherIP, publisherPort));
            if ( subscriptionState == SubscriptionState::NOT_SUBSCRIBED) {
                _someIpSD->subscribeService(serviceID, relations.first->getInstanceID(), publisherIP, relations.first->getIpAddress(inet::L3Address::AddressType::IPv4), relations.first->getPort());
                relations.second.addRemoteSubscriptionRelation(std::make_pair(publisherIP, publisherPort), SubscriptionState::SUBSCRIBING);
            }
        }
    }
}

void SomeIpLocalServiceManager::publishToSubscriber(uint16_t serviceID, inet::L3Address subscriberIP, uint16_t subscriberPort) {
    std::list<ISomeIpServiceApp*> publisherList = _someIpLSR->getPublisherService(serviceID);
    if (!publisherList.empty()) {
        for (ISomeIpServiceApp *someIpPublisher : publisherList) {
            dynamic_cast<SomeIpPublisher*>(someIpPublisher)->addSomeIpSubscriberDestinationInformartion(subscriberIP, subscriberPort);
            _someIpSD->acknowledgeSubscription(someIpPublisher, subscriberIP);
        }
    }
}

void SomeIpLocalServiceManager::acknowledgeService(uint16_t serviceID, inet::L3Address publisherIp, uint16_t publisherPort) {
    auto it = _subscriptionRelations.find(serviceID);
    if (it != _subscriptionRelations.end()) {
        for (auto &relations : it->second) {
            if (relations.second.getRemoteRelation(std::make_pair(publisherIp, publisherPort)) == SubscriptionState::SUBSCRIBING) {
                relations.second.addRemoteSubscriptionRelation(std::make_pair(publisherIp, publisherPort), SubscriptionState::SUBSCRIBED);
            }
        }
    }
}

}
