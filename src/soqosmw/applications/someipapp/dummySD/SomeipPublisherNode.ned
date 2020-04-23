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

package soqosmw.applications.someipapp.dummySD;

import soqosmw.applications.someipapp.dummySD.publisherapp.SomeipPublisher;
import soqosmw.applications.someipapp.base.SomeipSD;
import inet.node.ethernet.EtherLink;
import inet.node.inet.StandardHost;

module SomeipPublisherNode
{
    parameters:
    gates:
        inout ethg[2] @labels(EtherFrame-conn);
    submodules:
        publisher: StandardHost {
            @display("p=40,30");
        }
        sd: StandardHost {
            @display("p=112,30");
        }
    connections:
        ethg[0] <--> EtherLink {length=5m; datarate=100Mbps;} <--> publisher.ethg++;
        ethg[1] <--> EtherLink {length=5m; datarate=100Mbps;} <--> sd.ethg++;
}
