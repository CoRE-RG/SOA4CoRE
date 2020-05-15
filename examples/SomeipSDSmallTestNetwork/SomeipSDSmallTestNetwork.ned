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

package soqosmw.examples.SomeipSDSmallTestNetwork;

import soqosmw.nodes.SomeIpHost;
import inet.node.ethernet.EtherLink;
import inet.networklayer.configurator.ipv4.IPv4NetworkConfigurator;
import inet.node.ethernet.EtherSwitch;

network SomeipSDSmallTestNetwork
{
    parameters:
    types:
        channel ETH_100MBIT extends EtherLink
        {
            datarate = 100Mbps;
            length = 5.0m;
        }

    submodules:
        pubnode: SomeIpHost {

            @display("p=30,30");
        }
        subnode: SomeIpHost {
            @display("p=172,147");
        }

        drittesRad: SomeIpHost {
            @display("p=315,30");
        }

        switch: EtherSwitch {
            @display("p=172,62");
        }

        configurator: IPv4NetworkConfigurator {

            @display("p=315,155");
        }
    connections:
        pubnode.ethg++ <--> ETH_100MBIT <--> switch.ethg++;
        subnode.ethg++ <--> ETH_100MBIT <--> switch.ethg++;
        drittesRad.ethg++ <--> ETH_100MBIT <--> switch.ethg++;
}
