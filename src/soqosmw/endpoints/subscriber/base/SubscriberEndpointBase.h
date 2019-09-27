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

#ifndef __SOQOSMW_SUBSCRIBERENDPOINTBASE_H_
#define __SOQOSMW_SUBSCRIBERENDPOINTBASE_H_

#include "soqosmw/endpoints/base/EndpointBase.h"

namespace soqosmw {

/**
 * @brief The SubscriberEndpointBase implements the common interface for all endpoint modules (@see~EndpointBase).
 * Subscribers are created by the QoSNegotiationProtocol module (@see~QoSNegotiationProtocol)
 * during runtime to connect realise a subscribing endpoint for a service.
 * Subscribers are connected to a specific transport and a subscriber connector module (@see~SubscriberConnector)
 * which loosely couples the subscriber endpoint with a subscriber application (@see~SubscriberAppBase).
 *
 * @ingroup soqosmw/endpoints
 *
 * @author Timo Haeckel for HAW Hamburg
 */
class SubscriberEndpointBase : public EndpointBase
{
protected:
    virtual void initialize() override;

    /**
     * Handles messages incoming from transport gate and
     * forwards them to the connector if they are for applications.
     *
     * @param msg   the message to handle.
     */
    virtual void handleTransportIn(cMessage *msg) override;

    /**
     * Handles messages incoming from the connector gate and
     * forwards them to the transport if needed.
     *
     * @param msg   the message to handle.
     */
    virtual void handleConnectorIn(cMessage *msg) override;

private:
    /**
     * Signal to capture statistics for received messages
     */
    simsignal_t _msgRecv;

};

} /*end namespace soqosmw*/

#endif
