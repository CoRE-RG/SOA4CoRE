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

package soqosmw.examples.SomeipSDDummy;

import inet.node.inet.StandardHost;
import soqosmw.applications.someipapp.dummySD.SomeipPublisherNode;
import inet.node.ethernet.EtherLink;
import inet.networklayer.configurator.ipv4.IPv4NetworkConfigurator;
import inet.node.ethernet.EtherSwitch;

network SomeipSDDummy
{
    parameters:
    types:
        channel ETH_100MBIT extends EtherLink
        {
            datarate = 100Mbps;
            length = 5.0m;
        }

    submodules:
        pubnode: StandardHost;

        subnode: StandardHost {
            @display("p=210,110");
        }

        drittesRad: StandardHost {
            @display("p=283,38");
        }

        switch: EtherSwitch {
            @display("p=133,62");
        }

        configurator: IPv4NetworkConfigurator;

    connections:
        pubnode.ethg++ <--> ETH_100MBIT <--> switch.ethg++;
        subnode.ethg++ <--> ETH_100MBIT <--> switch.ethg++;
        drittesRad.ethg++ <--> ETH_100MBIT <--> switch.ethg++;
}
