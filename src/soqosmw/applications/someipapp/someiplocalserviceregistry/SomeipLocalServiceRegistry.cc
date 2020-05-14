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

#include <soqosmw/applications/someipapp/someiplocalserviceregistry/SomeipLocalServiceRegistry.h>
#include "soqosmw/applications/someipapp/someipSDTestApp/publisherapp/SomeipPublisher.h"
#include "soqosmw/applications/someipapp/someipSDTestApp/subscriberapp/SomeipSubscriber.h"

namespace SOQoSMW {
Define_Module(SomeipLocalServiceRegistry);

void SomeipLocalServiceRegistry::initialize(int stage) {
    SomeipAppBase::initialize(stage);
    if (stage == inet::INITSTAGE_LOCAL) {
    }
}

void SomeipLocalServiceRegistry::handleMessageWhenUp(cMessage *msg) {
    //Does nothing here
}

void SomeipLocalServiceRegistry::registerPublisherService(SomeipPublisher *someipPublisher) {
    auto it = _serviceIDToPublisher.find(someipPublisher->getPublishServiceID());
    if (it != _serviceIDToPublisher.end()) {
        std::list<SomeipPublisher*> publishers = it->second;
        publishers.push_back(someipPublisher);
    } else {
        std::list<SomeipPublisher*> publisherList;
        publisherList.push_back(someipPublisher);
        _serviceIDToPublisher[someipPublisher->getPublishServiceID()] = publisherList;
    }
}

void SomeipLocalServiceRegistry::registerSubscriberService(SomeipSubscriber *someipSubscriber) {
    auto it = _serviceIDToSubscriber.find(someipSubscriber->getSubscribeServiceID());
    if (it != _serviceIDToSubscriber.end()) {
        std::list<SomeipSubscriber*> subscribers = it->second;
        subscribers.push_back(someipSubscriber);
    } else {
        std::list<SomeipSubscriber*> subscriberList;
        subscriberList.push_back(someipSubscriber);
        _serviceIDToSubscriber[someipSubscriber->getSubscribeServiceID()] = subscriberList;
    }
}

std::list<SomeipPublisher*> SomeipLocalServiceRegistry::getPublisherService(uint16_t serviceID) {
    std::list<SomeipPublisher*> publisherList;
    auto it = _serviceIDToPublisher.find(serviceID);
    if (it != _serviceIDToPublisher.end()) {
        publisherList = it->second;
    }
    return publisherList ;
}

std::list<SomeipSubscriber*> SomeipLocalServiceRegistry::getSubscriberService(uint16_t serviceID) {
    std::list<SomeipSubscriber*> subscriberList;
    auto it = _serviceIDToSubscriber.find(serviceID);
    if (it != _serviceIDToSubscriber.end()) {
        subscriberList = it->second;
    }
    return subscriberList ;
}

void SomeipLocalServiceRegistry::registerRemotePublisherService(uint16_t serviceID, inet::L3Address publisherIP, uint16_t publisherPort) {
    auto it = _remoteServiceIDToPublisher.find(serviceID);
    if (it != _remoteServiceIDToPublisher.end()) {
        std::list<std::pair<inet::L3Address,uint16_t>> remotePublisherInfo = it->second;
        remotePublisherInfo.push_back(std::make_pair(publisherIP,publisherPort));
    } else {
        std::list<std::pair<inet::L3Address,uint16_t>> remotePublisherInfo;
        remotePublisherInfo.push_back(std::make_pair(publisherIP,publisherPort));
        _remoteServiceIDToPublisher[serviceID] = remotePublisherInfo;
    }
}

std::list<std::pair<inet::L3Address,uint16_t>> SomeipLocalServiceRegistry::getRemotePublisherInfoList(uint16_t serviceID) {
    std::list<std::pair<inet::L3Address,uint16_t>> publisherInfoList;
    auto it = _remoteServiceIDToPublisher.find(serviceID);
    if (it != _remoteServiceIDToPublisher.end()) {
        publisherInfoList = it->second;
    }
    return publisherInfoList;
}

}
