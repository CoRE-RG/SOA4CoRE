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

#ifndef __SOA4CORE_TCPPUBLISHERENDPOINT_H_
#define __SOA4CORE_TCPPUBLISHERENDPOINT_H_

#include "soa4core/endpoints/publisher/standard/base/STDPublisherEndpointBase.h"
//STD
#include <unordered_map>
#include <string>
//INET
#include <inet/transportlayer/contract/tcp/TCPSocket.h>
#include <inet/transportlayer/contract/tcp/TCPSocketMap.h>


namespace SOA4CoRE {

/**
 * @brief The TCPPublisherEndpoint is a publisher endpoint module with STD qos (@see~EndpointBase).
 * It realizes an TCP specific publishing endpoint.
 * Publishers are created by the QoSNegotiationProtocol module (@see~QoSNegotiationProtocol)
 * during runtime to connect realise a publishing endpoint for a service.
 * Publishers are connected to a specific transport and a publisher connector module (@see~PublisherConnector)
 * which loosely couples the publisher endpoint with a publisher application (@see~Publisher).
 *
 * @ingroup soa4core/endpoints
 *
 * @author Timo Haeckel and Mehmet Mueller for HAW Hamburg
 */
class TCPPublisherEndpoint : public STDPublisherEndpointBase, public inet::TCPSocketMap
{
/**
 * Methods
 */
public:
    virtual ~TCPPublisherEndpoint();

    /**
     * Creates a connection specific information for this endpoint.
     * @return  the connection specific information.
     */
    virtual ConnectionSpecificInformation* getConnectionSpecificInformation() override;

protected:
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

    /**
     * Publish a payload to all subscribers. This needs to be implemented by all subclasses.
     * The message will be deleted by the caller afterwards.
     *
     * @param payload   the payload to be published.
     */
    virtual void publish(cPacket* msg) override;

private:

/**
 * Member variables
 */
public:
protected:
    /**
     * Local ip address for TCP socket
     */
    std::string _localAddress;

    /**
     * Local port for TCP socket
     */
    int _localPort;

    /**
     * TCP server socket
     */
    inet::TCPSocket _serverSocket;
private:
};


} /*end namespace SOA4CoRE*/

#endif
