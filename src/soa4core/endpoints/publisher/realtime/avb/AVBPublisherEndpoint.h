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

#ifndef __SOA4CORE_AVBPUBLISHERENDPOINT_H_
#define __SOA4CORE_AVBPUBLISHERENDPOINT_H_

#include "soa4core/endpoints/publisher/realtime/base/RTPublisherEndpointBase.h"
//INET
#include <inet/linklayer/common/MACAddress.h>
//CoRE4INET
#include <core4inet/base/avb/AVBDefs.h>
#include <core4inet/services/avb/SRP/SRPTable.h>

namespace SOA4CoRE {

/**
 * @brief The AVBPublisherEndpoint is a publisher endpoint module with RT qos (@see~EndpointBase).
 * It realizes an AVB specific publishing endpoint.
 * Publishers are created by the QoSNegotiationProtocol module (@see~QoSNegotiationProtocol)
 * during runtime to connect realise a publishing endpoint for a service.
 * Publishers are connected to a specific transport and a publisher connector module (@see~PublisherConnector)
 * which loosely couples the publisher endpoint with a publisher application (@see~Publisher).
 *
 * @ingroup soa4core/endpoints
 *
 * @author Timo Haeckel and Mehmet Mueller for HAW Hamburg
 */
class AVBPublisherEndpoint : public RTPublisherEndpointBase, public virtual cListener
{
public:

    /**
     * Creates a connection specific information for this endpoint.
     * @return  the connection specific information.
     */
    virtual ConnectionSpecificInformation* getConnectionSpecificInformation() override;

protected:
    virtual void initialize() override;
    virtual void handleParameterChange(const char* parname) override;

    /**
     * @brief Receives Stream Reservation Protocol signals
     */
    virtual void receiveSignal(cComponent *src, simsignal_t id, cObject *obj, cObject *details) override;

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
     * Cached reference to the SRP Table to register a talker and be notified about new listeners.
     */
    CoRE4INET::SRPTable *_srpTable;

    /**
     * Output module for AVB traffic.
     */
    cModule *_avbOutCTC;

    /**
     * Cached auto generated multicast address of the Stream.
     */
    inet::MACAddress _multicastMAC;

    /**
     * Number of frames per interval.
     */
    uint16_t _intervalFrames;

    /**
     * The class of the AVB stream.
     */
    CoRE4INET::SR_CLASS _srClass;

    /**
     * Stream ID.
     */
    unsigned long _streamID;

    /**
     * The vlan id of the stream.
     */
    uint16_t _vlanID;

    /**
     * The pcp of the stream.
     */
    uint16_t _pcp;

    /**
     * Calculated framesize.
     */
    size_t _frameSize;
};

} /*end namespace SOA4CoRE*/

#endif
