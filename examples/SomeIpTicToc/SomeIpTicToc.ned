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

package soa4core.examples.SomeIpTicToc;

import inet.node.ethernet.EtherLink;
import inet.networklayer.configurator.ipv4.IPv4NetworkConfigurator;
import inet.node.ethernet.EtherSwitch;
import inet.node.inet.StandardHost;

network SomeIpTicToc {
    parameters:
    types:
        channel ETH_100MBIT extends EtherLink {
            datarate = 100Mbps;
            length = 5.0m;
        }
    submodules:
        Tic: StandardHost {
        }
        
        Toc: StandardHost {
        }
        
        switch: EtherSwitch {
        }
        
        configurator: IPv4NetworkConfigurator {
        }
        
    connections:
        Tic.ethg++ <--> ETH_100MBIT <--> switch.ethg++;
        Toc.ethg++ <--> ETH_100MBIT <--> switch.ethg++;
}