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

package soa4core.endpoints.publisher.standard.base;

import soa4core.endpoints.publisher.base.PublisherEndpointBase;

//
// The STDPublisherEndpointBase is a publisher endpoint module with STD qos (@see~EndpointBase).
// Publishers are created by the QoSNegotiationProtocol module (@see~QoSNegotiationProtocol)
// during runtime to connect realise a publishing endpoint for a service. 
// Publishers are connected to a specific transport and a publisher connector module (@see~PublisherConnector)
// which loosely couples the publisher endpoint with a publisher application (@see~PublisherAppBase). 
//
// @author Timo Haeckel and Mehmet Mueller for HAW Hamburg
//
simple STDPublisherEndpointBase extends PublisherEndpointBase
{
    parameters:
        endpointPath = default("RTPublisher");
}
