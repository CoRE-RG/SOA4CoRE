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

package soa4core.nodes;

import signalsandgateways.gateway.GatewayBase;
import signalsandgateways.applications.can.CanGatewayNode;
import signalsandgateways.applications.ethernet.EthernetGatewayApplication;

//
// The MiddlewareGatewayHost module.
//
// @author Timo Haeckel for HAW Hamburg
//

module MiddlewareGatewayHost extends MiddlewareHost
{
    parameters:
        
        //Number of CAN bus ports
        int countOfCanBusses = default(1);
        //Number of Ethernet ports
        int countOfEthernetPorts = default(1);
    
    gates:
        //bus connection
        inout cang[countOfCanBusses];
    
     submodules:
     	//Application for the ethernet node in the gateway
        gatewayApp: EthernetGatewayApplication {
            @display("p=282,37");
        }
         
        //bus module
        can[countOfCanBusses]: CanGatewayNode {
            @display("p=369,100,c,130");
        }
        //gateway module
        gateway: GatewayBase {
            @display("p=224,100");
            numPorts = countOfCanBusses + countOfEthernetPorts + numServices;
        }
        
    connections:
        for i=0..countOfCanBusses-1 {
        	can[i].upperLayerOut --> gateway.lowerLayerIn++;
        	can[i].upperLayerIn <-- gateway.lowerLayerOut++;
       		can[i].gate <--> cang[i];
        }
        
        gatewayApp.upperLayerOut --> gateway.lowerLayerIn++;
        gatewayApp.upperLayerIn <-- gateway.lowerLayerOut++;
}