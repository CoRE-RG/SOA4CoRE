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
#include "inet/common/ModuleAccess.h"
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
        _mcastDestAddress = inet::L3Address(par("mcastDestAddress"));
        startTime = par("startTime").doubleValue();
        stopTime = par("stopTime").doubleValue();
        packetName = par("packetName");
        _includeIEEE8021QConfig = par("includeIEEE8021QConfig").boolValue();
        _includeRessourceConfig = par("includeRessourceConfig").boolValue();
        _includeRealTimeConfig = par("includeRealTimeConfig").boolValue();
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

void SomeIpSD::setSocketOptions() {
    socket.setMulticastLoop(false);
    UDPBasicApp::setSocketOptions();
    const char *multicastInterface = par("multicastInterface");
    if (multicastInterface[0]) {
        IInterfaceTable *ift = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this);
        InterfaceEntry *ie = ift->getInterfaceByName(multicastInterface);
        if (!ie)
            throw cRuntimeError("Wrong multicastInterface setting: no interface named \"%s\"", multicastInterface);
        socket.joinMulticastGroup(_mcastDestAddress, ie->getInterfaceId());
    }
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

    sendTo(someIpSDHeader);
}

void SomeIpSD::offer(SomeIpDiscoveryNotification* notification) {
    Enter_Method("SomeIpSD::offer");
    SomeIpSDHeader *someIpSDHeader = new SomeIpSDHeader("SOME/IP SD - OFFER");

    ServiceEntry *offerEntry = new ServiceEntry("ServiceEntry");
    offerEntry->setType(SOA4CoRE::SomeIpSDEntryType::OFFER);
    offerEntry->setIndex1stOptions(0);
    offerEntry->setIndex2ndOptions(0);
    offerEntry->setNum1stOptions(notification->getQoSGroups().size());
    offerEntry->setNum2ndOptions(0);
    offerEntry->setServiceID(notification->getServiceId());
    offerEntry->setInstanceID(notification->getInstanceId());
    offerEntry->setMajorVersion(MAJOR_VERSION);
    offerEntry->setTTL(TTL);
    offerEntry->setMinorVersion(MINOR_VERSION);
    someIpSDHeader->encapEntry(offerEntry);

    if (!_hasQoSNP) {
        for (QoSGroup qosGroup : notification->getQoSGroups())
        {
            IPv4EndpointOption *ipv4EndpointOption = createIpv4Endpoint(notification,qosGroup);
            someIpSDHeader->encapOption(ipv4EndpointOption);
        }
        if (_includeIEEE8021QConfig
                && notification->getPcp() >= 0
                && notification->getVlanId() >= 0)
        {
            IEEE8021QConfigurationOption* qconfig = new IEEE8021QConfigurationOption();
            qconfig->setVlan_id(notification->getVlanId());
            qconfig->setPcp(notification->getPcp());
            someIpSDHeader->encapOption(qconfig);
            offerEntry->setNum1stOptions(offerEntry->getNum1stOptions() + 1);
        }
        if (_includeRessourceConfig
                && notification->getIntervalMin() > 0)
        {
            RessourceConfigurationOption* resconfig = new RessourceConfigurationOption();
            resconfig->setMaxPayload(notification->getPayloadMax());
            resconfig->setMinInterval(notification->getIntervalMin());
            someIpSDHeader->encapOption(resconfig);
            offerEntry->setNum1stOptions(offerEntry->getNum1stOptions() + 1);
        }
        if (_includeRealTimeConfig
                && notification->getDeadline() > 0)
        {
            RealTimeConfigurationOption* rtconfig = new RealTimeConfigurationOption();
            rtconfig->setDeadline(notification->getDeadline());
            someIpSDHeader->encapOption(rtconfig);
            offerEntry->setNum1stOptions(offerEntry->getNum1stOptions() + 1);
        }
    } else {
        offerEntry->setNum1stOptions(QOS_NP_OPTIONS_COUNT);
        IPv4EndpointOption *ipv4EndpointOption = new IPv4EndpointOption("IPv4EndpointOption of Publisher");
        ipv4EndpointOption->setIpv4Address(this->_localAddress.toIPv4());
        someIpSDHeader->encapOption(ipv4EndpointOption);
    }

    sendTo(someIpSDHeader, notification->getAddress());
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

    IPv4EndpointOption *ipv4EndpointOption = createIpv4Endpoint(someIpDiscoveryNotification);
    someIpSDHeader->encapOption(ipv4EndpointOption);

    if(_includeRealTimeConfig && someIpDiscoveryNotification->getDeadline() > 0)
    {
        RealTimeConfigurationOption* rtconfig = new RealTimeConfigurationOption();
        rtconfig->setDeadline(someIpDiscoveryNotification->getDeadline());
        someIpSDHeader->encapOption(rtconfig);
        subscribeEventgroupEntry->setNum1stOptions(subscribeEventgroupEntry->getNum1stOptions() + 1);
    }

    sendTo(someIpSDHeader, someIpDiscoveryNotification->getAddress());
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

    IPv4EndpointOption *ipv4EndpointOption = createIpv4Endpoint(someIpDiscoveryNotification);
    someIpSDHeader->encapOption(ipv4EndpointOption);

    sendTo(someIpSDHeader, someIpDiscoveryNotification->getAddress());
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

    SomeIpDiscoveryNotification* notification = new SomeIpDiscoveryNotification();
    notification->setServiceId(offerEntry->getServiceID());
    notification->setInstanceId(offerEntry->getInstanceID());

    std::advance(optionsListIterator, offerEntry->getIndex1stOptions());
    for (int firstOptionsIdx = 0; firstOptionsIdx < offerEntry->getNum1stOptions(); firstOptionsIdx++)
    {
        if(IPv4EndpointOption* ipv4EndpointOption = dynamic_cast<IPv4EndpointOption*>(*optionsListIterator))
        {
            if (_hasQoSNP) {
                if(!dynamic_cast<IPv4MulticastOption*>(ipv4EndpointOption)) {
                    notification->setAddress(ipv4EndpointOption->getIpv4Address());
                }
            } else {
                notification->updateFromEndpointOption(ipv4EndpointOption);
            }
        }
        else if(!dynamic_cast<ConfigurationOption*>(*optionsListIterator)) {
            throw cRuntimeError("SomeIpSDOption is not of type IPv4EndpointOption");
        }
        optionsListIterator++;
    }

    if(notification->getAddress().isUnspecified()) {
        // check if this is mcast endpoint
        if(notification->getMcastDestAddr().isUnspecified()) {
            throw cRuntimeError("No address found in non mcast options");
        }
        // use udp src of someip sd message instead
        if(inet::UDPDataIndication *udpDataIndication = dynamic_cast<inet::UDPDataIndication*>(someIpSDHeader->getControlInfo())) {
            notification->setAddress(udpDataIndication->getSrcAddr());
        } else {
            throw cRuntimeError("Could not determine any address for service");
        }
    }

    emit(_serviceOfferSignal,notification);
}

