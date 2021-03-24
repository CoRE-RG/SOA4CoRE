package soqosmw.examples.SomeIpMinimal;

import core4inet.nodes.ethernet.avb_AS6802.TTEAVBEtherHost;
import inet.node.ethernet.EtherLink;
import inet.networklayer.configurator.ipv4.IPv4NetworkConfigurator;

network small_someip_network
{
parameters:
     @display("bgb=,,white");
     double measure_interval @unit(s) = default(1s);

types: 
 channel ETH_1000MBIT extends EtherLink
  {
	datarate = 1000000000bps;
	length = 5.0m;
	ber = 0.0;
	per = 0.0;
  }

submodules:
  Publisher: Publisher {
    parameters: 
      @display("p=30,162;i=block/source;is=l");
}

  Subscriber: Subscriber {
    parameters: 
      @display("p=580,162;i=block/sink;is=l");
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
      ethg[3];
  }

configurator: IPv4NetworkConfigurator;

connections:
  Publisher.ethg <--> ETH_1000MBIT <--> switch1.ethg[0];
  switch1.ethg[1] <--> ETH_1000MBIT <--> switch2.ethg[0];
  Subscriber.ethg <--> ETH_1000MBIT <--> switch2.ethg[1];

}