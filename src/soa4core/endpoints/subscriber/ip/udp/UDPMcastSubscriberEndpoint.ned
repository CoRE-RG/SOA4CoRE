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

package soa4core.endpoints.subscriber.ip.udp;

import soa4core.endpoints.subscriber.ip.udp.UDPSubscriberEndpoint;

//
// The UDPMcastSubscriberEndpoint is a subscriber endpoint module with IP qos (@see~EndpointBase).
// It realizes an UDP specific subscriber endpoint.
// Subscribers are created by the QoSNegotiationProtocol module (@see~QoSNegotiationProtocol)
// during runtime to connect realise a subscribing endpoint for a service. 
// Subscribers are connected to a specific transport and a subscriber connector module (@see~SubscriberConnector)
// which loosely couples the subscriber endpoint with a subscriber application (@see~Subscriber). 
//
// @author Timo Haeckel for HAW Hamburg
//
simple UDPMcastSubscriberEndpoint extends UDPSubscriberEndpoint
{
    parameters:
        @class(SOA4CoRE::UDPMcastSubscriberEndpoint);
        // The QoS group this endpoint serves
    	qos = default("IP_UDP_MCAST");
    
        // Mcast IP address for UDP socket
        string mcastDestAddress @mutable;
        // Mcast port for UDP socket
        int mcastDestPort @mutable;
}
