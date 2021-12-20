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

package soqosmw.nodes;

import core4inet.nodes.inet.RTIPHost;

import soqosmw.nodes.SOQoSMW;
import soqosmw.applications.base.ISOQoSMWApplication;

//
// The SOQoSMWHost module
//
// @author Timo Haeckel for HAW Hamburg
//

module SOQoSMWHost extends RTIPHost
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

        services[numServices]: <> like ISOQoSMWApplication {
            @display("p=1071,65,m,5,50,50");
        }

        soqosmw: SOQoSMW {
            @display("p=966,65");
        }

    connections allowunconnected:

        soqosmw.qosnpUDPOut --> udp.appIn++ if hasQoSNP;
        soqosmw.qosnpUDPIn <-- udp.appOut++ if hasQoSNP;

        soqosmw.sdUDPOut --> udp.appIn++;
        soqosmw.sdUDPIn <-- udp.appOut++;

}
