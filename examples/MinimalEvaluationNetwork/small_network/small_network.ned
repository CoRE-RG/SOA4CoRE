package soa4core.examples.MinimalEvaluationNetwork.small_network;

import inet.node.ethernet.EtherLink;
import inet.networklayer.configurator.ipv4.IPv4NetworkConfigurator;

network small_network
{
parameters:
     @display("bgb=,,white");
     double measure_interval @unit(s) = default(1s);

types: 
 channel ETH_1000MBIT extends EtherLink
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

  Subscriber: Subscriber {
    parameters: 
      @display("p=580,162;i=block/sink;is=l");
}

  App1: App1 {
    parameters: 
      @display("p=108,71;i=block/source;is=l");
}

  App2: App2 {
    parameters: 
      @display("p=490,71;i=block/sink;is=l");
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
  App1.ethg <--> ETH_1000MBIT <--> switch1.ethg[1];
  switch1.ethg[2] <--> ETH_1000MBIT <--> switch2.ethg[0];
  App2.ethg <--> ETH_1000MBIT <--> switch2.ethg[1];
  Subscriber.ethg <--> ETH_1000MBIT <--> switch2.ethg[2];

}