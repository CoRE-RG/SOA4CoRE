package soa4core.examples.minimal_evaluation.medium_network;

import core4inet.nodes.ethernet.avb_AS6802.TTEAVBEtherHost;
import inet.node.ethernet.EtherLink;
import inet.networklayer.configurator.ipv4.IPv4NetworkConfigurator;

network MinimalEvaluationMediumNetwork
{
    parameters:
        @display("bgb=,,white");
        double measure_interval @unit(s) = default(1s);

    types:
        channel ETH_1GBIT extends EtherLink
        {
            datarate = 1000000000bps;
            length = 0.0m;
            ber = 0.0;
            per = 0.0;
        }

    submodules:
        Publisher: Publisher {
            parameters:
                @display("p=30,162;i=block/source;is=l");
        }

        SubscriberTCP: SubscriberTCP {
            parameters:
                @display("p=580,71;i=block/sink;is=l");
        }

        SubscriberUDP: SubscriberUDP {
            parameters:
                @display("p=580,162;i=block/sink;is=l");
        }

        SubscriberAVB: SubscriberAVB {
            parameters:
                @display("p=580,254;i=block/sink;is=l");
        }

        App1: App1 {
            parameters:
                @display("p=134,50;is=l");
        }

        App2: App2 {
            parameters:
                @display("p=446,50;is=l");
        }

        switch1: switch1 {
            parameters:
                @display("p=205,162;is=l");
            gates:
                ethg[3];
        }

        switch2: switch2 {
            parameters:
                @display("p=386,162;is=l");
            gates:
                ethg[5];
        }

        configurator: IPv4NetworkConfigurator;
    connections:
        Publisher.ethg <--> ETH_1GBIT <--> switch1.ethg[0];
        App1.ethg <--> ETH_1GBIT <--> switch1.ethg[1];
        switch1.ethg[2] <--> ETH_1GBIT <--> switch2.ethg[0];
        App2.ethg <--> ETH_1GBIT <--> switch2.ethg[1];
        SubscriberTCP.ethg <--> ETH_1GBIT <--> switch2.ethg[2];
        SubscriberUDP.ethg <--> ETH_1GBIT <--> switch2.ethg[3];
        SubscriberAVB.ethg <--> ETH_1GBIT <--> switch2.ethg[4];

}
