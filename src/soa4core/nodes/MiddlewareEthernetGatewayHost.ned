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

import signalsandgateways.applications.ethernet.EthernetGatewayApplication;
import signalsandgateways.applications.ethernet.IEthernetGatewayHost;

import core4inet.incoming.AS6802.TTIncoming;
import core4inet.buffer.AS6802.TTQueueBuffer;
import core4inet.incoming.AS6802.RCIncoming;
import core4inet.buffer.AS6802.RCQueueBuffer;
import core4inet.buffer.base.BGQueueBuffer;

//
// The MiddlewareEthernetGatewayHost module.
//
// @author Timo Haeckel for HAW Hamburg
//

module MiddlewareEthernetGatewayHost extends MiddlewareHost like IEthernetGatewayHost
{
    parameters:
        //@display("bgb=688,409");

        int numberOfTT_Vlinks = default(0);
        int numberOfRC_Vlinks = default(0);

        @display("i=device/port");
    gates:
        output upperLayerOut;
        input upperLayerIn;

    submodules:
        //Application for the ethernet node in the gateway
        gatewayApp: EthernetGatewayApplication {
            @display("p=283,65");
        }

        //Vector of output buffers for timetriggered traffic(TT)
        TTvlinks[numberOfTT_Vlinks]: TTQueueBuffer {
            @display("p=308,386,r,70");
        }

        //Vector of input buffers for timetriggered traffic(TT)
        TTvlinks_ctc[numberOfTT_Vlinks]: TTIncoming {
            @display("p=308,315,r,70");
        }

        //Vector of output buffers for rate constrained traffic(RC)
        RCvlinks[numberOfRC_Vlinks]: RCQueueBuffer {
            parameters:
                @display("p=162,386,r,70");
        }

        //Vector of input buffers for rate constrained traffic(RC)
        RCvlinks_ctc[numberOfRC_Vlinks]: RCIncoming {
            parameters:
                @display("p=162,315,r,70");
        }
        
        // Best-efford buffer for incoming traffic 
        bgIn: BGQueueBuffer {
            @display("p=335,232");
        }

    connections:
        gatewayApp.upperLayerOut --> upperLayerOut;
        gatewayApp.upperLayerIn <-- upperLayerIn;
        for i=0..numberOfTT_Vlinks-1 {
            TTvlinks_ctc[i].out --> TTvlinks[i].in;
        }
        for i=0..numberOfRC_Vlinks-1 {
            RCvlinks_ctc[i].out --> RCvlinks[i].in;
        }
        
        bgIn.in <-- phy[0].bgOut;

}
