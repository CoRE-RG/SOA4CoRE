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

#include <soqosmw/applications/someipapp/someipservicemanager/SomeipLocalServiceManager.h>
#include <soqosmw/applications/someipapp/someipSDTestApp/publisherapp/SomeipPublisher.h>
#include <soqosmw/applications/someipapp/someipSDTestApp/subscriberapp/SomeipSubscriber.h>

namespace SOQoSMW {

Define_Module(SomeipLocalServiceManager);

void SomeipLocalServiceManager::initialize(int stage) {
    SomeipAppBase::initialize(stage);
    if (stage == inet::INITSTAGE_LOCAL) {
        cModule* module = getParentModule()->getSubmodule("udpApp", SOMEIPSDIDX);
        if((_someipSD = dynamic_cast<SomeipSD*>(module))){
        } else {
            throw cRuntimeError("Submodule at index %d is no Someip SD app."
                    "Please place the SomeipServiceDiscovery at index %d", SOMEIPSDIDX);
        }
        module = getParentModule()->getSubmodule("udpApp", SOMEIPLOCALSERVICEREGISTRYIDX);
        if((_someipLSR = dynamic_cast<SomeipLocalServiceRegistry*>(module))){
        } else {
            throw cRuntimeError("Submodule at index %d is no Someip LSR app."
                    "Please place the SomeipLocalServiceRegistry at index %d", SOMEIPLOCALSERVICEREGISTRYIDX);
        }
    }
}

void SomeipLocalServiceManager::handleMessageWhenUp(cMessage *msg) {
    // Does nothing here
}

void SomeipLocalServiceManager::registerPublisherService(SomeipPublisher *someipPublisher) {
    _someipLSR->registerPublisherService(someipPublisher);
}

void SomeipLocalServiceManager::registerSubscriberService(SomeipSubscriber *someipSubscriber) {
    _someipLSR->registerSubscriberService(someipSubscriber);
}

void SomeipLocalServiceManager::discoverService(uint16_t serviceID, uint16_t instanceID, inet::L3Address subscriberIP, uint16_t subscriberPort) {
    std::list<SomeipPublisher*> publisherList = _someipLSR->getPublisherService(serviceID);
    if (!publisherList.empty()) {
        for (SomeipPublisher *someipPublisher : publisherList) {
            someipPublisher->addSomeipSubscriberDestinationInformartion(subscriberIP, subscriberPort);
        }
    } else {
        std::list<std::pair<inet::L3Address,uint16_t>> remotePublisherInfoList = _someipLSR->getRemotePublisherInfoList(serviceID);
        if (!remotePublisherInfoList.empty()) {
            for (std::pair<inet::L3Address,uint16_t> publisherInfo : remotePublisherInfoList) {
                _someipSD->subscribeService(serviceID, instanceID, publisherInfo.first, subscriberIP, subscriberPort);
            }
        } else {
            _someipSD->findService(serviceID, instanceID);
        }
        _pendingRequests[serviceID] = std::make_pair(subscriberIP, subscriberPort);
    }
}

std::list<SomeipPublisher*> SomeipLocalServiceManager::lookForPublisherService(uint16_t serviceID) {
    return _someipLSR->getPublisherService(serviceID);
}

void SomeipLocalServiceManager::addRemotePublisher(uint16_t serviceID, inet::L3Address publisherIP, uint16_t publisherPort) {
    _someipLSR->registerRemotePublisherService(serviceID, publisherIP, publisherPort);
    _someipSD->subscribeService(serviceID, 0xFFFF, publisherIP, _pendingRequests[serviceID].first, _pendingRequests[serviceID].second);
}

void SomeipLocalServiceManager::publishToSubscriber(uint16_t serviceID, inet::L3Address subscriberIP, uint16_t subscriberPort) {
    std::list<SomeipPublisher*> publisherList = _someipLSR->getPublisherService(serviceID);
    if (!publisherList.empty()) {
        for (SomeipPublisher *someipPublisher : publisherList) {
            someipPublisher->addSomeipSubscriberDestinationInformartion(subscriberIP, subscriberPort);
        }
    }
    _someipSD->acknowledgeSubscription(serviceID, 0xFFFF, subscriberIP);
}

void SomeipLocalServiceManager::acknowledgeService(uint16_t serviceID) {
    _pendingRequests.erase(serviceID);
}

}
