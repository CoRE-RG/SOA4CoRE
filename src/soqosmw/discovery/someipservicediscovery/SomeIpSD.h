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
#include "soqosmw/messages/someip/SomeIpSDEntry_m.h"

namespace SOQoSMW {

/**
 * @brief Basic SomeIpSD
 *
 * @ingroup soqosmw/discovery
 *
 * @author Mehmet Cakir
 */
class SomeIpSD : public virtual inet::UDPBasicApp
{
public:
protected:
private:

public:
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
     * Searches a service in the network
     */
    void findService(uint16_t serviceID, uint16_t instanceID);

    /**
     * Acknowledges subscription
     * @param serviceID
     * @param instanceID
     * @param publisherIP
     * @param publisherPort
     * @param remoteAddress
     */
    void acknowledgeSubscription(uint16_t serviceID, uint16_t instanceID, inet::L3Address publisherIP, uint16_t publisherPort, inet::L3Address remoteAddress);
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
     * Processes a SOME/IP SD Offer entry
     * @param offerEntry
     * @param someIpSDHeader
     */
    void processOfferEntry(SomeIpSDEntry *offerEntry, SomeIpSDHeader* someIpSDHeader);

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
};
} /* end namespace SOQoSMW */
#endif
