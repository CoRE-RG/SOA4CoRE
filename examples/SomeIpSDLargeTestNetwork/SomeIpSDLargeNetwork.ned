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

package soqosmw.examples.SomeIpSDLargeTestNetwork;

import soqosmw.nodes.SomeIpHost;
import inet.node.ethernet.EtherLink;
import inet.networklayer.configurator.ipv4.IPv4NetworkConfigurator;
import inet.node.ethernet.EtherSwitch;

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
        HostNorth: SomeIpHost {
            @display("p=493,50");
        }
        HostSouth: SomeIpHost {
            @display("p=493,376");
        }

        HostEast: SomeIpHost {
            @display("p=935,213");
        }

        HostWest: SomeIpHost {

            @display("p=71,213");
        }
        HostNorthWest: SomeIpHost {

            @display("p=253,50");
        }
        HostNorthEast: SomeIpHost {

            @display("p=734,50");
        }
        HostSouthWest: SomeIpHost {

            @display("p=253,376");
        }
        HostSouthEast: SomeIpHost {

            @display("p=734,376");
        }
        switchWest: EtherSwitch {
            @display("p=253,212");
        }

        switchEast: EtherSwitch {
            @display("p=734,212");
        }

        switchCenter: EtherSwitch {
            @display("p=493,212");
        }

        configurator: IPv4NetworkConfigurator {
            @display("p=1009,384");
        }
    connections:
        HostNorthWest.ethg++ <--> ETH_100MBIT <--> switchWest.ethg++;
        HostWest.ethg++ <--> ETH_100MBIT <--> switchWest.ethg++;
        HostSouthWest.ethg++ <--> ETH_100MBIT <--> switchWest.ethg++;

        HostNorth.ethg++ <--> ETH_100MBIT <--> switchCenter.ethg++;
        HostSouth.ethg++ <--> ETH_100MBIT <--> switchCenter.ethg++;

        HostNorthEast.ethg++ <--> ETH_100MBIT <--> switchEast.ethg++;
        HostEast.ethg++ <--> ETH_100MBIT <--> switchEast.ethg++;
        HostSouthEast.ethg++ <--> ETH_100MBIT <--> switchEast.ethg++;
        
        switchWest.ethg++ <--> ETH_100MBIT <--> switchCenter.ethg++;
        switchEast.ethg++ <--> ETH_100MBIT <--> switchCenter.ethg++;
}
