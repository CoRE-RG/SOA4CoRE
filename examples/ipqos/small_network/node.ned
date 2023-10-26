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

package soa4core.examples.ipqos.small_network;

import soa4core.nodes.MiddlewareTSNHost;

module Node extends MiddlewareTSNHost
{
    parameters:           
        hasQoSNP = default(false);
        middleware.sm.typename = "SomeIpManager";
        middleware.sd.typename = "SomeIpSD";
		middleware.**.multicastInterface = "eth0";
		middleware.sm.initialDelayMin = 0ms;
		middleware.sm.initialDelayMax = 0ms;
		middleware.sm.repetitionsMax = 3;
		middleware.sm.repetitionBaseDelay = 1s;
		middleware.sm.cyclicOfferDelay = 7s;
        middleware.sd.includeIEEE8021QConfig = default(true);
        middleware.sd.includeRessourceConfig = default(true);
        middleware.sd.includeRealTimeConfig = default(true);
		
	submodules:
	connections:


}