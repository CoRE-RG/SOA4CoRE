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

import inet.node.inet.StandardHost;
import soqosmw.discovery.someipservicediscovery.SomeIpSD;
import soqosmw.servicemanager.someipservicemanager.SomeIpLocalServiceManager;
import soqosmw.serviceregistry.someiplocalserviceregistry.SomeIpLocalServiceRegistry;
import soqosmw.applications.publisherapp.someip.SomeIpPublisher;
import soqosmw.applications.subscriberapp.someip.SomeIpSubscriber;

//
// The SomeIpHost provides a SOME/IP host.
//
// @author Mehmet Cakir
//
module SomeIpHost extends StandardHost
{
    parameters:
        // The SOME/IP host uses UDP for data transmission
        hasUdp = true;
        // The local address of the SOME/IP host
        string localAddress = default("");
        // The number of publishers on the SOME/IP host
        int numPublisher = default(0);
        // The number of subscribers on the SOME/IP host
        int numSubscriber = default(0);
        // Assign all submodules the same local address as the host
        *.localAddress = localAddress;

    submodules:
        // The SOME/IP publishers 
        publisher[numPublisher]: SomeIpPublisher {

            @display("p=455,28");
        }

        // The SOME/IP subscribers
        subscriber[numSubscriber]: SomeIpSubscriber {

            @display("p=379,28");
        }

        // The SOME/IP service discovery
        sd: SomeIpSD {

            @display("p=628,28;i=block/routing");
        }

        // The SOME/IP local service manager
        lsm: SomeIpLocalServiceManager {

            @display("p=681,28");
        }

        // The SOME/IP local service registry
        lsr: SomeIpLocalServiceRegistry {

            @display("p=732,28;i=block/table2");
        }

    connections:
        // Connect the udp gates of the service discovery
        // with the udp gates of the StandardHost
        sd.udpOut --> udp.appIn++;
        sd.udpIn <-- udp.appOut++;

        // Connect the udp gates of the publishers
        // with the udp gates of the StandardHost
        for i=0..numPublisher-1 {
            publisher[i].udpOut --> udp.appIn++;
            publisher[i].udpIn <-- udp.appOut++;
        }

        // Connect the udp gates of the subscribers
        // with the udp gates of the StandardHost
        for i=0..numSubscriber-1 {
            subscriber[i].udpOut --> udp.appIn++;
            subscriber[i].udpIn <-- udp.appOut++;
        }
}
