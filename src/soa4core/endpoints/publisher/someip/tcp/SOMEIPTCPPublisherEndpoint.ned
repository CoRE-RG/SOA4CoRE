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

package soa4core.endpoints.publisher.someip.tcp;

import soa4core.endpoints.publisher.standard.tcp.TCPPublisherEndpoint;

//
// The SOMEIPTCPPublisherEndpoint is a publisher endpoint module for SOME/IP traffic via TCP.
//
// @author Mehmet Mueller for HAW Hamburg
//
simple SOMEIPTCPPublisherEndpoint extends TCPPublisherEndpoint
{
    parameters:
        @class(SOA4CoRE::SOMEIPTCPPublisherEndpoint);
        // The endpoint path or name
        endpointPath = default("SOME/IP Publisher");
        // The QoS group this endpoint serves
        qos = default("SOMEIP_TCP");
}
