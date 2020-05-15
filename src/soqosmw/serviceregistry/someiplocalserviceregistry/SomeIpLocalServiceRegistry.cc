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

#include <soqosmw/applications/publisherapp/someippublisherapp/SomeIpPublisher.h>
#include <soqosmw/applications/subscriberapp/someipsubscriberapp/SomeIpSubscriber.h>
#include <soqosmw/serviceregistry/someiplocalserviceregistry/SomeIpLocalServiceRegistry.h>

namespace SOQoSMW {
Define_Module(SomeIpLocalServiceRegistry);

SomeIpLocalServiceRegistry::SomeIpLocalServiceRegistry() {
    _serviceIDToPublisher = new std::map<uint16_t,std::list<SomeIpPublisher*>*>();
}

void SomeIpLocalServiceRegistry::initialize(int stage) {
    if (stage == inet::INITSTAGE_LOCAL) {
    }
}

void SomeIpLocalServiceRegistry::handleMessage(cMessage *msg) {
    //Does nothing here
}

void SomeIpLocalServiceRegistry::registerPublisherService(SomeIpPublisher *someIpPublisher) {
    size_t size = _serviceIDToPublisher->size();
    auto it = _serviceIDToPublisher->find(someIpPublisher->getPublishServiceID());
    if (it != _serviceIDToPublisher->end()) {
        std::list<SomeIpPublisher*> *publishers = it->second;
        publishers->push_back(someIpPublisher);
    } else {
        std::list<SomeIpPublisher*> *publisherList = new std::list<SomeIpPublisher*>();
        publisherList->push_back(someIpPublisher);
        _serviceIDToPublisher->insert({someIpPublisher->getPublishServiceID(),publisherList});
    }
}

void SomeIpLocalServiceRegistry::registerSubscriberService(SomeIpSubscriber *someIpSubscriber) {
    auto it = _serviceIDToSubscriber.find(someIpSubscriber->getSubscribeServiceID());
    if (it != _serviceIDToSubscriber.end()) {
        std::list<SomeIpSubscriber*> subscribers = it->second;
        subscribers.push_back(someIpSubscriber);
    } else {
        std::list<SomeIpSubscriber*> subscriberList;
        subscriberList.push_back(someIpSubscriber);
        _serviceIDToSubscriber[someIpSubscriber->getSubscribeServiceID()] = subscriberList;
    }
}

std::list<SomeIpPublisher*>* SomeIpLocalServiceRegistry::getPublisherService(uint16_t serviceID) {
    std::list<SomeIpPublisher*> *publisherList =  nullptr;
    auto it = _serviceIDToPublisher->find(serviceID);
    if (it != _serviceIDToPublisher->end()) {
        publisherList = it->second;
    }
    return publisherList;
}

std::list<SomeIpSubscriber*> SomeIpLocalServiceRegistry::getSubscriberService(uint16_t serviceID) {
    std::list<SomeIpSubscriber*> subscriberList;
    auto it = _serviceIDToSubscriber.find(serviceID);
    if (it != _serviceIDToSubscriber.end()) {
        subscriberList = it->second;
    }
    return subscriberList ;
}

void SomeIpLocalServiceRegistry::registerRemotePublisherService(uint16_t serviceID, inet::L3Address publisherIP, uint16_t publisherPort) {
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

std::list<std::pair<inet::L3Address,uint16_t>> SomeIpLocalServiceRegistry::getRemotePublisherInfoList(uint16_t serviceID) {
    std::list<std::pair<inet::L3Address,uint16_t>> publisherInfoList;
    auto it = _remoteServiceIDToPublisher.find(serviceID);
    if (it != _remoteServiceIDToPublisher.end()) {
        publisherInfoList = it->second;
    }
    return publisherInfoList;
}

}
