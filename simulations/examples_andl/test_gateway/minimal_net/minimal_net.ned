package soa4core.simulations.examples_andl.test_gateway.minimal_net;

import core4inet.nodes.ethernet.avb_AS6802.TTEAVBEtherHost;
import inet.node.ethernet.EtherLink;
import fico4omnet.bus.can.CanBus;
import fico4omnet.nodes.can.CanNode;
import inet.networklayer.configurator.ipv4.IPv4NetworkConfigurator;

network minimal_net
{
parameters:
     @display("bgb=,,white");
     double measure_interval @unit(s) = default(1s);

types: 
 channel ETH_100MBIT extends EtherLink
  {
	datarate = 100000000bps;
	length = 5.0m;
	ber = 0.0;
	per = 0.0;
  }

submodules:
  InfotainmentSystem: InfotainmentSystem {
    parameters: 
      @display("p=207,70");
}

  GatewayLicht: GatewayLicht {
    parameters: 
      @display("p=69,184");
}

  GatewaySteuerung: GatewaySteuerung {
    parameters: 
      @display("p=362,184");
}

  Scheinwerfer: CanNode {
    parameters: 
      @display("p=69,29");
}

  Lichtregulierung: CanNode {
    parameters: 
      @display("p=362,29");
}

  ethswitch: ethswitch {
    parameters: 
      @display("p=207,184");
    gates:
      ethg[3];
  }

  Licht: CanBus  {
    parameters: 
      @display("p=69,109");
    gates:
        gate[2];
 }

  Steuerung: CanBus  {
    parameters: 
      @display("p=362,109");
    gates:
        gate[2];
 }

configurator: IPv4NetworkConfigurator;

connections:
  GatewayLicht.ethg[0] <--> ETH_100MBIT <--> ethswitch.ethg[0];
  GatewaySteuerung.ethg[0] <--> ETH_100MBIT <--> ethswitch.ethg[1];
  InfotainmentSystem.ethg <--> ETH_100MBIT <--> ethswitch.ethg[2];
  GatewayLicht.cang[0] <--> Licht.gate[0];
  GatewaySteuerung.cang[0] <--> Steuerung.gate[0];
  Scheinwerfer.gate <--> Licht.gate[1];
  Lichtregulierung.gate <--> Steuerung.gate[1];

}