//
// c Timo Haeckel for HAW Hamburg
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

#ifndef __SOA4CORE_AVBSUBSCRIBERENDPOINT_H_
#define __SOA4CORE_AVBSUBSCRIBERENDPOINT_H_

#include "soa4core/endpoints/subscriber/realtime/base/RTSubscriberEndpointBase.h"
//CoRE4INET
#include <core4inet/services/avb/SRP/SRPTable.h>

namespace SOA4CoRE {
/**
 * @brief The AVBSubscriberEndpoint is a subscriber endpoint module with RT qos (@see~EndpointBase).
 * It realizes an AVB specific subscriber endpoint.
 * Subscribers are created by the QoSNegotiationProtocol module (@see~QoSNegotiationProtocol)
 * during runtime to connect realise a subscribing endpoint for a service.
 * Subscribers are connected to a specific transport and a subscriber connector module (@see~SubscriberConnector)
 * which loosely couples the subscriber endpoint with a subscriber application (@see~Subscriber).
 *
 * @ingroup soa4core/endpoints
 *
 * @author Timo Haeckel and Mehmet Mueller for HAW Hamburg
 */
class AVBSubscriberEndpoint : public RTSubscriberEndpointBase, public cListener
{
/**
 * Methods
 */
public:

    /**
     * Creates a connection specific information for this endpoint.
     * @return  the connection specific information.
     */
    virtual ConnectionSpecificInformation* getConnectionSpecificInformation() override;

protected:
    /**
     * Initializes the module
     */
    virtual void initialize() override;

    /**
     * Handles the incoming message
     * @param msg the message
     */
    virtual void handleMessage(cMessage *msg) override;

    /**
     * @brief Indicates a parameter has changed.
     *
     * @param parname Name of the changed parameter or nullptr if multiple parameter changed.
     */
    virtual void handleParameterChange(const char* parname) override;

    /**
    * Processes the scheduled message. Needs to delete the message after handling it.
    * @param msg  the incoming message
    */
   virtual void processScheduledMessage(cMessage *msg) override;

    /**
     * Receives Stream Reservation Protocol signals
     */
    virtual void receiveSignal(cComponent *src, simsignal_t id, cObject *obj, cObject *details) override;

    /**
     * Is called during module initialization to initialize the transport connection;
     */
    virtual void initializeTransportConnection() override;

    /**
     * Schedules the update message at the given time, that will retry/update the registration.
     *
     * @param at    the time to schedule the event at.
     */
    virtual void scheduleUpdateMessage(SimTime at);

private:

/**
 * Member variables
 */
public:
protected:
    /**
     * Cached reference to the SRP Table to register a talker and be notified about new listeners.
     */
    CoRE4INET::SRPTable *_srpTable;

    /**
     * Update message.
     */
    cMessage* _updateMessage;

    /**
     * Stream update interval.
     */
    simtime_t _updateInterval;

    /**
     * Retry interval if registration fails.
     */
    simtime_t _retryInterval;

    /**
     * Stream ID.
     */
    unsigned long _streamID;

    /**
     * VLAN ID of the stream.
     */
    uint16_t _vlanID;
private:
};

} /*end namespace SOA4CoRE*/

#endif
