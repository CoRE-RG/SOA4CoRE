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

void SomeIpLocalServiceManager::discoverService(uint16_t serviceID, uint16_t instanceID, inet::L3Address subscriberIP, uint16_t subscriberPort) {
    std::list<SomeIpPublisher*> publisherList = _someIpLSR->getPublisherService(serviceID);
    if (!publisherList.empty()) {
        for (SomeIpPublisher *someIpPublisher : publisherList) {
            someIpPublisher->addSomeipSubscriberDestinationInformartion(subscriberIP, subscriberPort);
        }
    } else {
        std::list<std::pair<inet::L3Address,uint16_t>> remotePublisherInfoList = _someIpLSR->getRemotePublisherInfoList(serviceID);
        if (!remotePublisherInfoList.empty()) {
            for (std::pair<inet::L3Address,uint16_t> publisherInfo : remotePublisherInfoList) {
                _someIpSD->subscribeService(serviceID, instanceID, publisherInfo.first, subscriberIP, subscriberPort);
            }
        } else {
            _someIpSD->findService(serviceID, instanceID);
        }
        _pendingRequests[serviceID] = std::make_pair(subscriberIP, subscriberPort);
    }
}

std::list<SomeIpPublisher*> SomeIpLocalServiceManager::lookLocalForPublisherService(uint16_t serviceID) {
    return _someIpLSR->getPublisherService(serviceID);
}

void SomeIpLocalServiceManager::addRemotePublisher(uint16_t serviceID, inet::L3Address publisherIP, uint16_t publisherPort) {
    _someIpLSR->registerRemotePublisherService(serviceID, publisherIP, publisherPort);
    //TODO An already subscribed publisher will be subscribed again, if the serviceID from another publisher is as the one subscribed before
    _someIpSD->subscribeService(serviceID, 0xFFFF, publisherIP, _pendingRequests[serviceID].first, _pendingRequests[serviceID].second);
}

void SomeIpLocalServiceManager::publishToSubscriber(uint16_t serviceID, inet::L3Address subscriberIP, uint16_t subscriberPort) {
    std::list<SomeIpPublisher*> publisherList = _someIpLSR->getPublisherService(serviceID);
    if (!publisherList.empty()) {
        for (SomeIpPublisher *someIpPublisher : publisherList) {
            someIpPublisher->addSomeipSubscriberDestinationInformartion(subscriberIP, subscriberPort);
        }
    }
    _someIpSD->acknowledgeSubscription(serviceID, 0xFFFF, subscriberIP);
}

void SomeIpLocalServiceManager::acknowledgeService(uint16_t serviceID) {
    _pendingRequests.erase(serviceID);
}

}
