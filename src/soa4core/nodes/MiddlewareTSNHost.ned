//
// c Timo Haeckel for HAW Hamburg
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

import core4inet.nodes.inet.TSNIPHost;

import soa4core.nodes.Middleware;
import soa4core.applications.base.IService;

//
// The MiddlewareHost module
//
// @author Timo Haeckel for HAW Hamburg
//

module MiddlewareTSNHost extends TSNIPHost
{
    parameters:
        hasUdp = true; //needed for qosnp
        hasTcp = true; //needed for tcp based communication
        bool hasQoSNP = default(true);
        *.hasQoSNP = hasQoSNP;
        int numServices = default(0);
        string localAddress = default("");
        *.localAddress = localAddress;
        @display("i=device/device");
    submodules:

        services[numServices]: <> like IService {
            @display("p=1071,65,m,5,50,50");
        }

        middleware: Middleware {
            @display("p=966,65");
        }

    connections allowunconnected:

        middleware.qosnpUDPOut --> udp.appIn++ if hasQoSNP;
        middleware.qosnpUDPIn <-- udp.appOut++ if hasQoSNP;

        middleware.sdUDPOut --> udp.appIn++;
        middleware.sdUDPIn <-- udp.appOut++;

}
