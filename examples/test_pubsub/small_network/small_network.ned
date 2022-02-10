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

package soa4core.examples.test_pubsub.small_network;

import core4inet.nodes.ethernet.avb_AS6802.TTEAVBEtherSwitch;
import soa4core.nodes.MiddlewareHost;

import inet.networklayer.configurator.ipv4.IPv4NetworkConfigurator;
import inet.node.ethernet.Eth100M;

//
// Small sample network with three hosts and one switch that shows how to 

// configure the TTEthernet Model. Configuration in omnetpp.ini

// - Uses 5ms Cycletime

// - 80ns Tick length
//
network small_network
{
    @display("bgb=721,473,white,black");
    submodules:
        Publisher: MiddlewareHost {
            @display("p=49,408;i=block/source;is=l");
        }

        RTSubscriber: MiddlewareHost {
            @display("p=365,30;i=block/sink,#FF8080,50;is=l");
        }

        STDSubscriber: MiddlewareHost {
            @display("p=634,400;i=block/sink,#0080FF;is=l");
        }

        Switch: TTEAVBEtherSwitch {
            parameters:
                @display("p=365,236;is=l");
            gates:
                ethg[3];
        }

        configurator: IPv4NetworkConfigurator {
            @display("p=89,22");
        }
    connections:
        //
        // The example uses a simple star topology
        //
        Publisher.ethg <--> Eth100M {  length = 20m; } <--> Switch.ethg[0];
        RTSubscriber.ethg <--> Eth100M {  length = 10m; } <--> Switch.ethg[1];
        STDSubscriber.ethg <--> Eth100M {  length = 5m; } <--> Switch.ethg[2];
}