void SomeIpSD::processSubscribeEventGroupEntry(SomeIpSDEntry* subscribeEventGroupEntry, SomeIpSDHeader* someIpSDHeader) {
    std::list<SomeIpSDOption*> optionsList = someIpSDHeader->getEncapOptions();
    std::_List_iterator<SomeIpSDOption*> optionsListIterator = optionsList.begin();

    std::advance(optionsListIterator, subscribeEventGroupEntry->getIndex1stOptions());
    for (int firstOptionsIdx = 0; firstOptionsIdx < subscribeEventGroupEntry->getNum1stOptions(); firstOptionsIdx++)
    {
        if(!dynamic_cast<ConfigurationOption*>(*optionsListIterator))
        {
            SomeIpDiscoveryNotification* notification = new SomeIpDiscoveryNotification();
            notification->setServiceId(subscribeEventGroupEntry->getServiceID());
            notification->setInstanceId(subscribeEventGroupEntry->getInstanceID());
            notification->updateFromEndpointOption(*optionsListIterator);
            notification->setQosGroup(*(notification->getQoSGroups().begin()));
            if(notification->getAddress().isUnspecified())
            {
                // use udp src of someip sd message instead
                if(inet::UDPDataIndication *udpDataIndication = dynamic_cast<inet::UDPDataIndication*>(someIpSDHeader->getControlInfo()))
                {
                    notification->setAddress(udpDataIndication->getSrcAddr());
                } else
                {
                    throw cRuntimeError("Could not determine any address for service");
                }
            }
            emit(_subscribeEventGroupSignal, notification);
        }
        optionsListIterator++;
    }
}

