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

#include "soqosmw/applications/publisherapp/someip/SomeIpPublisher.h"
#include "soqosmw/applications/subscriberapp/someip/SomeIpSubscriber.h"
#include "soqosmw/servicemanager/someipservicemanager/SomeIpLocalServiceManager.h"
#include "soqosmw/servicemanager/someipservicemanager/SubscriptionRelations.h"
#include "soqosmw/discovery/someipservicediscovery/SomeIpSDHeaderContainer.h"
#include "soqosmw/discovery/someipservicediscovery/SomeIpSDSubscriptionInformation.h"

namespace SOQoSMW {

Define_Module(SomeIpLocalServiceManager);

void SomeIpLocalServiceManager::initialize(int stage) {
    LocalServiceManager::initialize(stage);
    if (stage == inet::INITSTAGE_APPLICATION_LAYER) {
        if (!(_sd = dynamic_cast<IServiceDiscovery*>(getParentModule()->getSubmodule(
                   par("sdmoduleName"))))) {
            throw cRuntimeError("No IServiceDiscovery found.");
        }
        if(SomeIpSD* sd = dynamic_cast<SomeIpSD*>(_sd)) {
            sd->subscribe("serviceFoundSignal",this);
            sd->subscribe("serviceFindSignal",this);
            sd->subscribe("serviceOfferSignal",this);
            sd->subscribe("subscribeEventGroupSignal",this);
            sd->subscribe("subscribeEventGroupAckSignal",this);
        }

        _findResultSignal = omnetpp::cComponent::registerSignal("findResultSignal");
        _subscribeSignal = omnetpp::cComponent::registerSignal("subscribeSignal");
        _subscribeAckSignal = omnetpp::cComponent::registerSignal("subscribeAckSignal");
    }
}

void SomeIpLocalServiceManager::handleMessage(cMessage *msg) {
    // Does nothing here
}


void SomeIpLocalServiceManager::processAcknowledgedSubscription(cObject* obj) {
    SomeIpService& someIpService = *dynamic_cast<SomeIpService*>(obj);
    _pendingOffersMap.erase(someIpService.getServiceId());
}

void SomeIpLocalServiceManager::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj, cObject *details) {
    if (!strcmp(getSignalName(signalID),"serviceFindSignal")) {
        lookForService(obj);
    } else if(!strcmp(getSignalName(signalID),"serviceOfferSignal")) {
        addToPendingOffersAndSubscribe(obj);
    } else if(!strcmp(getSignalName(signalID),"subscribeEventGroupSignal")){
        acknowledgeSubscription(obj);
    } else if (!strcmp(getSignalName(signalID),"subscribeEventGroupAckSignal")) {
        processAcknowledgedSubscription(obj);
    } else {
        LocalServiceManager::receiveSignal(source, signalID, obj, details);
    }
}

void SomeIpLocalServiceManager::lookForService(cObject* obj) {
    SomeIpSDHeaderContainer* someIpSDHeaderContainer = dynamic_cast<SomeIpSDHeaderContainer*>(obj);
    ServiceIdentifier serviceIdentifier = ServiceIdentifier(someIpSDHeaderContainer->getSomeIpSdEntry().getServiceID());
    if (IService* service = _lsr->getService(serviceIdentifier)) {
        someIpSDHeaderContainer->setService(service);
        emit(_findResultSignal,someIpSDHeaderContainer);
    }
}

void SomeIpLocalServiceManager::addToPendingOffersAndSubscribe(cObject* obj) {
    SomeIpService& someIpService = *dynamic_cast<SomeIpService*>(obj);
    if (_pendingOffersMap.count(someIpService.getServiceId())) {
        _pendingOffersMap[someIpService.getServiceId()].push_back(someIpService);
    } else {
        std::list<SomeIpService> someIpservices;
        someIpservices.push_back(someIpService);
        _pendingOffersMap[someIpService.getServiceId()] = someIpservices;
    }

    if (_pendingRequestsMap.count(someIpService.getServiceId())) {
        for(std::list<QoSService>::const_iterator it = _pendingRequestsMap[someIpService.getServiceId()].begin(); it != _pendingRequestsMap[someIpService.getServiceId()].end(); ++it){
            SomeIpSDSubscriptionInformation someIpSDSubscriptionInformation = SomeIpSDSubscriptionInformation(
                    someIpService.getServiceId(), someIpService.getInstanceId(), it->getAddress(), it->getPort(), someIpService.getAddress());
            emit(_subscribeSignal,&someIpSDSubscriptionInformation);
        }
    }
}

void SomeIpLocalServiceManager::acknowledgeSubscription(cObject* obj) {
    SomeIpService& someIpService = *dynamic_cast<SomeIpService*>(obj);
    // Possible checks before final acknowledge ...
    ServiceIdentifier serviceIdentifier = ServiceIdentifier(someIpService.getServiceId());
    if (IService* service = _lsr->getService(serviceIdentifier)) {
        SomeIpSDSubscriptionInformation someIpSDSubscriptionInformation = SomeIpSDSubscriptionInformation(someIpService.getServiceId(),someIpService.getInstanceId(),service->getAddress(),service->getPort(),someIpService.getAddress());
        emit(_subscribeAckSignal,&someIpSDSubscriptionInformation);
    }

}

} /* end namespace SOQoSMW */
