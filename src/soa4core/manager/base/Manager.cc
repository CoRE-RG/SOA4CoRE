 //
// c Timo Haeckel for HAW Hamburg
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

#include "soa4core/manager/base/Manager.h"
//INET
#include <inet/networklayer/common/L3AddressResolver.h>
//STD
#include <algorithm>
#include <cstring>
#include <iterator>
#include <stdio.h>
#include <string.h>


using namespace std;
using namespace inet;

namespace SOA4CoRE {

void Manager::initialize(int stage) {

    if (stage == INITSTAGE_APPLICATION_LAYER) {
        handleParameterChange(nullptr);
        _lsr =
               dynamic_cast<Registry*>(getParentModule()->getSubmodule(
                       par("srmoduleName")));
    }
}

void Manager::handleMessage(cMessage *msg) {
    throw cRuntimeError("Manager does not handle messages.");
}

void Manager::handleParameterChange(const char* parname) {
    if (!parname || !strcmp(parname, "localAddress")) {
        const char* localAddr = par("localAddress");
        _localAddress = L3AddressResolver().resolve(localAddr);
    }
    if (!parname || !strcmp(parname, "registerStreams"))
    {
        _registerStreams = par("registerStreams").boolValue();
    }
    if (!parname || !strcmp(parname, "advertiseStreamRegistration"))
    {
        _advertiseStreamRegistration = par("advertiseStreamRegistration").boolValue();
    }
}

PublisherConnector* Manager::registerPublisherService(ServiceBase* publisherApplication) {
    Enter_Method("LSM:registerPublisherService()");

    Publisher* publisherApplication_ = nullptr;
    if(!(publisherApplication_ = dynamic_cast<Publisher*>(publisherApplication))){
        throw cRuntimeError("Given publisher application must be of type Publisher");
    }
    if (_lsr->getPublisherConnector(publisherApplication_->getServiceId())) {
        throw cRuntimeError("There can not be multiple publisher services with same ID");
    }

    PublisherConnector* publisherConnector = createPublisherConnector(publisherApplication_);
    _lsr->addPublisherServiceConnector(publisherApplication_->getServiceId(), publisherConnector);
    return publisherConnector;
}

PublisherConnector* Manager::createPublisherConnector(Publisher* publisherApplication) {
    // create a connector for the publisher
    // 1. Find the factory object;
    cModuleType *moduleType = cModuleType::get("soa4core.connector.publisher.PublisherConnector");
    // 2. Create the module;
    int vectorsize = _lsr->getPublisherConnectorsCount();
    PublisherConnector *module = dynamic_cast<PublisherConnector*> (moduleType->create("publisherConnectors", this->getParentModule(), vectorsize + 1, vectorsize));
    // 3. Set up its parameters and gate sizes as needed;
    module->setApplication(publisherApplication);
    module->finalizeParameters();
    // 4. Tell the (possibly compound) module to recursively create its internal submodules and connections;
    module->buildInside();
    module->callInitialize();
    // 5. Schedule activation message(s) for the new simple module(s).
    module->scheduleStart(simTime());
    return module;
}

SubscriberConnector* Manager::registerSubscriberServiceAndSubscribeService(ServiceIdentifier publisherServiceIdentifier, ServiceBase* subscriberApplication)
{
    Enter_Method("LSM:registerSubscriberService()");
    Subscriber* subscriberApplication_ = nullptr;
    SubscriberConnector* validSubscriberConnector = nullptr;
    if(!(subscriberApplication_ = dynamic_cast<Subscriber*>(subscriberApplication))){
        throw cRuntimeError("Given subscriber application must be of type Subscriber");
    }
    std::list<SubscriberConnector*> subscriberConnectors = _lsr->getSubscriberConnectors(publisherServiceIdentifier.getServiceId());
    for (SubscriberConnector* subscriberConnector : subscriberConnectors) {
        std::vector<ServiceBase*> subscriberApplications = subscriberConnector->getApplications();
        Subscriber* connectedSubscriberApplication = nullptr;
        if (subscriberApplications.empty()) {
            throw cRuntimeError("Empty subscriber connector without applications!");
        }
        if (!(connectedSubscriberApplication = dynamic_cast<Subscriber*>(subscriberApplications[0]))) {
            throw cRuntimeError("The subscriber application must be of type Subscriber");
        }
        if (connectedSubscriberApplication->getQoSGroup() == subscriberApplication_->getQoSGroup()) {
            validSubscriberConnector = subscriberConnector;
            break;
        }
    }

    if (!validSubscriberConnector) {
        validSubscriberConnector = createSubscriberConnector(subscriberApplication_);
        _lsr->addSubscriberServiceConnector(publisherServiceIdentifier.getServiceId(), validSubscriberConnector);
        discoverService(publisherServiceIdentifier, subscriberApplication_);
    } else {
        addSubscriberServiceToConnector(validSubscriberConnector, subscriberApplication_);
    }
    return validSubscriberConnector;
}

void Manager::discoverService(ServiceIdentifier publisherServiceIdentifier, ServiceBase* subscriberApplication) {
    throw cRuntimeError("Manager has no implementation of discoverService. \n"
            "Use other Manager (e.g. QoSManager, SomeIpManager which implements discoverService");
}

void Manager::addSubscriberServiceToConnector(SubscriberConnector* subscriberConnector, ServiceBase* subscriberApplication) {
    if(!(subscriberConnector->addApplication(subscriberApplication))){
        throw cRuntimeError("This Subscriber service already exists on this host...");
    }
}

SubscriberConnector* Manager::createSubscriberConnector(ServiceBase* subscriberApplication) {
    // create a connector for the subscriber
    // 1. Find the factory object;
    cModuleType *moduleType = cModuleType::get("soa4core.connector.subscriber.SubscriberConnector");
    // 2. Create the module;
    int vectorsize  = _lsr->getSubscriberConnectorsCount();
    SubscriberConnector *module = dynamic_cast<SubscriberConnector*> (moduleType->create("subscriberConnectors", this->getParentModule(), vectorsize + 1, vectorsize));
    // 3. Set up its parameters and gate sizes as needed;
    module->setAddress(subscriberApplication->getAddress());
    module->setTcpPort(subscriberApplication->getTcpPort());
    module->setUdpPort(subscriberApplication->getUdpPort());
    module->addApplication(subscriberApplication);
    module->finalizeParameters();
    // 4. Tell the (possibly compound) module to recursively create its internal submodules and connections;
    module->buildInside();
    module->callInitialize();
    // 5. Schedule activation message(s) for the new simple module(s).
    module->scheduleStart(simTime());
    return module;
}

PublisherEndpointBase* Manager::createOrFindPublisherEndpoint(
        uint32_t publisherServiceId, QoSGroup qosGroup) {
    PublisherEndpointBase* pub = nullptr;
    pub = findPublisherEndpoint(publisherServiceId, qosGroup);
    if(!pub){
        PublisherConnector* publisherConnector = _lsr->getPublisherConnector(publisherServiceId);
        if(publisherConnector){
            pub = createQoSSpecificPublisherEndpoint(qosGroup, publisherConnector);
            pub->finalizeParameters();
            // 4. Tell the (possibly compound) module to recursively create its internal submodules and connections;
            pub->buildInside();
            pub->callInitialize();
            // 5. Schedule activation message(s) for the new simple module(s).
            pub->scheduleStart(simTime());
        } else {
            throw cRuntimeError("Found no connector for the requested publisher so it is not needed.");
        }
    }
    return pub;
}

SubscriberEndpointBase* Manager::createOrFindSubscriberEndpoint(
        uint32_t publisherServiceId, ConnectionSpecificInformation* csi) {
    SubscriberEndpointBase* sub = nullptr;
    QoSGroup qosGroup = getQoSGroupForConnectionType(ConnectionType(csi->getConnectionType()));
    sub = findSubscriberEndpoint(publisherServiceId, qosGroup);
    if(!sub){
        SubscriberConnector* subscriberConnector = findSubscriberConnector(
                publisherServiceId, qosGroup);
        if(subscriberConnector){
            sub = createConnectionSpecificSubscriberEndpoint(csi, subscriberConnector);
            sub->finalizeParameters();
            // 4. Tell the (possibly compound) module to recursively create its internal submodules and connections;
            sub->buildInside();
            sub->callInitialize();
            // 5. Schedule activation message(s) for the new simple module(s).
            sub->scheduleStart(simTime());
        } else {
            throw cRuntimeError("Found no connector for the requested subscriber so it is not needed.");
        }
    }
    return sub;
}

PublisherEndpointBase* Manager::findPublisherEndpoint(
        uint32_t publisherServiceId, QoSGroup qosGroup) {

    // find fitting publisher connector
    PublisherConnector* publisherConnector = _lsr->getPublisherConnector(publisherServiceId);
    PublisherEndpointBase* publisherEndpointBase = nullptr;
    if(publisherConnector){
        for (auto endpoint: publisherConnector->getEndpoints()){
            if(endpoint->getQos() == qosGroup){
                publisherEndpointBase = dynamic_cast<PublisherEndpointBase*>(endpoint);
                break;
            }
        }
    }

    return publisherEndpointBase;
}

SubscriberEndpointBase* Manager::findSubscriberEndpoint(
        uint32_t publisherServiceId, QoSGroup qosGroup) {

    SubscriberEndpointBase* subscriberEndpoint = nullptr;
    // find fitting subscriber connector
    std::list<SubscriberConnector*> subscriberConnectors = _lsr->getSubscriberConnectors(publisherServiceId);
    for (SubscriberConnector* subscriberConnector : subscriberConnectors) {
        std::vector<ServiceBase*> subscriberApplications = subscriberConnector->getApplications();
        if (subscriberApplications.empty()) {
            throw cRuntimeError("Found empty subscriber connector without subscriber applications.");
        }
        Subscriber* subscriberApplication = nullptr;
        if (!(subscriberApplication = dynamic_cast<Subscriber*>(subscriberApplications[0]))) {
            throw cRuntimeError("The subscriber application must be of the type Subscriber.");
        }

        if (subscriberApplication->getQoSGroup() == qosGroup && (subscriberEndpoint = subscriberConnector->getEndpoint())) {
            break;
        }
    }
    return subscriberEndpoint;
}

SubscriberConnector* Manager::findSubscriberConnector(uint32_t publisherServiceId,
        QoSGroup qosGroup) {
    SubscriberConnector* subscriberConnector = nullptr;
    std::list<SubscriberConnector*> subscriberConnectors =
            _lsr->getSubscriberConnectors(publisherServiceId);
    for (SubscriberConnector* subscriberConnector_ : subscriberConnectors) {
        std::vector<ServiceBase*> subscriberApplications =
                subscriberConnector_->getApplications();
        if (subscriberApplications.empty()) {
            throw cRuntimeError(
                    "Found empty subscriber connector without subscriber applications.");
        }
        Subscriber* subscriberApplication = nullptr;
        if (!(subscriberApplication =
                dynamic_cast<Subscriber*>(subscriberApplications[0]))) {
            throw cRuntimeError(
                    "The subscriber application must be of the type Subscriber.");
        }
        if (subscriberApplication->getQoSGroup() == qosGroup) {
            subscriberConnector = subscriberConnector_;
            break;
        }
    }
    return subscriberConnector;
}

} /* end namespace  */
