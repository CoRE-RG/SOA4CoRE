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

#ifndef __SOA4CORE_UDPPUBLISHERENDPOINT_H_
#define __SOA4CORE_UDPPUBLISHERENDPOINT_H_

#include <soa4core/endpoints/publisher/ip/base/IPPublisherEndpointBase.h>
#include <string>
#include <vector>
#include <utility>
//INET
#include <inet/transportlayer/contract/udp/UDPSocket.h>
#include <inet/networklayer/common/L3Address.h>


namespace SOA4CoRE {

/**
 * @brief The UDPPublisherEndpoint is a publisher endpoint module with IP qos (@see~EndpointBase).
 * It realizes an UDP specific publishing endpoint.
 * Publishers are created by the QoSNegotiationProtocol module (@see~QoSNegotiationProtocol)
 * during runtime to connect realise a publishing endpoint for a service.
 * Publishers are connected to a specific transport and a publisher connector module (@see~PublisherConnector)
 * which loosely couples the publisher endpoint with a publisher application (@see~Publisher).
 *
 * @ingroup soa4core/endpoints
 *
 * @author Timo Haeckel and Mehmet Mueller for HAW Hamburg
 */
class UDPPublisherEndpoint : public IPPublisherEndpointBase
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

    /**
     * Adds a new UDP remote to send to.
     * @param csi   the connection specific information of the new remote.
     */
    void addRemote(ConnectionSpecificInformation* csi);

protected:

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
     * Checks if a filter must be set for the given connection specific information and
     * creates it if necessary.
     * @param csi   the connection specific information of the new remote.
     */
    virtual void checkAndCreateFilter(ConnectionSpecificInformation* csi);

    /**
     * Publish a payload to all subscribers. This needs to be implemented by all subclasses.
     * The message will be deleted by the caller afterwards.
     *
     * @param payload   the payload to be published.
     */
    virtual void publish(cPacket* msg) override;

    /**
     * Calculate the layer 1 frame size for the given payload adding all known headers.
     * @param payload
     * @return The sum of all header bytes and the payload.
     */
    virtual uint16_t calculateL2Framesize(uint16_t payload) override;

private:

/**
 * Member variables
 */
public:
protected:

    /**
     * UDP server socket
     */
    inet::UDPSocket _serverSocket;

    /**
     * Remotes.
     */
    std::vector<std::pair<inet::L3Address, int>> _remotes;
private:
};

} /*end namespace SOA4CoRE*/

#endif
