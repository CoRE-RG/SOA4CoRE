//
// c Timo Haeckel, for HAW Hamburg
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

package soa4core.nodes;

import soa4core.qosmanagement.negotiation.QoSNegotiationProtocol;
import soa4core.discovery.base.IServiceDiscovery;
import soa4core.discovery.someipservicediscovery.SomeIpSD;
import soa4core.servicemanager.base.ILocalServiceManager;
import soa4core.serviceregistry.base.IServiceRegistry;

import soa4core.connector.pubsub.writer.PublisherConnector;
import soa4core.connector.pubsub.reader.SubscriberConnector;

import soa4core.endpoints.base.IEndpoint;

//
// This compound module groups all SOA4CoRE Components into on module.
// All modules created during runtime will be integrated to this compound module.
//
// @author Timo Haeckel and Mehmet Mueller for HAW Hamburg
//

module Middleware
{
    parameters:
        string interfaceTableModule;
        *.interfaceTableModule = interfaceTableModule;
        string localAddress = default("");
        *.localAddress = localAddress;
        *.lsmmoduleName = "lsm";
        *.sdmoduleName = "sd";
        *.qosnpmoduleName = "qosnp";
        *.lsrmoduleName = "lsr";
        bool hasQoSNP = default(true);
        *.hasQoSNP = hasQoSNP;

        @display("i=block/layer");
    gates:
        //connection for the quality of service negotiation module to udp
        output qosnpUDPOut;
        //connection for the quality of service negotiation module from udp
        input qosnpUDPIn;
        
        //connection for the SOME/IP SD module to udp
        output sdUDPOut;
        //connection for the SOME/IP SD module from udp
        input sdUDPIn;

    submodules:

        qosnp: QoSNegotiationProtocol if hasQoSNP {
            @display("p=160,50");
        }

        lsm: <default("LocalServiceManager")> like ILocalServiceManager {
            @display("p=100,50");
        }

        lsr: <default("LocalServiceRegistry")> like IServiceRegistry {
            @display("p=239,50");
        }

        sd: <default("StaticServiceDiscovery")> like IServiceDiscovery {
            @display("p=40,50");
        }

		// placeholder for publisher connector modules created during runtime.
        publisherConnectors[0]: PublisherConnector {
            @display("p=300,100,c,50");
        }

		// placeholder for subscriber connector modules created during runtime.
        subscriberConnectors[0]: SubscriberConnector {
            @display("p=200,100,c,50");
        }

        // placeholder for subscriber endpoints created during runtime.
        subscriberEndpoints[0]: <> like IEndpoint {
            @display("p=100,100,c,50");
        }

        // placeholder for avb publisher endpoints created during runtime.
        publisherEndpoints[0]: <> like IEndpoint {
            @display("p=400,100,c,50");
        }

    connections allowunconnected:
        qosnp.udpOut --> qosnpUDPOut if hasQoSNP;
        qosnp.udpIn <-- qosnpUDPIn if hasQoSNP;
}