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

#include "QoSLocalServiceManager.h"
#include <inet/networklayer/common/L3Address.h>
#include <soqosmw/discovery/someipservicediscovery/SomeIpSD.h>
#include <algorithm>

namespace SOQoSMW {

Define_Module(QoSLocalServiceManager);

void QoSLocalServiceManager::initialize(int stage)
{
    LocalServiceManager::initialize(stage);
    if (stage == inet::INITSTAGE_APPLICATION_LAYER) {
        if(!(_qosnp =
               dynamic_cast<QoSNegotiationProtocol*>(getParentModule()->getSubmodule(
                       par("qosnpmoduleName"))))) {
            throw cRuntimeError("No QoSNegotiationProtocol module found.");
        }
        if (!(_sd = dynamic_cast<IServiceDiscovery*>(getParentModule()->getSubmodule(par("sdmoduleName"))))) {
            throw cRuntimeError("No IServiceDiscovery found.");
        }
        if (dynamic_cast<SomeIpSD*>(_sd)) {
            throw cRuntimeError("QoSLocalServiceManager does not work with SOME/IP ServiceDiscovery.");
        }
        if (cSimpleModule* sd = dynamic_cast<cSimpleModule*>(_sd)) {
            sd->subscribe("serviceOfferSignal",this);
        } else {
            throw cRuntimeError("Service discovery serviceOfferSignal could not be subscribed.");
        }

    }
}

void QoSLocalServiceManager::handleMessage(cMessage *msg) {
    // Nothing to do
}

void QoSLocalServiceManager::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj, cObject *details) {
    if (!strcmp(getSignalName(signalID),"serviceOfferSignal")) {
        subscribeOfferedService(obj);
    } else {
        throw cRuntimeError("Unknown signal.");
    }
}

void QoSLocalServiceManager::subscribeOfferedService(cObject* obj) {
    if (QoSService* offeredService = dynamic_cast<QoSService*>(obj)) {
        for (QoSService qosService : _pendingRequestsMap[offeredService->getServiceId()]) {
            if (QoSGroups* commonQoSGroup = offeredService->getCommonQoSGroup(qosService)) {
                QoSService publisherService = getQoSServiceCopyWithGivenQoSGroupOnly(*offeredService, *commonQoSGroup);
                delete commonQoSGroup;
                Request* request = createNegotiationRequest(publisherService);
                //create qos broker for the request
                _qosnp->createQoSBroker(request);
                _pendingRequestsMap[qosService.getServiceId()].remove(qosService);
            }
        }
        if(!_pendingRequestsMap[offeredService->getServiceId()].size()) {
            _pendingRequestsMap.erase(offeredService->getServiceId());
        }
        _lsr->addPublisherService(*offeredService);
        delete offeredService;
    } else {
        throw cRuntimeError("Given object is not type of QoSService.");
    }
}

void QoSLocalServiceManager::subscribeService(QoSServiceIdentifier publisherServiceIdentifier, QoSService qosService) {
    //check if publisher is already discovered, and if so, start the negotiation with a request.
    bool serviceFound = false;
    if (_lsr->containsService(publisherServiceIdentifier)) {
        QoSService publisherService = _lsr->getService(publisherServiceIdentifier);
        if (QoSGroups* commonQoSGroup = qosService.getCommonQoSGroup(publisherService)) {
            publisherService = getQoSServiceCopyWithGivenQoSGroupOnly(publisherService, *commonQoSGroup);
            delete commonQoSGroup;
            Request* request = createNegotiationRequest(publisherService);
            //create qos broker for the request
            _qosnp->createQoSBroker(request);
            serviceFound = true;
        }
    }
    if (!serviceFound) {
        LocalServiceManager::addServiceToPendingRequestsMap(publisherServiceIdentifier, qosService);
        _sd->discover(publisherServiceIdentifier);
    }
}

Request* QoSLocalServiceManager::createNegotiationRequest(QoSService publisherService) {
    EndpointDescription subscriber(publisherService.getServiceId(), _localAddress, _qosnp->getProtocolPort());
    EndpointDescription publisher(publisherService.getServiceId(), publisherService.getAddress(), _qosnp->getProtocolPort());
    Request *request = new Request(_requestID++, subscriber, publisher, publisherService, nullptr);
    return request;
}

QoSService QoSLocalServiceManager::getQoSServiceCopyWithGivenQoSGroupOnly(QoSService qosService, QoSGroups qosGroup) {
    std::set<QoSGroups> qosGroups;
    qosGroups.insert(qosGroup);
    return QoSService(qosService.getServiceId(), qosService.getAddress(),
            qosService.getInstanceId(), qosGroups, qosService.getTCPPort(), qosService.getUDPPort(),
            qosService.getStreamId(), qosService.getSrClass(), qosService.getFramesize(),
            qosService.getIntervalFrames());
}

} /* end namespace SOQoSMW */
