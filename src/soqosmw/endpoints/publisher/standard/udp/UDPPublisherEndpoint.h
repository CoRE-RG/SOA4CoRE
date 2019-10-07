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

#ifndef __SOQOSMW_UDPPUBLISHERENDPOINT_H_
#define __SOQOSMW_UDPPUBLISHERENDPOINT_H_

#include "soqosmw/endpoints/publisher/standard/base/STDPublisherEndpointBase.h"

//STD
#include <string>
#include <vector>
#include <utility>
//INET
#include <inet/transportlayer/contract/udp/UDPSocket.h>
#include <inet/networklayer/common/L3Address.h>


namespace SOQoSMW {

/**
 * @brief The UDPPublisherEndpoint is a publisher endpoint module with STD qos (@see~EndpointBase).
 * It realizes an UDP specific publishing endpoint.
 * Publishers are created by the QoSNegotiationProtocol module (@see~QoSNegotiationProtocol)
 * during runtime to connect realise a publishing endpoint for a service.
 * Publishers are connected to a specific transport and a publisher connector module (@see~PublisherConnector)
 * which loosely couples the publisher endpoint with a publisher application (@see~PublisherAppBase).
 *
 * @ingroup soqosmw/endpoints
 *
 * @author Timo Haeckel and Mehmet Cakir for HAW Hamburg
 */
class UDPPublisherEndpoint : public STDPublisherEndpointBase
{
public:
    /**
     * Creates a connection specific information for this endpoint.
     * @return  the connection specific information.
     */
    virtual ConnectionSpecificInformation* getConnectionSpecificInformation() override;

    /**
     * Adds a new UDP remote to send to.
     * @param csi   the connection specific information of the new remote.
     */
    void addRemote(ConnectionSpecificInformation* csi);

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

    /**
     * UDP server socket
     */
    inet::UDPSocket _serverSocket;

    /**
     * Local ip address for UDP socket
     */
    std::string _localAddress;

    /**
     * Local port for UDP socket
     */
    int _localPort;

    /**
     * Remotes.
     */
    std::vector<std::pair<inet::L3Address, int>> _remotes;
};

} /*end namespace SOQoSMW*/

#endif
