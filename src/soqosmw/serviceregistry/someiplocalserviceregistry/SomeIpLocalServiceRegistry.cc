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
#include "soqosmw/applications/someipapp/base/ISomeIpServiceApp.h"
#include <soqosmw/serviceregistry/someiplocalserviceregistry/SomeIpLocalServiceRegistry.h>

namespace SOQoSMW {
Define_Module(SomeIpLocalServiceRegistry);


void SomeIpLocalServiceRegistry::initialize(int stage) {
    if (stage == inet::INITSTAGE_LOCAL) {
    }
}

void SomeIpLocalServiceRegistry::handleMessage(cMessage *msg) {
    //Does nothing here
}

void SomeIpLocalServiceRegistry::registerService(std::map<uint16_t,std::list<ISomeIpServiceApp*>>& someIpServiceMap, ISomeIpServiceApp* someIpService) {
    auto it = someIpServiceMap.find(someIpService->getServiceID());
    if (it != someIpServiceMap.end()) {
        std::list<ISomeIpServiceApp*> &services = it->second;
        services.push_back(someIpService);
    } else {
        std::list<ISomeIpServiceApp*> serviceList;
        serviceList.push_back(someIpService);
        someIpServiceMap[someIpService->getServiceID()] = serviceList;
    }
}

std::list<ISomeIpServiceApp*> SomeIpLocalServiceRegistry::getServices(const std::map<uint16_t,std::list<ISomeIpServiceApp*>>& someIpServiceMap, uint16_t serviceID) {
    std::list<ISomeIpServiceApp*> serviceList;
    auto it = someIpServiceMap.find(serviceID);
    if (it != someIpServiceMap.end()) {
        serviceList = it->second;
    }
    return serviceList;
}

void SomeIpLocalServiceRegistry::registerPublisherService(ISomeIpServiceApp *someIpPublisher) {
    registerService(_serviceIDToPublisher, someIpPublisher);
}

void SomeIpLocalServiceRegistry::registerSubscriberService(ISomeIpServiceApp *someIpSubscriber) {
    registerService(_serviceIDToSubscriber, someIpSubscriber);
}

std::list<ISomeIpServiceApp*> SomeIpLocalServiceRegistry::getPublisherService(uint16_t serviceID) {
    return getServices(_serviceIDToPublisher, serviceID);
}

std::list<ISomeIpServiceApp*> SomeIpLocalServiceRegistry::getSubscriberService(uint16_t serviceID) {
    return getServices(_serviceIDToSubscriber, serviceID);
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

std::list<std::pair<inet::L3Address,uint16_t>> SomeIpLocalServiceRegistry::getRemotePublisherEndpoints(uint16_t serviceID) {
    std::list<std::pair<inet::L3Address,uint16_t>> publisherInfoList;
    auto it = _remoteServiceIDToPublisher.find(serviceID);
    if (it != _remoteServiceIDToPublisher.end()) {
        publisherInfoList = it->second;
    }
    return publisherInfoList;
}

}
