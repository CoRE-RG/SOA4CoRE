package soa4core.examples.negotiationTestNetwork.TCP;

import core4inet.nodes.ethernet.avb_AS6802.TTEAVBEtherHost;
import inet.node.ethernet.EtherLink;
import inet.networklayer.configurator.ipv4.IPv4NetworkConfigurator;

network negotiationTestNetwork
{
    parameters:
        @display("bgb=,,white");
        double measure_interval @unit(s) = default(1s);

    types:
        channel ETH100MBIT extends EtherLink
        {
            datarate = 100000000bps;
            length = 5.0m;
            ber = 0.0;
            per = 0.0;
        }

    submodules:
        Publisher: Publisher {
            parameters:
                @display("p=30,147;i=block/source;is=l");
        }

        Subscriber: Subscriber {
            parameters:
                @display("p=730,147;i=block/sink;is=l");
        }

        switch1: switch1 {
            parameters:
                @display("p=380,70;is=l");
            gates:
                ethg[2];
        }

        configurator: IPv4NetworkConfigurator {

            @display("p=39,31");
        }
    connections:
        Publisher.ethg <--> ETH100MBIT <--> switch1.ethg[0];
        Subscriber.ethg <--> ETH100MBIT <--> switch1.ethg[1];

}
