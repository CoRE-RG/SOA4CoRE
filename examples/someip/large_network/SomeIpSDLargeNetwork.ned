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

package soa4core.examples.SomeIpSDLargeTestNetwork;

import inet.node.ethernet.EtherLink;
import inet.networklayer.configurator.ipv4.IPv4NetworkConfigurator;

network SomeIpSDLargeTestNetwork
{
    parameters:
    types:
        channel ETH_100MBIT extends EtherLink
        {
            datarate = 100Mbps;
            length = 5.0m;
        }

    submodules:
        HostNorth: HostNorth {
            @display("p=493,50");
        }
        
        HostSouth: HostSouth {
            @display("p=493,376");
        }

        HostEast: HostEast {
            @display("p=935,213");
        }

        HostWest: HostWest {
            @display("p=71,213");
        }
        
        HostNorthWest: HostNorthWest {
            @display("p=253,50");
        }
        
        HostNorthEast: HostNorthEast {
            @display("p=734,50");
        }
        
        HostSouthWest: HostSouthWest {
            @display("p=253,376");
        }
        
        HostSouthEast: HostSouthEast {
            @display("p=734,376");
        }
        
        SwitchWest: SwitchWest {
            @display("p=253,212");
        }

        SwitchEast: SwitchEast {
            @display("p=734,212");
        }

        SwitchCenter: SwitchCenter {
            @display("p=493,212");
        }

        configurator: IPv4NetworkConfigurator {
            @display("p=1009,384");
        }
        
    connections:
        HostNorthWest.ethg <--> ETH_100MBIT <--> SwitchWest.ethg++;
        HostWest.ethg <--> ETH_100MBIT <--> SwitchWest.ethg++;
        HostSouthWest.ethg <--> ETH_100MBIT <--> SwitchWest.ethg++;

        HostNorth.ethg <--> ETH_100MBIT <--> SwitchCenter.ethg++;
        HostSouth.ethg <--> ETH_100MBIT <--> SwitchCenter.ethg++;

        HostNorthEast.ethg <--> ETH_100MBIT <--> SwitchEast.ethg++;
        HostEast.ethg <--> ETH_100MBIT <--> SwitchEast.ethg++;
        HostSouthEast.ethg <--> ETH_100MBIT <--> SwitchEast.ethg++;
        
        SwitchWest.ethg++ <--> ETH_100MBIT <--> SwitchCenter.ethg++;
        SwitchEast.ethg++ <--> ETH_100MBIT <--> SwitchCenter.ethg++;
}