void SomeIpSD::processSubscribeEventGroupAckEntry(SomeIpSDEntry *subscribeEventGroupAckEntry, SomeIpSDHeader* someIpSDHeader) {
    std::list<SomeIpSDOption*> optionsList = someIpSDHeader->getEncapOptions();
    std::_List_iterator<SomeIpSDOption*> optionsListIterator = optionsList.begin();

    SomeIpDiscoveryNotification* notification = new SomeIpDiscoveryNotification();
    notification->setServiceId(subscribeEventGroupAckEntry->getServiceID());
    notification->setInstanceId(subscribeEventGroupAckEntry->getInstanceID());

    std::advance(optionsListIterator, subscribeEventGroupAckEntry->getIndex1stOptions());
    for (int firstOptionsIdx = 0; firstOptionsIdx < subscribeEventGroupAckEntry->getNum1stOptions(); firstOptionsIdx++) {

        notification->updateFromEndpointOption(*optionsListIterator);
        optionsListIterator++;
    }

    if(notification->getAddress().isUnspecified()) {
        // use udp src of someip sd message instead
        if(inet::UDPDataIndication *udpDataIndication = dynamic_cast<inet::UDPDataIndication*>(someIpSDHeader->getControlInfo())) {
            notification->setAddress(udpDataIndication->getSrcAddr());
        } else {
            throw cRuntimeError("Could not determine any address for service");
        }
    }
    emit(_subscribeEventGroupAckSignal, notification);
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

IPv4EndpointOption* SomeIpSD::createIpv4Endpoint(
        SomeIpDiscoveryNotification* someIpDiscoveryNotification,
        QoSGroup qosGroup) {
    if (qosGroup == QoSGroup::UNDEFINED) {
        qosGroup = someIpDiscoveryNotification->getQoSGroup();
    }
    IPv4EndpointOption *ipv4EndpointOption;
    switch (qosGroup) {
        case QoSGroup::SOMEIP_TCP:
            ipv4EndpointOption = new IPv4EndpointOption();
            ipv4EndpointOption->setIpv4Address(this->_localAddress.toIPv4());
            ipv4EndpointOption->setL4Protocol(IPProtocolId::IP_PROT_TCP);
            ipv4EndpointOption->setPort(someIpDiscoveryNotification->getTcpPort());
            break;
        case QoSGroup::SOMEIP_UDP:
            ipv4EndpointOption = new IPv4EndpointOption();
            ipv4EndpointOption->setIpv4Address(this->_localAddress.toIPv4());
            ipv4EndpointOption->setL4Protocol(IPProtocolId::IP_PROT_UDP);
            ipv4EndpointOption->setPort(someIpDiscoveryNotification->getUdpPort());
            break;
        case QoSGroup::SOMEIP_UDP_MCAST:
            if(someIpDiscoveryNotification->getMcastDestAddr().isUnspecified()
                    || someIpDiscoveryNotification->getMcastDestPort() < 0) {
                throw cRuntimeError("MCast QoS selected but no mcast endpoint information set.");
            }
            ipv4EndpointOption = new IPv4MulticastOption();
            ipv4EndpointOption->setIpv4Address(someIpDiscoveryNotification->getMcastDestAddr().toIPv4());
            ipv4EndpointOption->setL4Protocol(IPProtocolId::IP_PROT_UDP);
            ipv4EndpointOption->setPort(someIpDiscoveryNotification->getMcastDestPort());
            break;
        default:
            throw cRuntimeError("Unknown QoSGroup");
    }
    return ipv4EndpointOption;
}

void SomeIpSD::sendTo(SomeIpSDHeader* someIpSDHeader, inet::L3Address destIp, int destPort) {
    if(destIp.isUnspecified()) {
        destIp = this->_mcastDestAddress;
    }
    if(destPort == -1) {
        destPort = this->destPort;
    }
    socket.sendTo(someIpSDHeader, destIp, destPort);
}

} /* end namespace SOA4CoRE */
