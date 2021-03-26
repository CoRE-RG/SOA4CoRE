/*
 * QoSBrokerSomeIp.cpp
 *
 *  Created on: Mar 25, 2021
 *      Author: Mehmet Cakir
 */

#include <soqosmw/qosmanagement/negotiation/broker/QoSBrokerSomeIp.h>
#include "soqosmw/servicemanager/LocalServiceManager.h"
#include <inet/networklayer/common/L3AddressResolver.h>
#include "soqosmw/endpoints/publisher/someip/udp/SOMEIPUDPPublisherEndpoint.h"

namespace SOQoSMW {

QoSBrokerSomeIp::QoSBrokerSomeIp(QoSBroker* qosBroker) : _qosBroker(qosBroker) {
}

QoSBrokerSomeIp::~QoSBrokerSomeIp() {
}

bool QoSBrokerSomeIp::startSomeIpSD() {
    bool handled = false;
    find();
    handled = true;
    return handled;
}

bool QoSBrokerSomeIp::processSomeIpSDHeader(QoSNegotiationProtocolMsg *msg) {
    bool handled = false;
    SomeIpSDHeader *someIpSDHeader = dynamic_cast<SomeIpSDHeader*>(msg->decapsulate());
    std::list<SomeIpSDEntry*> entries = someIpSDHeader->getEncapEntries();
    for (std::list<SomeIpSDEntry*>::const_iterator it = entries.begin(); it != entries.end(); ++it) {
        switch ((*it)->getType()) {
            case SOQoSMW::SomeIpSDEntryType::FIND:
                EV << "FIND ARRIVED" << endl;
                handled = processFindEntry(*it, someIpSDHeader);
                break;
            case SOQoSMW::SomeIpSDEntryType::OFFER:
                EV << "OFFER ARRIVED" << endl;
                handled = processOfferEntry(*it, someIpSDHeader);
                break;
            case SOQoSMW::SomeIpSDEntryType::SUBSCRIBEVENTGROUP:
                EV << "SUBSCRIBEVENTGROUP ARRIVED" << endl;
                handled = processSubscribeEventGroupEntry(*it, someIpSDHeader);
                break;
            case SOQoSMW::SomeIpSDEntryType::SUBSCRIBEVENTGROUPACK:
                EV << "SUBSCRIBEVENTGROUPACK ARRIVED" << endl;
                handled = processSubscribeEventGroupAckEntry(*it, someIpSDHeader);
                break;
            default:
                EV << "Unknown type" << std::endl;
        }
    }
    delete someIpSDHeader;
    return handled;
}

void QoSBrokerSomeIp::find() {
    SomeIpSDHeader *someIpSDHeader = new SomeIpSDHeader("SOME/IP SD - FIND");

    ServiceEntry *findEntry = new ServiceEntry("ServiceEntry");
    findEntry->setType(SOQoSMW::SomeIpSDEntryType::FIND);
    findEntry->setIndex1stOptions(0x00);
    findEntry->setIndex2ndOptions(0x00);
    findEntry->setNum1stAnd2ndOptions(0x00);
    //findEntry->setServiceID(serviceID);
    //findEntry->setInstanceID(instanceID);
    findEntry->setMajorVersion_TTL(0x01000003);
    findEntry->setMinorVersion(0xFFFFFFFF);
    someIpSDHeader->encapEntry(findEntry);

    QoSNegotiationSomeIpFind *someIpFind = new QoSNegotiationSomeIpFind("QoSNegotiationSomeIpFind");
    someIpFind->setQosClass(QoSGroups::SOMEIP);
    _qosBroker->fillEnvelope(someIpFind);
    someIpFind->setByteLength(_qosBroker->getNegotiationMessageSize(someIpFind));
    someIpFind->encapsulate(someIpSDHeader);

    _qosBroker->sendMessage(someIpFind);
}

void QoSBrokerSomeIp::offer() {
    SomeIpSDHeader *someIpSDHeader = new SomeIpSDHeader("SOME/IP SD - OFFER");

    ServiceEntry *offerEntry = new ServiceEntry("ServiceEntry");
    offerEntry->setType(SOQoSMW::SomeIpSDEntryType::OFFER);
    offerEntry->setIndex1stOptions(0x00);
    offerEntry->setIndex2ndOptions(0x00);
    offerEntry->setNum1stAnd2ndOptions(0x01);
    //offerEntry->setServiceID(serviceID);
    //offerEntry->setInstanceID(instanceID);
    offerEntry->setMajorVersion_TTL(0x01000003);
    offerEntry->setMinorVersion(0xFFFFFFFF);
    someIpSDHeader->encapEntry(offerEntry);

    IPv4EndpointOption *ipv4EndpointOption = new IPv4EndpointOption("IPv4EndpointOption of Publisher");
    //ipv4EndpointOption->setIpv4Address(publisherIP.toIPv4());
    ipv4EndpointOption->setL4Protocol(IPProtocolId::IP_PROT_UDP);
    //ipv4EndpointOption->setPort(publisherPort);
    someIpSDHeader->encapOption(ipv4EndpointOption);

    QoSNegotiationSomeIpOffer *someIpOffer = new QoSNegotiationSomeIpOffer("QoSNegotiationSomeIpOffer");
    someIpOffer->setQosClass(QoSGroups::SOMEIP);
    _qosBroker->fillEnvelope(someIpOffer);
    someIpOffer->setByteLength(_qosBroker->getNegotiationMessageSize(someIpOffer));
    someIpOffer->encapsulate(someIpSDHeader);

    _qosBroker->sendMessage(someIpOffer);

}

void QoSBrokerSomeIp::subscribeEventgroup(inet::L3Address subscriberIP, uint16_t subscriberPort) {
    SomeIpSDHeader *someIpSDHeader = new SomeIpSDHeader("SOME/IP SD - SUBSCRIBEEVENTGROUP");

    EventgroupEntry *subscribeEventgroupEntry = new EventgroupEntry("EventgroupEntry");
    subscribeEventgroupEntry->setType(SOQoSMW::SomeIpSDEntryType::SUBSCRIBEVENTGROUP);
    subscribeEventgroupEntry->setIndex1stOptions(0);
    subscribeEventgroupEntry->setIndex2ndOptions(0);
    subscribeEventgroupEntry->setNum1stAnd2ndOptions(0x01);
    //subscribeEventgroupEntry->setServiceID(serviceID);
    //subscribeEventgroupEntry->setInstanceID(instanceID);
    subscribeEventgroupEntry->setMajorVersion_TTL(0x01000003);
    someIpSDHeader->encapEntry(subscribeEventgroupEntry);

    IPv4EndpointOption *ipv4EndpointOption = new IPv4EndpointOption("IPv4EndpointOption of Subscriber");
    ipv4EndpointOption->setIpv4Address(subscriberIP.toIPv4());
    ipv4EndpointOption->setL4Protocol(IPProtocolId::IP_PROT_UDP);
    ipv4EndpointOption->setPort(subscriberPort);
    someIpSDHeader->encapOption(ipv4EndpointOption);

    QoSNegotiationSomeIpSubscribe *someIpSubscribe = new QoSNegotiationSomeIpSubscribe("QoSNegotiationSomeIpSubscribe");
    someIpSubscribe->setQosClass(QoSGroups::SOMEIP);
    _qosBroker->fillEnvelope(someIpSubscribe);
    someIpSubscribe->setByteLength(_qosBroker->getNegotiationMessageSize(someIpSubscribe));
    someIpSubscribe->encapsulate(someIpSDHeader);

    _qosBroker->sendMessage(someIpSubscribe);
}

void QoSBrokerSomeIp::subscribeEventgroupAck() {
    SomeIpSDHeader *someIpSDHeader = new SomeIpSDHeader("SOME/IP SD - SUBSCRIBEEVENTGROUPACK");

    EventgroupEntry *subscribeEventgroupAckEntry = new EventgroupEntry("EventgroupEntry");
    subscribeEventgroupAckEntry->setType(SOQoSMW::SomeIpSDEntryType::SUBSCRIBEVENTGROUPACK);
    subscribeEventgroupAckEntry->setIndex1stOptions(0);
    subscribeEventgroupAckEntry->setIndex2ndOptions(0);
    subscribeEventgroupAckEntry->setNum1stAnd2ndOptions(0x01);
    //subscribeEventgroupAckEntry->setServiceID(serviceID);
    //subscribeEventgroupAckEntry->setInstanceID(instanceID);
    subscribeEventgroupAckEntry->setMajorVersion_TTL(0x01000003);
    someIpSDHeader->encapEntry(subscribeEventgroupAckEntry);

    IPv4EndpointOption *ipv4EndpointOption = new IPv4EndpointOption("IPv4EndpointOption of Publisher");
    //ipv4EndpointOption->setIpv4Address(publisherIP.toIPv4());
    ipv4EndpointOption->setL4Protocol(IPProtocolId::IP_PROT_UDP);
    //ipv4EndpointOption->setPort(publisherPort);
    someIpSDHeader->encapOption(ipv4EndpointOption);

    QoSNegotiationSomeIpSubscribeAck *someIpSubscribeAck = new QoSNegotiationSomeIpSubscribeAck("QoSNegotiationSomeIpSubscribeAck");
    someIpSubscribeAck->setQosClass(QoSGroups::SOMEIP);
    _qosBroker->fillEnvelope(someIpSubscribeAck);
    someIpSubscribeAck->setByteLength(_qosBroker->getNegotiationMessageSize(someIpSubscribeAck));
    someIpSubscribeAck->encapsulate(someIpSDHeader);

    _qosBroker->sendMessage(someIpSubscribeAck);
}

bool QoSBrokerSomeIp::processFindEntry(SomeIpSDEntry *findEntry, SomeIpSDHeader* someIpSDHeader) {
    bool handled = false;
    offer();
    handled = true;
    return handled;
}

bool QoSBrokerSomeIp::processOfferEntry(SomeIpSDEntry *offerEntry, SomeIpSDHeader* someIpSDHeader) {
    bool handled = false;
    ConnectionSpecificInformation* connectionlessCSI = new CSI_SOMEIP();
    SubscriberEndpointBase* sub = _qosBroker->_lsm->createOrFindSubscriberFor(_qosBroker->_remote.getPath(),connectionlessCSI);
    delete connectionlessCSI;
    CSI_SOMEIP *subCSI = dynamic_cast<CSI_SOMEIP*>(sub->getConnectionSpecificInformation());
    subscribeEventgroup(inet::L3AddressResolver().resolve(subCSI->getAddress()), subCSI->getPort());
    delete subCSI;
    handled = true;
    return handled;
}

bool QoSBrokerSomeIp::processSubscribeEventGroupEntry(SomeIpSDEntry *subscribeEventGroupEntry, SomeIpSDHeader* someIpSDHeader) {
    bool handled = false;
    IPv4EndpointOption* ipv4EndpointOption = dynamic_cast<IPv4EndpointOption*>(someIpSDHeader->decapOption());
    SOMEIPUDPPublisherEndpoint* pub = dynamic_cast<SOMEIPUDPPublisherEndpoint*>(_qosBroker->_lsm->createOrFindPublisherFor(_qosBroker->_local.getPath(),QoSGroups::SOMEIP));
    CSI_SOMEIP *subCSI = new CSI_SOMEIP();
    subCSI->setAddress(ipv4EndpointOption->getIpv4Address().str().c_str());
    subCSI->setPort(ipv4EndpointOption->getPort());
    pub->addRemote(subCSI);
    delete subCSI;
    delete ipv4EndpointOption;
    subscribeEventgroupAck();
    handled = true;
    return handled;
}

bool QoSBrokerSomeIp::processSubscribeEventGroupAckEntry(SomeIpSDEntry *subscribeEventGroupAckEntry, SomeIpSDHeader* someIpSDHeader) {
    bool handled = false;
    handled = true;
    return handled;
}

} /* namespace SOQoSMW */
