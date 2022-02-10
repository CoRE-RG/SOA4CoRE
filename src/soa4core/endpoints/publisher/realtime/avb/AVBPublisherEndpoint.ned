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

package soa4core.endpoints.publisher.realtime.avb;

import soa4core.endpoints.publisher.realtime.base.RTPublisherEndpointBase;

//
// The AVBPublisherEndpoint is a publisher endpoint module with RT qos (@see~EndpointBase).
// It realizes an AVB specific publishing endpoint.
// Publishers are created by the QoSNegotiationProtocol module (@see~QoSNegotiationProtocol)
// during runtime to connect realise a publishing endpoint for a service. 
// Publishers are connected to a specific transport and a publisher connector module (@see~PublisherConnector)
// which loosely couples the publisher endpoint with a publisher application (@see~PublisherAppBase). 
//
// @author Timo Haeckel for HAW Hamburg
//
simple AVBPublisherEndpoint extends RTPublisherEndpointBase
{
    parameters:
        @class(SOA4CoRE::AVBPublisherEndpoint);

        // ID of the AVB Stream
        int streamID = default(0);     
        // Stream Reservation Class of the AVB Stream
        string srClass = default("A");
        // Number of frames per interval
        int intervalFrames = default(1);        
        // VLAN identifier of the AVB Stream
        int vlan_id = default(2);
        // payload size to calculate reserved bandwidth
        int payload @unit(Byte) = default(46Byte);
}
