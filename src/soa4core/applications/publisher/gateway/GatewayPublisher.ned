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

package soa4core.applications.publisher.gateway;

import soa4core.applications.publisher.base.Publisher;

//
// The GatewayPublisher is a middleware application running on a gateway module.
// It is used to provide legacy bus data as a service.
//
// @author Timo Haeckel for HAW Hamburg
//
simple GatewayPublisher extends Publisher
{
    parameters:
        @class(SOA4CoRE::GatewayPublisher);
                
        //The CAN Data Frame ID(s) - String parameter (int) separated with commas
        string canIds = default("");		
	gates:
	    input gwIn;
    
}
