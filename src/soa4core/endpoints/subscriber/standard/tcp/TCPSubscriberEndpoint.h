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

#ifndef __SOA4CORE_TCPSUBSCRIBERENDPOINT_H_
#define __SOA4CORE_TCPSUBSCRIBERENDPOINT_H_

#include "soa4core/endpoints/subscriber/standard/base/STDSubscriberEndpointBase.h"
//INET
#include <inet/transportlayer/contract/tcp/TCPSocket.h>

namespace SOA4CoRE {
/**
 * @brief The TCPSubscriberEndpoint is a subscriber endpoint module with STD qos (@see~EndpointBase).
 * It realizes an TCP specific subscriber endpoint.
 * Subscribers are created by the QoSNegotiationProtocol module (@see~QoSNegotiationProtocol)
 * during runtime to connect realise a subscribing endpoint for a service.
 * Subscribers are connected to a specific transport and a subscriber connector module (@see~SubscriberConnector)
 * which loosely couples the subscriber endpoint with a subscriber application (@see~Subscriber).
 *
 * @ingroup soa4core/endpoints
 *
 * @author Timo Haeckel and Mehmet Mueller for HAW Hamburg
 */
class TCPSubscriberEndpoint : public STDSubscriberEndpointBase
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
     * @brief Indicates a parameter has changed.
     *
     * @param parname Name of the changed parameter or nullptr if multiple parameter changed.
     */
    virtual void handleParameterChange(const char* parname) override;

    /**
     * Handles messages incoming from transport gate and
     * forwards them to the connector if they are for applications.
     *
     * @param msg   the message to handle.
     */
    virtual void handleTransportIn(cMessage *msg) override;

    /**
     * Is called during module initialization to initialize the transport connection;
     */
    virtual void initializeTransportConnection() override;

private:

/**
 * Member variables
 */
public:
protected:
    /**
     * The TCP socket to receive on.
     */
    inet::TCPSocket _socket;

    /**
     * The local IP address.
     */
    std::string _localAddress;

    /**
     * The local TCP port.
     */
    int _localPort;

    /**
     * The remote IP address.
     */
    std::string _remoteAddress;

    /**
     * The remote TCP port.
     */
    int _remotePort;
private:
};

} /*end namespace SOA4CoRE*/

#endif
