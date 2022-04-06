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

#include <soa4core/discovery/SomeIpDiscoveryNotification.h>
#include "soa4core/discovery/someip/SomeIpSD.h"
#include "soa4core/manager/qos/QoSManager.h"
#include "soa4core/manager/someip/SomeIpManager.h"
#include "soa4core/serviceidentifier/ServiceIdentifier.h"
//INET
#include <inet/networklayer/common/L3AddressResolver.h>
#include <inet/networklayer/contract/ipv4/IPv4Address.h>
//STD
#include <list>

namespace SOA4CoRE {

#define MAJOR_VERSION 0xFF       // see [PRS_SOMEIPSD_00351],[PRS_SOMEIPSD_00356],[PRS_SOMEIPSD_00386],[PRS_SOMEIPSD_00391]
#define MINOR_VERSION 0xFFFFFFFF // see [PRS_SOMEIPSD_00351],[PRS_SOMEIPSD_00356],[PRS_SOMEIPSD_00386],[PRS_SOMEIPSD_00391]
#define TTL 0xFFFFFF             // see [PRS_SOMEIPSD_00351],[PRS_SOMEIPSD_00356],[PRS_SOMEIPSD_00386],[PRS_SOMEIPSD_00391]
#define QOS_NP_OPTIONS_COUNT 1

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
        _localAddress = inet::L3AddressResolver().resolve(par("localAddress").stdstringValue().c_str());
        processStart();
        IServiceDiscovery::_serviceOfferSignal = omnetpp::cComponent::registerSignal("serviceOfferSignal");
        _serviceFindSignal = omnetpp::cComponent::registerSignal("serviceFindSignal");
        _subscribeEventGroupSignal = omnetpp::cComponent::registerSignal("subscribeEventGroupSignal");
        _subscribeEventGroupAckSignal = omnetpp::cComponent::registerSignal("subscribeEventGroupAckSignal");

