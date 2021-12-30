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

#include <soqosmw/service/qosserviceidentifier/QoSServiceIdentifier.h>
#include "soqosmw/discovery/someipservicediscovery/SomeIpSD.h"
#include "soqosmw/servicemanager/someipservicemanager/SomeIpLocalServiceManager.h"
#include "soqosmw/discovery/someipservicediscovery/SomeIpSDSubscriptionInformation.h"
#include "soqosmw/service/qosservice/QoSService.h"
#include <inet/networklayer/common/L3AddressResolver.h>
#include <inet/networklayer/contract/ipv4/IPv4Address.h>
#include <list>

namespace SOQoSMW {

#define MAJOR_VERSION 0xFF       // see [PRS_SOMEIPSD_00351],[PRS_SOMEIPSD_00356],[PRS_SOMEIPSD_00386],[PRS_SOMEIPSD_00391]
#define MINOR_VERSION 0xFFFFFFFF // see [PRS_SOMEIPSD_00351],[PRS_SOMEIPSD_00356],[PRS_SOMEIPSD_00386],[PRS_SOMEIPSD_00391]
#define TTL 0xFFFFFF             // see [PRS_SOMEIPSD_00351],[PRS_SOMEIPSD_00356],[PRS_SOMEIPSD_00386],[PRS_SOMEIPSD_00391]

Define_Module(SomeIpSD);

void SomeIpSD::initialize(int stage) {
    inet::ApplicationBase::initialize(stage);
    if (stage == inet::INITSTAGE_LOCAL) {
        gate("udpOut")->connectTo(getParentModule()->gate("sdUDPOut"));
        getParentModule()->gate("sdUDPIn")->connectTo(gate("udpIn"));
        numSent = 0;
        numReceived = 0;
        WATCH(numSent);
        WATCH(numReceived);

        localPort = par("localPort");
        destPort = par("destPort");
        startTime = par("startTime").doubleValue();
        stopTime = par("stopTime").doubleValue();
        packetName = par("packetName");
    }
    if (stage == inet::INITSTAGE_ROUTING_PROTOCOLS) {
        if (!(par("localAddress").stdstringValue().length())) {
            throw cRuntimeError("Please define a local ip address");
        }
        processStart();
        IServiceDiscovery::_serviceOfferSignal = omnetpp::cComponent::registerSignal("serviceOfferSignal");
        _serviceFindSignal = omnetpp::cComponent::registerSignal("serviceFindSignal");
        _subscribeEventGroupSignal = omnetpp::cComponent::registerSignal("subscribeEventGroupSignal");
        _subscribeEventGroupAckSignal = omnetpp::cComponent::registerSignal("subscribeEventGroupAckSignal");

        if (SomeIpLocalServiceManager* someIplocalServiceManager = dynamic_cast<SomeIpLocalServiceManager*>(getParentModule()->getSubmodule("lsm"))) {
            someIplocalServiceManager->subscribe("findResultSignal",this);
            someIplocalServiceManager->subscribe("subscribeSignal",this);
            someIplocalServiceManager->subscribe("subscribeAckSignal",this);
        } else {
            throw cRuntimeError("SomeIpLocalServiceManager is needed for SOME/IP Discovery.");
        }
    }
}

bool SomeIpSD::handleNodeStart(IDoneCallback *doneCallback)
{
    return true;
}

void SomeIpSD::handleMessageWhenUp(cMessage *msg) {
    if (SomeIpSDHeader *someIpSDHeader = dynamic_cast<SomeIpSDHeader*>(msg)) {
        if(inet::UDPDataIndication *udpDataIndication = dynamic_cast<inet::UDPDataIndication*>(someIpSDHeader->getControlInfo())) {
            if (udpDataIndication->getSrcAddr() != inet::L3AddressResolver().resolve(par("localAddress"))) {
                processSomeIpSDHeader(someIpSDHeader);
            }
        }
    }
    delete msg;
}

void SomeIpSD::find(uint16_t serviceID, uint16_t instanceID) {
    Enter_Method("SomeIpSD::find");
    SomeIpSDHeader *someIpSDHeader = new SomeIpSDHeader("SOME/IP SD - FIND");

    ServiceEntry *findEntry = new ServiceEntry("ServiceEntry");
    findEntry->setType(SOQoSMW::SomeIpSDEntryType::FIND);
    findEntry->setIndex1stOptions(0);
    findEntry->setIndex2ndOptions(0);
    findEntry->setNum1stOptions(0);
    findEntry->setNum2ndOptions(0);
    findEntry->setServiceID(serviceID);
    findEntry->setInstanceID(instanceID);
    findEntry->setMajorVersion(MAJOR_VERSION);
    findEntry->setTTL(3);
    findEntry->setMinorVersion(MINOR_VERSION);
    someIpSDHeader->encapEntry(findEntry);

    socket.sendTo(someIpSDHeader, inet::IPv4Address(BROADCASTADDRESS), destPort);
}

void SomeIpSD::offer(uint16_t serviceID, uint16_t instanceID, inet::L3Address remoteAddress, inet::L3Address publisherIP, uint16_t publisherPort, IPProtocolId ipProtocolId) {
    Enter_Method("SomeIpSD::offer");
    SomeIpSDHeader *someIpSDHeader = new SomeIpSDHeader("SOME/IP SD - OFFER");

    ServiceEntry *offerEntry = new ServiceEntry("ServiceEntry");
    offerEntry->setType(SOQoSMW::SomeIpSDEntryType::OFFER);
    offerEntry->setIndex1stOptions(0);
    offerEntry->setIndex2ndOptions(0);
    offerEntry->setNum1stOptions(0);
    offerEntry->setNum2ndOptions(1);
    offerEntry->setServiceID(serviceID);
    offerEntry->setInstanceID(instanceID);
    offerEntry->setMajorVersion(MAJOR_VERSION);
    offerEntry->setTTL(3);
    offerEntry->setMinorVersion(MINOR_VERSION);
    someIpSDHeader->encapEntry(offerEntry);

    IPv4EndpointOption *ipv4EndpointOption = new IPv4EndpointOption("IPv4EndpointOption of Publisher");
    ipv4EndpointOption->setIpv4Address(publisherIP.toIPv4());
    ipv4EndpointOption->setL4Protocol(ipProtocolId);
    ipv4EndpointOption->setPort(publisherPort);
    someIpSDHeader->encapOption(ipv4EndpointOption);

    socket.sendTo(someIpSDHeader, remoteAddress, destPort);
}

void SomeIpSD::subscribeEventgroup(uint16_t serviceID, uint16_t instanceID, inet::L3Address remoteAddress, inet::L3Address subscriberIP, uint16_t subscriberPort) {
    Enter_Method("SomeIpSD::subscribeEventgroup");
    SomeIpSDHeader *someIpSDHeader = new SomeIpSDHeader("SOME/IP SD - SUBSCRIBEEVENTGROUP");

    EventgroupEntry *subscribeEventgroupEntry = new EventgroupEntry("EventgroupEntry");
    subscribeEventgroupEntry->setType(SOQoSMW::SomeIpSDEntryType::SUBSCRIBEVENTGROUP);
    subscribeEventgroupEntry->setIndex1stOptions(0);
    subscribeEventgroupEntry->setIndex2ndOptions(0);
    subscribeEventgroupEntry->setNum1stOptions(0);
    subscribeEventgroupEntry->setNum2ndOptions(1);
    subscribeEventgroupEntry->setServiceID(serviceID);
    subscribeEventgroupEntry->setInstanceID(instanceID);
    subscribeEventgroupEntry->setMajorVersion(MAJOR_VERSION);
    subscribeEventgroupEntry->setTTL(3);
    someIpSDHeader->encapEntry(subscribeEventgroupEntry);

    IPv4EndpointOption *ipv4EndpointOption = new IPv4EndpointOption("IPv4EndpointOption of Subscriber");
    ipv4EndpointOption->setIpv4Address(subscriberIP.toIPv4());
    ipv4EndpointOption->setL4Protocol(IPProtocolId::IP_PROT_UDP);
    ipv4EndpointOption->setPort(subscriberPort);
    someIpSDHeader->encapOption(ipv4EndpointOption);

    socket.sendTo(someIpSDHeader, remoteAddress, destPort);
}

void SomeIpSD::subscribeEventgroupAck(uint16_t serviceID, uint16_t instanceID, inet::L3Address publisherIP, uint16_t publisherPort, inet::L3Address remoteAddress) {
    Enter_Method("SomeIpSD::subscribeEventgroupAck");
    SomeIpSDHeader *someIpSDHeader = new SomeIpSDHeader("SOME/IP SD - SUBSCRIBEEVENTGROUPACK");

    EventgroupEntry *subscribeEventgroupAckEntry = new EventgroupEntry("EventgroupEntry");
    subscribeEventgroupAckEntry->setType(SOQoSMW::SomeIpSDEntryType::SUBSCRIBEVENTGROUPACK);
    subscribeEventgroupAckEntry->setIndex1stOptions(0);
    subscribeEventgroupAckEntry->setIndex2ndOptions(0);
    subscribeEventgroupAckEntry->setNum1stOptions(0);
    subscribeEventgroupAckEntry->setNum2ndOptions(1);
    subscribeEventgroupAckEntry->setServiceID(serviceID);
    subscribeEventgroupAckEntry->setInstanceID(instanceID);
    subscribeEventgroupAckEntry->setMajorVersion(MAJOR_VERSION);
    subscribeEventgroupAckEntry->setTTL(3);
    someIpSDHeader->encapEntry(subscribeEventgroupAckEntry);

    IPv4EndpointOption *ipv4EndpointOption = new IPv4EndpointOption("IPv4EndpointOption of Publisher");
    ipv4EndpointOption->setIpv4Address(publisherIP.toIPv4());
    ipv4EndpointOption->setL4Protocol(IPProtocolId::IP_PROT_UDP);
    ipv4EndpointOption->setPort(publisherPort);
    someIpSDHeader->encapOption(ipv4EndpointOption);

    socket.sendTo(someIpSDHeader, remoteAddress, destPort);
}

void SomeIpSD::processSomeIpSDHeader(SomeIpSDHeader* someIpSDHeader) {
    std::list<SomeIpSDEntry*> entries = someIpSDHeader->getEncapEntries();
    for (std::list<SomeIpSDEntry*>::const_iterator it = entries.begin(); it != entries.end(); ++it) {
        switch ((*it)->getType()) {
            case SOQoSMW::SomeIpSDEntryType::FIND:
                EV << "FIND ARRIVED" << endl;
                processFindEntry(*it, someIpSDHeader);
                break;
            case SOQoSMW::SomeIpSDEntryType::OFFER:
                EV << "OFFER ARRIVED" << endl;
                processOfferEntry(*it, someIpSDHeader);
                break;
            case SOQoSMW::SomeIpSDEntryType::SUBSCRIBEVENTGROUP:
                EV << "SUBSCRIBEVENTGROUP ARRIVED" << endl;
                processSubscribeEventGroupEntry(*it, someIpSDHeader);
                break;
            case SOQoSMW::SomeIpSDEntryType::SUBSCRIBEVENTGROUPACK:
                EV << "SUBSCRIBEVENTGROUPACK ARRIVED" << endl;
                processSubscribeEventGroupAckEntry(*it, someIpSDHeader);
                break;
            default:
                EV << "Unknown type" << std::endl;
        }
    }
}


void SomeIpSD::processFindEntry(SomeIpSDEntry* findEntry, SomeIpSDHeader* someIpSDHeader) {
    inet::UDPDataIndication *udpDataIndication = dynamic_cast<inet::UDPDataIndication*>(someIpSDHeader->getControlInfo());
    SomeIpSDFindRequest* someIpSDFindRequest = new SomeIpSDFindRequest(findEntry->getServiceID(), findEntry->getInstanceID(), udpDataIndication->getSrcAddr());
    emit(_serviceFindSignal, someIpSDFindRequest);
}

void SomeIpSD::processFindResult(cObject* obj) {
    SomeIpSDFindResult* someIpSDFindResult = dynamic_cast<SomeIpSDFindResult*>(obj);
    QoSService service = someIpSDFindResult->getService();
    int port = -1;
    switch(someIpSDFindResult->getIPProtocolId()) {
        case IPProtocolId::IP_PROT_UDP:
            port = service.getUDPPort();
            break;
        case IPProtocolId::IP_PROT_TCP:
            port = service.getTCPPort();
            break;
        default:
            throw cRuntimeError("Unknown Protocol");
    }
    if (port == -1) {
        throw cRuntimeError("-1 is an invalid port.");
    }
    offer(someIpSDFindResult->getServiceId(), someIpSDFindResult->getInstanceId(), someIpSDFindResult->getRemoteAddress(),
            service.getAddress(), port, someIpSDFindResult->getIPProtocolId());
    delete(someIpSDFindResult);
}

void SomeIpSD::processOfferEntry(SomeIpSDEntry* offerEntry, SomeIpSDHeader* someIpSDHeader) {
    int numberOfOptions = offerEntry->getNum1stOptions()+offerEntry->getNum2ndOptions();
    for (int optionIdx = 0; optionIdx < numberOfOptions; optionIdx++) {
        if (IPv4EndpointOption* ipv4EndpointOption = dynamic_cast<IPv4EndpointOption*>(someIpSDHeader->decapOption())) {
            ExtractedQoSOptions extractedQoSOptions = getExtractedQoSOptions(ipv4EndpointOption);
            std::set<QoSGroups> qosGroups;
            qosGroups.insert(extractedQoSOptions.getQosGroup());
            QoSService* service = new QoSService(offerEntry->getServiceID(), ipv4EndpointOption->getIpv4Address(),
                                                 offerEntry->getInstanceID(), qosGroups, extractedQoSOptions.getTcpPort(),
                                                 extractedQoSOptions.getUdpPort());
            emit(_serviceOfferSignal,service);
            delete ipv4EndpointOption;
        }
    }
}

void SomeIpSD::processSubscribeEventGroupEntry(SomeIpSDEntry* subscribeEventGroupEntry, SomeIpSDHeader* someIpSDHeader) {
    int numberOfOptions = subscribeEventGroupEntry->getNum1stOptions()+subscribeEventGroupEntry->getNum2ndOptions();
    for (int optionIdx = 0; optionIdx < numberOfOptions; optionIdx++) {
        if (IPv4EndpointOption* ipv4EndpointOption = dynamic_cast<IPv4EndpointOption*>(someIpSDHeader->decapOption())) {
            ExtractedQoSOptions extractedQoSOptions = getExtractedQoSOptions(ipv4EndpointOption);
            std::set<QoSGroups> qosGroups;
            qosGroups.insert(extractedQoSOptions.getQosGroup());
            QoSService* service = new QoSService(subscribeEventGroupEntry->getServiceID(), ipv4EndpointOption->getIpv4Address(),
                                                 subscribeEventGroupEntry->getInstanceID(), qosGroups, extractedQoSOptions.getTcpPort(),
                                                 extractedQoSOptions.getUdpPort());
            emit(_subscribeEventGroupSignal, service);
            delete ipv4EndpointOption;
        }
    }
}

void SomeIpSD::processSubscribeEventGroupAckEntry(SomeIpSDEntry *subscribeEventGroupAckEntry, SomeIpSDHeader* someIpSDHeader) {
    int numberOfOptions = subscribeEventGroupAckEntry->getNum1stOptions()+subscribeEventGroupAckEntry->getNum2ndOptions();
    for (int optionIdx = 0; optionIdx < numberOfOptions; optionIdx++) {
        if (IPv4EndpointOption* ipv4EndpointOption = dynamic_cast<IPv4EndpointOption*>(someIpSDHeader->decapOption())) {
            ExtractedQoSOptions extractedQoSOptions = getExtractedQoSOptions(ipv4EndpointOption);
            std::set<QoSGroups> qosGroups;
            qosGroups.insert(extractedQoSOptions.getQosGroup());
            QoSService* service = new QoSService(subscribeEventGroupAckEntry->getServiceID(), ipv4EndpointOption->getIpv4Address(),
                                                 subscribeEventGroupAckEntry->getInstanceID(), qosGroups, extractedQoSOptions.getTcpPort(),
                                                 extractedQoSOptions.getUdpPort());
            emit(_subscribeEventGroupAckSignal,service);
            delete ipv4EndpointOption;
        }
    }
}

void SomeIpSD::discover(QoSServiceIdentifier qosServiceIdentifier) {
    find(qosServiceIdentifier.getServiceId(),qosServiceIdentifier.getInstanceId());
}

void SomeIpSD::processSubscription(cObject* obj) {
    SomeIpSDSubscriptionInformation& someIpSDSubscriptionInformation = *dynamic_cast<SomeIpSDSubscriptionInformation*>(obj);
    subscribeService(someIpSDSubscriptionInformation.getServiceId(), someIpSDSubscriptionInformation.getInstanceId(), someIpSDSubscriptionInformation.getRemoteAddress(),
            someIpSDSubscriptionInformation.getLocalAddress(), someIpSDSubscriptionInformation.getLocalPort());
}

void SomeIpSD::processAcknowledgment(cObject *obj) {
    SomeIpSDSubscriptionInformation& someIpSDSubscriptionInformation = *dynamic_cast<SomeIpSDSubscriptionInformation*>(obj);
    acknowledgeSubscription(someIpSDSubscriptionInformation.getServiceId(), someIpSDSubscriptionInformation.getInstanceId(), someIpSDSubscriptionInformation.getLocalAddress(),
            someIpSDSubscriptionInformation.getLocalPort(), someIpSDSubscriptionInformation.getRemoteAddress());
}

void SomeIpSD::subscribeService(uint16_t serviceID, uint16_t instanceID, inet::L3Address publisherIP, inet::L3Address subscriberIP, uint16_t subscriberPort) {
    subscribeEventgroup(serviceID, instanceID, publisherIP, subscriberIP, subscriberPort);
}

void SomeIpSD::acknowledgeSubscription(uint16_t serviceID, uint16_t instanceID, inet::L3Address publisherIP, uint16_t publisherPort, inet::L3Address remoteAddress) {
    subscribeEventgroupAck(serviceID, instanceID, publisherIP, publisherPort, remoteAddress);
}

void SomeIpSD::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj, cObject *details) {
    if (!strcmp(getSignalName(signalID),"findResultSignal")) {
        processFindResult(obj);
    } else if(!strcmp(getSignalName(signalID),"subscribeSignal")) {
        processSubscription(obj);
    } else if(!strcmp(getSignalName(signalID),"subscribeAckSignal")){
        processAcknowledgment(obj);
    } else {
        throw cRuntimeError("Unknown signal.");
    }
}

ExtractedQoSOptions SomeIpSD::getExtractedQoSOptions(IPv4EndpointOption* ipv4EndpointOption) {
    QoSGroups qosGroup = QoSGroups::RT;
    int tcpPort = -1;
    int udpPort = -1;
    switch(ipv4EndpointOption->getL4Protocol()) {
        case IPProtocolId::IP_PROT_UDP:
            qosGroup = QoSGroups::SOMEIP_UDP;
            udpPort = ipv4EndpointOption->getPort();
            break;
        case IPProtocolId::IP_PROT_TCP:
            qosGroup = QoSGroups::SOMEIP_TCP;
            tcpPort = ipv4EndpointOption->getPort();
            break;
        default:
            throw cRuntimeError("Unknown L4 Protocol.");
    }
    return ExtractedQoSOptions(qosGroup, udpPort, tcpPort);
}

} /* end namespace SOQoSMW */
