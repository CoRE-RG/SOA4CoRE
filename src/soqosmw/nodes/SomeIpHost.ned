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
import soqosmw.applications.someipapp.someipservicediscovery.SomeipSD;
import soqosmw.applications.someipapp.someipservicemanager.SomeipLocalServiceManager;
import soqosmw.applications.someipapp.someiplocalserviceregistry.SomeipLocalServiceRegistry;
import soqosmw.applications.someipapp.someipSDTestApp.publisherapp.SomeipPublisher;
import soqosmw.applications.someipapp.someipSDTestApp.subscriberapp.SomeipSubscriber;


module SomeIpHost extends StandardHost
{
    parameters:
        hasUdp = true;
        string localAddress = default("");
        int numPublisher = default(0);
        int numSubscriber = default(0);

        *.localAddress = localAddress;

    submodules:

        publisher[numPublisher]: SomeipPublisher {

            @display("p=455,28");
        }

        subscriber[numSubscriber]: SomeipSubscriber {

            @display("p=379,28");
        }

        sd: SomeipSD {

            @display("p=628,28");
        }

        lsm: SomeipLocalServiceManager {

            @display("p=681,28");
        }

        lsr: SomeipLocalServiceRegistry {

            @display("p=732,28");
        }

    connections:
        sd.udpOut --> udp.appIn++;
        sd.udpIn <-- udp.appOut++;

        for i=0..numPublisher-1 {
            publisher[i].udpOut --> udp.appIn++;
            publisher[i].udpIn <-- udp.appOut++;
        }

        for i=0..numSubscriber-1 {
            subscriber[i].udpOut --> udp.appIn++;
            subscriber[i].udpIn <-- udp.appOut++;
        }


}
