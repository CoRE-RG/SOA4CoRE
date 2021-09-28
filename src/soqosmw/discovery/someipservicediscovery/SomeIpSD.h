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

#ifndef __SOQOSMW_SOMEIPSDAPP_H_
#define __SOQOSMW_SOMEIPSDAPP_H_

#define BROADCASTADDRESS "255.255.255.255"

#include "inet/applications/udpapp/UDPBasicApp.h"
#include "soqosmw/messages/someip/SomeIpSDHeader.h"
#include "soqosmw/discovery/base/IServiceDiscovery.h"
#include "soqosmw/messages/someip/SomeIpSDEntry_m.h"
#include <soqosmw/discovery/someipservicediscovery/SomeIpSDFindResult.h>

namespace SOQoSMW {

/**
 * @brief Basic SomeIpSD
 *
 * @ingroup soqosmw/discovery
 *
 * @author Mehmet Cakir
 */
class SomeIpSD : public IServiceDiscovery, public virtual inet::UDPBasicApp, public cListener
{

  /**
   * Methods
   */
  public:
    /**
     * Initiates a service discovery
     * @param serviceIdentifier
     */
    void discover(IServiceIdentifier& serviceIdentifier) override;

    /**
     * Subscribes a service
     * @param serviceID
     * @param instanceID
     * @param publisherIP
     * @param subscriberIP
     * @param subscriberPort
     */
    void subscribeService(uint16_t serviceID, uint16_t instanceID, inet::L3Address publisherIP, inet::L3Address subscriberIP, uint16_t subscriberPort);

    /**
     * Acknowledges subscription
     * @param serviceID
     * @param instanceID
     * @param publisherIP
     * @param publisherPort
     * @param remoteAddress
     */
    void acknowledgeSubscription(uint16_t serviceID, uint16_t instanceID, inet::L3Address publisherIP, uint16_t publisherPort, inet::L3Address remoteAddress);

    /**
     * @brief Receives discovery response
     * @param source
     * @param signalID
     * @param obj
     * @param details
     */
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj, cObject *details) override;
  protected:
    /**
     * Initializes module with stages
     *
     * @param stage indicates the initialization stage
     */
    virtual void initialize(int stage) override;

    /**
     * Handles incoming message as soon as node is up and
     * responds with a new message
     *
     * @param msg
     */
    virtual void handleMessageWhenUp(cMessage *msg) override;

    /**
     * Is used just to override because there is no use for this
     * @param doneCallback
     * @return true
     */
    virtual bool handleNodeStart(IDoneCallback *doneCallback) override;

  private:
    /**
     * Discovers a service
     */
    void find(uint16_t serviceID, uint16_t instanceID);

    /**
     * Offers a service
     */
    void offer(uint16_t serviceID, uint16_t instanceID, inet::L3Address remoteAddress, inet::L3Address publisherIP, uint16_t publisherPort);

    /**
     * Subscribes a service
     */
    void subscribeEventgroup(uint16_t serviceID, uint16_t instanceID, inet::L3Address remoteAddress, inet::L3Address subscriberIP, uint16_t subscriberPort);

    /**
     * Acknowledges a subscription
     */
    void subscribeEventgroupAck(uint16_t serviceID, uint16_t instanceID, inet::L3Address publisherIP, uint16_t publisherPort, inet::L3Address remoteAddress);

    /**
     * Processes a SOME/IP SD Header
     * @param someIpSDHeader
     */
    void processSomeIpSDHeader(SomeIpSDHeader* someIpSDHeader);

    /**
     * Processes a SOME/IP SD Find entry
     * @param findEntry
     * @param someIpSDHeader
     */
    void processFindEntry(SomeIpSDEntry *findEntry, SomeIpSDHeader* someIpSDHeader);

    /**
     * Processes
     * @param obj the SOME/IP SD header container
     */
    void processFindResult(cObject* obj);

    /**
     * Processes a SOME/IP SD Offer entry
     * @param offerEntry
     * @param someIpSDHeader
     */
    void processOfferEntry(SomeIpSDEntry *offerEntry, SomeIpSDHeader* someIpSDHeader);

    /**
     * Processes a subscription
     * @param obj the subscription information
     */
    void processSubscription(cObject* obj);

    /**
     * Processes the acknowledgment of a subscription
     * @param obj the service about to acknowledged
     */
    void processAcknowledgment(cObject *obj);

    /**
     * Processes a SOME/IP SD Subscribe Eventgroup entry
     * @param subscribeEventGroupEntry
     * @param someIpSDHeader
     */
    void processSubscribeEventGroupEntry(SomeIpSDEntry *subscribeEventGroupEntry, SomeIpSDHeader* someIpSDHeader);

    /**
     * Processes a SOME/IP SD Subscribe Eventgroup Acknowledge entry
     * @param subscribeEventGroupAckEntry
     * @param someIpSDHeader
     */
    void processSubscribeEventGroupAckEntry(SomeIpSDEntry *subscribeEventGroupAckEntry, SomeIpSDHeader* someIpSDHeader);

/**
 * Member variables
 */
  public:
  protected:
    /**
     * The signal which is emitted when a find is received
     */
    omnetpp::simsignal_t _serviceFindSignal;

    /**
     * The signal which is emitted when a offer is received
     */
    omnetpp::simsignal_t _serviceOfferSignal;

    /**
     * The signal which is emitted when a subscribeEventGroup is received
     */
    omnetpp::simsignal_t _subscribeEventGroupSignal;

    /**
     * The signal which is emitted when a subscribeEventGroupAck is received
     */
    omnetpp::simsignal_t _subscribeEventGroupAckSignal;

  private:
    /**
     * The local ip address
     */
    inet::L3Address _localAddress;
};
} /* end namespace SOQoSMW */
#endif
