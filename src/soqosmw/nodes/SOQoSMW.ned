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

package soqosmw.nodes;

import soqosmw.qosmanagement.negotiation.QoSNegotiationProtocol;
import soqosmw.discovery.base.IServiceDiscovery;
import soqosmw.discovery.someipservicediscovery.SomeIpSD;
import soqosmw.servicemanager.base.ILocalServiceManager;
import soqosmw.serviceregistry.base.IServiceRegistry;

import soqosmw.connector.pubsub.writer.PublisherConnector;
import soqosmw.connector.pubsub.reader.SubscriberConnector;

import soqosmw.endpoints.base.IEndpoint;

//
// This compound module groups all SOQoSMW Components into on module.
// All modules created during runtime will be integrated to this compound module.
//
// @author Timo Haeckel and Mehmet Cakir for HAW Hamburg
//

module SOQoSMW
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

        @display("i=block/layer");
    gates:
        //connection for the quality of service negotiation module to udp
        output qosnpUDPOut;
        //connection for the quality of service negotiation module from udp
        input qosnpUDPIn;

    submodules:

        qosnp: QoSNegotiationProtocol {
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

    connections:
        qosnp.udpOut --> qosnpUDPOut;
        qosnp.udpIn <-- qosnpUDPIn;
        
}