        _hasQoSNP = getParentModule()->par("hasQoSNP");
        if (SomeIpManager* someIplocalServiceManager = dynamic_cast<SomeIpManager*>(getParentModule()->getSubmodule("sm"))) {
            someIplocalServiceManager->subscribe("findResultSignal",this);
            someIplocalServiceManager->subscribe("subscribeSignal",this);
            someIplocalServiceManager->subscribe("subscribeAckSignal",this);
        } else if (QoSManager* qosLocalServiceManager = dynamic_cast<QoSManager*>(getParentModule()->getSubmodule("sm"))){
            qosLocalServiceManager->subscribe("findResultSignal",this);
        } else {
            throw cRuntimeError("SomeIpManager or QoSManager is needed for SOME/IP Discovery.");
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
    findEntry->setType(SOA4CoRE::SomeIpSDEntryType::FIND);
    findEntry->setIndex1stOptions(0);
    findEntry->setIndex2ndOptions(0);
    findEntry->setNum1stOptions(0);
    findEntry->setNum2ndOptions(0);
    findEntry->setServiceID(serviceID);
    findEntry->setInstanceID(instanceID);
    findEntry->setMajorVersion(MAJOR_VERSION);
    findEntry->setTTL(TTL);
    findEntry->setMinorVersion(MINOR_VERSION);
    someIpSDHeader->encapEntry(findEntry);

    socket.sendTo(someIpSDHeader, inet::IPv4Address(BROADCASTADDRESS), destPort);
}

void SomeIpSD::offer(SomeIpDiscoveryNotification* someIpDiscoveryNotification) {
    Enter_Method("SomeIpSD::offer");
    SomeIpSDHeader *someIpSDHeader = new SomeIpSDHeader("SOME/IP SD - OFFER");

    ServiceEntry *offerEntry = new ServiceEntry("ServiceEntry");
    offerEntry->setType(SOA4CoRE::SomeIpSDEntryType::OFFER);
    offerEntry->setIndex1stOptions(0);
    offerEntry->setIndex2ndOptions(0);
    offerEntry->setNum1stOptions(someIpDiscoveryNotification->getQoSGroups().size());
    offerEntry->setNum2ndOptions(0);
    offerEntry->setServiceID(someIpDiscoveryNotification->getServiceId());
    offerEntry->setInstanceID(someIpDiscoveryNotification->getInstanceId());
    offerEntry->setMajorVersion(MAJOR_VERSION);
    offerEntry->setTTL(TTL);
    offerEntry->setMinorVersion(MINOR_VERSION);
    someIpSDHeader->encapEntry(offerEntry);

    if (!_hasQoSNP) {
        for (QoSGroup qosGroup : someIpDiscoveryNotification->getQoSGroups()) {
            IPProtocolId ipProtocolId;
            uint16_t publisherPort;
            switch (qosGroup) {
                case QoSGroup::SOMEIP_TCP:
                    ipProtocolId = IPProtocolId::IP_PROT_TCP;
                    publisherPort = someIpDiscoveryNotification->getTcpPort();
                    break;
                case QoSGroup::SOMEIP_UDP:
                    ipProtocolId = IPProtocolId::IP_PROT_UDP;
                    publisherPort = someIpDiscoveryNotification->getUdpPort();
                    break;
                default:
                    throw cRuntimeError("Unknown QoSGroup");
            }

            IPv4EndpointOption *ipv4EndpointOption = new IPv4EndpointOption("IPv4EndpointOption of Publisher");
            ipv4EndpointOption->setIpv4Address(this->_localAddress.toIPv4());
            ipv4EndpointOption->setL4Protocol(ipProtocolId);
            ipv4EndpointOption->setPort(publisherPort);
            someIpSDHeader->encapOption(ipv4EndpointOption);
        }
    } else {
        offerEntry->setNum1stOptions(QOS_NP_OPTIONS_COUNT);
        IPv4EndpointOption *ipv4EndpointOption = new IPv4EndpointOption("IPv4EndpointOption of Publisher");
        ipv4EndpointOption->setIpv4Address(this->_localAddress.toIPv4());
        someIpSDHeader->encapOption(ipv4EndpointOption);
    }

    std::string moin = this->_localAddress.str();

    socket.sendTo(someIpSDHeader, someIpDiscoveryNotification->getAddress(), destPort);
}

void SomeIpSD::subscribeEventgroup(SomeIpDiscoveryNotification* someIpDiscoveryNotification) {
    Enter_Method("SomeIpSD::subscribeEventgroup");
    SomeIpSDHeader *someIpSDHeader = new SomeIpSDHeader("SOME/IP SD - SUBSCRIBEEVENTGROUP");

    EventgroupEntry *subscribeEventgroupEntry = new EventgroupEntry("EventgroupEntry");
    subscribeEventgroupEntry->setType(SOA4CoRE::SomeIpSDEntryType::SUBSCRIBEVENTGROUP);
    subscribeEventgroupEntry->setIndex1stOptions(0);
    subscribeEventgroupEntry->setIndex2ndOptions(0);
    subscribeEventgroupEntry->setNum1stOptions(1);
    subscribeEventgroupEntry->setNum2ndOptions(0);
    subscribeEventgroupEntry->setServiceID(someIpDiscoveryNotification->getServiceId());
    subscribeEventgroupEntry->setInstanceID(someIpDiscoveryNotification->getInstanceId());
    subscribeEventgroupEntry->setMajorVersion(MAJOR_VERSION);
    subscribeEventgroupEntry->setTTL(TTL);
    someIpSDHeader->encapEntry(subscribeEventgroupEntry);

    IPProtocolId ipProtocolId;
    uint16_t subscriberPort;
    switch (someIpDiscoveryNotification->getQoSGroup()) {
        case QoSGroup::SOMEIP_TCP:
            ipProtocolId = IPProtocolId::IP_PROT_TCP;
            subscriberPort = someIpDiscoveryNotification->getTcpPort();
            break;
        case QoSGroup::SOMEIP_UDP:
            ipProtocolId = IPProtocolId::IP_PROT_UDP;
            subscriberPort = someIpDiscoveryNotification->getUdpPort();
            break;
        default:
            throw cRuntimeError("Unknown QoSGroup");
    }

    IPv4EndpointOption *ipv4EndpointOption = new IPv4EndpointOption("IPv4EndpointOption of Subscriber");
    ipv4EndpointOption->setIpv4Address(this->_localAddress.toIPv4());
    ipv4EndpointOption->setL4Protocol(ipProtocolId);
    ipv4EndpointOption->setPort(subscriberPort);
    someIpSDHeader->encapOption(ipv4EndpointOption);

    socket.sendTo(someIpSDHeader, someIpDiscoveryNotification->getAddress(), destPort);
}

void SomeIpSD::subscribeEventgroupAck(SomeIpDiscoveryNotification* someIpDiscoveryNotification) {
    Enter_Method("SomeIpSD::subscribeEventgroupAck");
    SomeIpSDHeader *someIpSDHeader = new SomeIpSDHeader("SOME/IP SD - SUBSCRIBEEVENTGROUPACK");

    EventgroupEntry *subscribeEventgroupAckEntry = new EventgroupEntry("EventgroupEntry");
    subscribeEventgroupAckEntry->setType(SOA4CoRE::SomeIpSDEntryType::SUBSCRIBEVENTGROUPACK);
    subscribeEventgroupAckEntry->setIndex1stOptions(0);
    subscribeEventgroupAckEntry->setIndex2ndOptions(0);
    subscribeEventgroupAckEntry->setNum1stOptions(1);
    subscribeEventgroupAckEntry->setNum2ndOptions(0);
    subscribeEventgroupAckEntry->setServiceID(someIpDiscoveryNotification->getServiceId());
    subscribeEventgroupAckEntry->setInstanceID(someIpDiscoveryNotification->getInstanceId());
    subscribeEventgroupAckEntry->setMajorVersion(MAJOR_VERSION);
    subscribeEventgroupAckEntry->setTTL(TTL);
    someIpSDHeader->encapEntry(subscribeEventgroupAckEntry);

    IPProtocolId ipProtocolId;
    uint16_t publisherPort;
    switch (someIpDiscoveryNotification->getQoSGroup()) {
        case QoSGroup::SOMEIP_TCP:
            ipProtocolId = IPProtocolId::IP_PROT_TCP;
            publisherPort = someIpDiscoveryNotification->getTcpPort();
            break;
        case QoSGroup::SOMEIP_UDP:
            ipProtocolId = IPProtocolId::IP_PROT_UDP;
            publisherPort = someIpDiscoveryNotification->getUdpPort();
            break;
        default:
            throw cRuntimeError("Unknown QoSGroup");
    }

    IPv4EndpointOption *ipv4EndpointOption = new IPv4EndpointOption("IPv4EndpointOption of Publisher");
    ipv4EndpointOption->setIpv4Address(this->_localAddress.toIPv4());
    ipv4EndpointOption->setL4Protocol(ipProtocolId);
    ipv4EndpointOption->setPort(publisherPort);
    someIpSDHeader->encapOption(ipv4EndpointOption);

    socket.sendTo(someIpSDHeader, someIpDiscoveryNotification->getAddress(), destPort);
}

void SomeIpSD::processSomeIpSDHeader(SomeIpSDHeader* someIpSDHeader) {
    std::list<SomeIpSDEntry*> entries = someIpSDHeader->getEncapEntries();
    for (std::list<SomeIpSDEntry*>::const_iterator it = entries.begin(); it != entries.end(); ++it) {
        switch ((*it)->getType()) {
            case SOA4CoRE::SomeIpSDEntryType::FIND:
                EV << "FIND ARRIVED" << endl;
                processFindEntry(*it, someIpSDHeader);
                break;
            case SOA4CoRE::SomeIpSDEntryType::OFFER:
                EV << "OFFER ARRIVED" << endl;
                processOfferEntry(*it, someIpSDHeader);
                break;
            case SOA4CoRE::SomeIpSDEntryType::SUBSCRIBEVENTGROUP:
                EV << "SUBSCRIBEVENTGROUP ARRIVED" << endl;
                processSubscribeEventGroupEntry(*it, someIpSDHeader);
                break;
            case SOA4CoRE::SomeIpSDEntryType::SUBSCRIBEVENTGROUPACK:
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
    SomeIpDiscoveryNotification* someIpDiscoveryNotification = new SomeIpDiscoveryNotification(findEntry->getServiceID(),
            udpDataIndication->getSrcAddr(), findEntry->getInstanceID(), std::set<QoSGroup>{}, QoSGroup::UNDEFINED, -1, -1);
    emit(_serviceFindSignal, someIpDiscoveryNotification);
}

void SomeIpSD::processFindResult(cObject* obj) {
    SomeIpDiscoveryNotification* someIpDiscoveryNotification = nullptr;
    if (!(someIpDiscoveryNotification = dynamic_cast<SomeIpDiscoveryNotification*>(obj))) {
        throw cRuntimeError("The discovery notification must be of type SomeIpDiscoveryNotification.");
    }
    offer(someIpDiscoveryNotification);
    delete(obj);
}

void SomeIpSD::processOfferEntry(SomeIpSDEntry* offerEntry, SomeIpSDHeader* someIpSDHeader) {
    std::list<SomeIpSDOption*> optionsList = someIpSDHeader->getEncapOptions();
    std::_List_iterator<SomeIpSDOption*> optionsListIterator = optionsList.begin();

    std::set<QoSGroup> qosGroups;
    L3Address ipAddress;
    uint16_t tcpPort;
    uint16_t udpPort;

    std::advance(optionsListIterator, offerEntry->getIndex1stOptions());
    for (int firstOptionsIdx = 0; firstOptionsIdx < offerEntry->getNum1stOptions(); firstOptionsIdx++) {
        std::advance(optionsListIterator, firstOptionsIdx);
        IPv4EndpointOption* ipv4EndpointOption = dynamic_cast<IPv4EndpointOption*>(*optionsListIterator);
        if(!ipv4EndpointOption) {
            throw cRuntimeError("SomeIpSDOption is not type of IPv4EndpointOption");
        }
        ipAddress = ipv4EndpointOption->getIpv4Address();
        if (!_hasQoSNP) {
            ExtractedQoSOptions extractedQoSOptions = getExtractedQoSOptions(ipv4EndpointOption);
            tcpPort = extractedQoSOptions.getTcpPort() != -1 ? extractedQoSOptions.getTcpPort() : tcpPort;
            udpPort = extractedQoSOptions.getUdpPort() != -1 ? extractedQoSOptions.getUdpPort() : udpPort;
            qosGroups.insert(extractedQoSOptions.getQosGroup());
        }
    }
    SomeIpDiscoveryNotification* someIpDiscoveryNotification = new SomeIpDiscoveryNotification(offerEntry->getServiceID(), ipAddress,
            offerEntry->getInstanceID(), qosGroups, QoSGroup::UNDEFINED, tcpPort, udpPort);
    emit(_serviceOfferSignal,someIpDiscoveryNotification);
}

void SomeIpSD::processSubscribeEventGroupEntry(SomeIpSDEntry* subscribeEventGroupEntry, SomeIpSDHeader* someIpSDHeader) {
    std::list<SomeIpSDOption*> optionsList = someIpSDHeader->getEncapOptions();
    std::_List_iterator<SomeIpSDOption*> optionsListIterator = optionsList.begin();

    QoSGroup qosGroup;
    std::advance(optionsListIterator, subscribeEventGroupEntry->getIndex1stOptions());
    for (int firstOptionsIdx = 0; firstOptionsIdx < subscribeEventGroupEntry->getNum1stOptions(); firstOptionsIdx++) {
        std::advance(optionsListIterator, firstOptionsIdx);
        IPv4EndpointOption* ipv4EndpointOption = dynamic_cast<IPv4EndpointOption*>(*optionsListIterator);
        if(!ipv4EndpointOption) {
            throw cRuntimeError("SomeIpSDOption is not type of IPv4EndpointOption");
        }
        ExtractedQoSOptions extractedQoSOptions = getExtractedQoSOptions(ipv4EndpointOption);
        qosGroup = extractedQoSOptions.getQosGroup();
        SomeIpDiscoveryNotification* someIpDiscoveryNotification = new SomeIpDiscoveryNotification(subscribeEventGroupEntry->getServiceID(), ipv4EndpointOption->getIpv4Address(),
                subscribeEventGroupEntry->getInstanceID(), std::set<QoSGroup>{}, qosGroup, extractedQoSOptions.getTcpPort(), extractedQoSOptions.getUdpPort());
        emit(_subscribeEventGroupSignal, someIpDiscoveryNotification);
    }
}

void SomeIpSD::processSubscribeEventGroupAckEntry(SomeIpSDEntry *subscribeEventGroupAckEntry, SomeIpSDHeader* someIpSDHeader) {
    std::list<SomeIpSDOption*> optionsList = someIpSDHeader->getEncapOptions();
    std::_List_iterator<SomeIpSDOption*> optionsListIterator = optionsList.begin();

    std::set<QoSGroup> qosGroups;
    L3Address ipAddress;
    uint16_t tcpPort;
    uint16_t udpPort;

    std::advance(optionsListIterator, subscribeEventGroupAckEntry->getIndex1stOptions());
    for (int firstOptionsIdx = 0; firstOptionsIdx < subscribeEventGroupAckEntry->getNum1stOptions(); firstOptionsIdx++) {
        std::advance(optionsListIterator, firstOptionsIdx);
        IPv4EndpointOption* ipv4EndpointOption = dynamic_cast<IPv4EndpointOption*>(*optionsListIterator);
        if(!ipv4EndpointOption) {
            throw cRuntimeError("SomeIpSDOption is not type of IPv4EndpointOption");
        }
        ipAddress = ipv4EndpointOption->getIpv4Address();
        ExtractedQoSOptions extractedQoSOptions = getExtractedQoSOptions(ipv4EndpointOption);
        tcpPort = extractedQoSOptions.getTcpPort() != -1 ? extractedQoSOptions.getTcpPort() : tcpPort;
        udpPort = extractedQoSOptions.getUdpPort() != -1 ? extractedQoSOptions.getUdpPort() : udpPort;
        qosGroups.insert(extractedQoSOptions.getQosGroup());
    }
    SomeIpDiscoveryNotification* someIpDiscoveryNotification = new SomeIpDiscoveryNotification(subscribeEventGroupAckEntry->getServiceID(), ipAddress,
            subscribeEventGroupAckEntry->getInstanceID(), qosGroups, QoSGroup::UNDEFINED, tcpPort, udpPort);
    emit(_subscribeEventGroupAckSignal, someIpDiscoveryNotification);
}

void SomeIpSD::discover(ServiceIdentifier serviceIdentifier) {
    find(serviceIdentifier.getServiceId(),serviceIdentifier.getInstanceId());
}

void SomeIpSD::processSubscription(cObject* obj) {
    SomeIpDiscoveryNotification* someIpdiscoveryNotificationSubscription = nullptr;
    if (!(someIpdiscoveryNotificationSubscription = dynamic_cast<SomeIpDiscoveryNotification*>(obj))) {
        throw cRuntimeError("The discovery notification must be of type SomeIpDiscoveryNotification");
    }
    subscribeEventgroup(someIpdiscoveryNotificationSubscription);
    delete obj;
}

void SomeIpSD::processAcknowledgment(cObject *obj) {
    SomeIpDiscoveryNotification* someIpDiscoveryNotificationAcknowledge = nullptr;
    if (!(someIpDiscoveryNotificationAcknowledge = dynamic_cast<SomeIpDiscoveryNotification*>(obj))) {
        throw cRuntimeError("The discovery notification must be of type SomeIpDiscoveryNotification");
    }
    subscribeEventgroupAck(someIpDiscoveryNotificationAcknowledge);
    delete obj;
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
    QoSGroup qosGroup = QoSGroup::RT;
    int tcpPort = -1;
    int udpPort = -1;
    switch(ipv4EndpointOption->getL4Protocol()) {
        case IPProtocolId::IP_PROT_UDP:
            qosGroup = QoSGroup::SOMEIP_UDP;
            udpPort = ipv4EndpointOption->getPort();
            break;
        case IPProtocolId::IP_PROT_TCP:
            qosGroup = QoSGroup::SOMEIP_TCP;
            tcpPort = ipv4EndpointOption->getPort();
            break;
        default:
            throw cRuntimeError("Unknown L4 Protocol.");
    }
    return ExtractedQoSOptions(qosGroup, udpPort, tcpPort);
}

} /* end namespace SOA4CoRE */
