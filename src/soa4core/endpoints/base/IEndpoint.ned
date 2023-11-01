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

package soa4core.endpoints.base;

//
// The interface module for the endpoint module.
//
// @author Timo Haeckel and Mehmet Mueller for HAW Hamburg
//

moduleinterface IEndpoint
{
    parameters:
        // indicates which qos is realized in this endpoint one of RT, IP_UDP, IP_TCP, WEB
        string qos;  
        
     	//The name of the endpoint as a path (e.g. "Reifendruck/links").
    	string endpointPath;
    
    gates:
        // input gate for messegas from the application
        input connectorIn @directIn;
        
        // incoming traffic from transport layer        
        input transportIn @loose;
        // outgoing traffic to transport layer
        output transportOut @loose;
}
