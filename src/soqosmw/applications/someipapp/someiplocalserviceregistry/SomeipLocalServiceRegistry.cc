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

void SomeipLocalServiceRegistry::initialize() {
    _serviceIDToPublisher = new std::map<uint16_t,std::list<SomeipPublisher*>*>();
    _serviceIDToSubscriber = new std::map<uint16_t,std::list<SomeipSubscriber*>*>();
}

void SomeipLocalServiceRegistry::handleMessage(cMessage *msg) {
}

void SomeipLocalServiceRegistry::registerPublisherService(SomeipPublisher *someipPublisher) {
    auto it = _serviceIDToPublisher->find(someipPublisher->getPublishServiceID());
    if (it != _serviceIDToPublisher->end()) {
        std::list<SomeipPublisher*> *publishers = it->second;
        publishers->push_back(someipPublisher);
    } else {
        std::list<SomeipPublisher*> *publisherList = new std::list<SomeipPublisher*>();
        publisherList->push_back(someipPublisher);
        (*_serviceIDToPublisher)[someipPublisher->getPublishServiceID()] = publisherList;
    }
}

void SomeipLocalServiceRegistry::registerSubscriberService(SomeipSubscriber *someipSubscriber) {
    auto it = _serviceIDToSubscriber->find(someipSubscriber->getSubscribeServiceID());
    if (it != _serviceIDToSubscriber->end()) {
        std::list<SomeipSubscriber*> *subscribers = it->second;
        subscribers->push_back(someipSubscriber);
    } else {
        std::list<SomeipSubscriber*> *subscriberList = new std::list<SomeipSubscriber*>();
        subscriberList->push_back(someipSubscriber);
        (*_serviceIDToSubscriber)[someipSubscriber->getSubscribeServiceID()] = subscriberList;
    }
}

}
