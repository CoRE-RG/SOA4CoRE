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

    //TODO List of publishers
    /**
     * The SomeipPublisher
     */
    SomeipPublisher* _someipPublisher;

    //TODO List of subscribers
    /**
     * The SomeipSubscriber
     */
    SomeipSubscriber* _someipSubscriber;
public:
    /**
     * Discovers a service
     */
    void find(uint16_t serviceID, uint16_t instanceID);

    /**
     * Registers a SomeipPublisher
     * @param someipPublisher
     */
    void registerPublisher(SomeipPublisher* someipPublisher);

    /**
     * Registers a SomeipSubscriber
     * @param someipSubscriber
     */
    void registerSubscriber(SomeipSubscriber* someipSubscriber);
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
     * Offers a service
     */
    void offer(uint16_t serviceID, uint16_t instanceID, inet::L3Address remoteAddress, int remotePort);

    /**
     * Subscribes a service
     */
    void subscribeEventgroup(uint16_t serviceID, uint16_t instanceID, inet::L3Address remoteAddress, int remotePort);

    /**
     * Acknowledges a subscription
     */
    void subscribeEventgroupAck(uint16_t serviceID, uint16_t instanceID, inet::L3Address remoteAddress, int remotePort);

    /**
     * Processes a packet
     *
     * @param packet
     */
    void processPacket(SomeIpSDHeader* someipSDHeader);

    void processFindEntry(SomeIpSDEntry *findEntry, SomeIpSDHeader* someipSDHeader);

    void processOfferEntry(SomeIpSDEntry *offerEntry, SomeIpSDHeader* someipSDHeader);

    void processSubscribeEventGroupEntry(SomeIpSDEntry *subscribeEventGroupEntry, SomeIpSDHeader* someipSDHeader);

    void processSubscribeEventGroupAckEntry(SomeIpSDEntry *subscribeEventGroupAckEntry, SomeIpSDHeader* someipSDHeader);
};
} /* end namespace SOQoSMW */
#endif
