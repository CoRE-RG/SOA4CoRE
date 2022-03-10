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

package soa4core.discovery.static;

import soa4core.discovery.base.IServiceDiscovery;

//
// Provides information about all services in the network.
//
// @author Timo Haeckel for HAW Hamburg
//
//
simple StaticServiceDiscovery like IServiceDiscovery
{
    parameters:
        @class(SOA4CoRE::StaticServiceDiscovery);
        // The Signal which is emitted when a service is discovered
        @signal[serviceOfferSignal];
        //this parameter holds the information of all services in the network,
        //so each node knows where the services are located.
        //Style:
        //<service name="servicename" address="ip or node (if autoconfig)" id="service id">
        //</service>
        xml services;
        @display("i=block/table2");
}
