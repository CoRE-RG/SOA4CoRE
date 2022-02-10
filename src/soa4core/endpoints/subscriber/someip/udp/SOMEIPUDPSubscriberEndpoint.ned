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

package soa4core.endpoints.subscriber.someip.udp;

import soa4core.endpoints.subscriber.standard.udp.UDPSubscriberEndpoint;

//
// @author Mehmet Mueller
//
simple SOMEIPUDPSubscriberEndpoint extends UDPSubscriberEndpoint
{
    parameters:
        @class(SOA4CoRE::SOMEIPUDPSubscriberEndpoint);
        endpointPath = default("SOME/IP Subscriber");
        qos = default("SOMEIP_UDP");
}
