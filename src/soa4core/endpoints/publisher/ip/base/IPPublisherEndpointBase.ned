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

package soa4core.endpoints.publisher.ip.base;

import soa4core.endpoints.publisher.base.PublisherEndpointBase;

//
// The IPPublisherEndpointBase is a publisher endpoint module with IP qos (@see~EndpointBase).
// Publishers are created by the QoSNegotiationProtocol module (@see~QoSNegotiationProtocol)
// during runtime to connect realise a publishing endpoint for a service. 
// Publishers are connected to a specific transport and a publisher connector module (@see~PublisherConnector)
// which loosely couples the publisher endpoint with a publisher application (@see~Publisher). 
//
// @author Timo Haeckel and Mehmet Mueller for HAW Hamburg
//
simple IPPublisherEndpointBase extends PublisherEndpointBase
{
    parameters:
        // The endpoint path or name
        endpointPath = default("IPPublisher");
        // Local ip address for socket
        string localAddress @mutable;
        // Local port for socket
        int localPort @mutable;
        // VLAN identifier of the stream
        int vlan_id = default(-1) @mutable; // if <0 no QHeader is used
        // Priority code point of the stream
        int pcp = default(-1) @mutable; // if <0 no QHeader is used 
        // Indicator if outgoing streams should be registered at the SRTable to be used by CBS
        bool registerStream = default(false) @mutable; // if true, register stream at SRTable
        bool streamIntervalAsCMI = default(false);  // calculate IdleSlope based on stream interval instead of CMI
        // Indicator if outgoing streams should be advertised to other nodes after SR
        // NOTE: advertise stream reservation is incompatible with streamIntervalAsCMI and may lead to overreservation and errors
        bool advertiseStreamRegistration = default(false) @mutable; // if true, advertise stream registration
        // the class measurement interval for the 802.1Q priorities
        // the key must be the pcp index (usually 0 - 7), see example below.  
        // A CMI <= 0 will not allow stream reservation in this priority.
        // Pcps that are unspecified will also not allow stream reservation.         
        object pcpCMI = default({3: 0s, 4: 0s, 5: 250us, 6: 125us, 7: 0s});
}
