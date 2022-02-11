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

#ifndef __SOA4CORE_SUBSCRIBERENDPOINTBASE_H_
#define __SOA4CORE_SUBSCRIBERENDPOINTBASE_H_

#include "soa4core/endpoints/base/EndpointBase.h"

namespace SOA4CoRE {

class SubscriberConnector;

/**
 * @brief The SubscriberEndpointBase implements the common interface for all endpoint modules (@see~EndpointBase).
 * Subscribers are created by the QoSNegotiationProtocol module (@see~QoSNegotiationProtocol)
 * during runtime to connect realise a subscribing endpoint for a service.
 * Subscribers are connected to a specific transport and a subscriber connector module (@see~SubscriberConnector)
 * which loosely couples the subscriber endpoint with a subscriber application (@see~Subscriber).
 *
 * @ingroup soa4core/endpoints
 *
 * @author Timo Haeckel and Mehmet Mueller for HAW Hamburg
 */
class SubscriberEndpointBase : public EndpointBase
{
    /**
     * Methods
     */
public:
    const SubscriberConnector* getSubscriberConnector() const;

    void setSubscriberConnector(SubscriberConnector* subscriberConnector);

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
     * Member variables
     */
public:
protected:
    /**
     * The connector to applications.
     */
    SubscriberConnector* _subscriberConnector;
private:
    /**
     * Signal to capture statistics for received messages
     */
    simsignal_t _msgRecv;
};

} /*end namespace SOA4CoRE*/

#endif
