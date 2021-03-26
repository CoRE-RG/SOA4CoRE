/*
 * QoSBrokerSomeIp.h
 *
 *  Created on: Mar 25, 2021
 *      Author: Mehmet Cakir
 */

#ifndef SOQOSMW_QOSMANAGEMENT_NEGOTIATION_BROKER_QOSBROKERSOMEIP_H_
#define SOQOSMW_QOSMANAGEMENT_NEGOTIATION_BROKER_QOSBROKERSOMEIP_H_

#include <soqosmw/qosmanagement/negotiation/broker/QoSBroker.h>
#include <soqosmw/messages/qosnegotiation/QoSNegotiationProtocol_m.h>
#include "soqosmw/messages/someip/SomeIpSDHeader.h"

namespace SOQoSMW {

class QoSBrokerSomeIp {
public:
    QoSBrokerSomeIp(QoSBroker* qosBroker);
    virtual ~QoSBrokerSomeIp();

    /**
     * Processes an encapsulated SomeIpSDHeader
     * @param msg
     * @return result whether it was successful
     */
    bool processSomeIpSDHeader(QoSNegotiationProtocolMsg *msg);

    /**
     * Starts the SOME/IP service discovery
     * @return result whether it was successful
     */
    bool startSomeIpSD();
private:
    QoSBroker* _qosBroker;

    /**
     * Discovers a service
     */
    void find();

    /**
     * Offers a service
     */
    void offer();

    /**
     * Subscribes a service
     * @param subscriberIP
     * @param subscriberPort
     */
    void subscribeEventgroup(inet::L3Address subscriberIP, uint16_t subscriberPort);

    /**
     * Acknowledges a subscription
     */
    void subscribeEventgroupAck();

    /**
     * Processes a SOME/IP SD Find entry
     * @param findEntry
     * @param someIpSDHeader
     * @return result whether it was successful
     */
    bool processFindEntry(SomeIpSDEntry *findEntry, SomeIpSDHeader* someIpSDHeader);

    /**
     * Processes a SOME/IP SD Offer entry
     * @param offerEntry
     * @param someIpSDHeader
     * @return result whether it was successful
     */
    bool processOfferEntry(SomeIpSDEntry *offerEntry, SomeIpSDHeader* someIpSDHeader);

    /**
     * Processes a SOME/IP SD Subscribe Eventgroup entry
     * @param subscribeEventGroupEntry
     * @param someIpSDHeader
     * @return result whether it was successful
     */
    bool processSubscribeEventGroupEntry(SomeIpSDEntry *subscribeEventGroupEntry, SomeIpSDHeader* someIpSDHeader);

    /**
     * Processes a SOME/IP SD Subscribe Eventgroup Acknowledge entry
     * @param subscribeEventGroupAckEntry
     * @param someIpSDHeader
     * @return result whether it was successful
     */
    bool processSubscribeEventGroupAckEntry(SomeIpSDEntry *subscribeEventGroupAckEntry, SomeIpSDHeader* someIpSDHeader);

};

} /* namespace SOQoSMW */

#endif /* SOQOSMW_QOSMANAGEMENT_NEGOTIATION_BROKER_QOSBROKERSOMEIP_H_ */
