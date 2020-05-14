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
#define SOMEIPSDIDX 0

#include "soqosmw/applications/someipapp/base/SomeipAppBase.h"
#include "soqosmw/messages/someip/SomeIpSDHeader.h"

namespace SOQoSMW {

class SomeipPublisher;
class SomeipSubscriber;
class SomeipLocalServiceManager;

/**
 * @brief Basic SomeipSDApp
 *
 * @ingroup soqosmw/applications
 *
 * @author Mehmet Cakir
 */
class SomeipSD : public virtual SomeipAppBase
{
public:
protected:
private:
    /**
     * The local ip address
     */
    const char* _localAddress;

    //TODO will not be used, delete later
    /**
     * The SomeipPublisher
     */
    SomeipPublisher* _someipPublisher;

    //TODO will not be used, delete later
    /**
     * The SomeipSubscriber
     */
    SomeipSubscriber* _someipSubscriber;

    /**
     * The SOME/IP local service manager
     */
    SomeipLocalServiceManager *_someipLSM;

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
     * @param remoteAddress
     * @param remotePort
     */
    void acknowledgeSubscription(uint16_t serviceID, uint16_t instanceID, L3Address remoteAddress);
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
    void offer(uint16_t serviceID, uint16_t instanceID, inet::L3Address remoteAddress, L3Address publisherIP, uint16_t publisherPort);

    /**
     * Subscribes a service
     */
    void subscribeEventgroup(uint16_t serviceID, uint16_t instanceID, inet::L3Address remoteAddress, L3Address subscriberIP, uint16_t subscriberPort);

    /**
     * Acknowledges a subscription
     */
    void subscribeEventgroupAck(uint16_t serviceID, uint16_t instanceID, inet::L3Address remoteAddress);

    /**
     * Processes a packet
     *
     * @param packet
     */
    void processSomeipSDHeader(SomeIpSDHeader* someipSDHeader);

    void processFindEntry(SomeIpSDEntry *findEntry, SomeIpSDHeader* someipSDHeader);

    void processOfferEntry(SomeIpSDEntry *offerEntry, SomeIpSDHeader* someipSDHeader);

    void processSubscribeEventGroupEntry(SomeIpSDEntry *subscribeEventGroupEntry, SomeIpSDHeader* someipSDHeader);

    void processSubscribeEventGroupAckEntry(SomeIpSDEntry *subscribeEventGroupAckEntry, SomeIpSDHeader* someipSDHeader);
};
} /* end namespace SOQoSMW */
#endif
