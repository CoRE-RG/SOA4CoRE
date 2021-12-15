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

package soqosmw.applications.base;

import soqosmw.applications.base.ISOQoSMWApplication;

//
// The SOQoSMWApplicationBase provides a base implementation of the common
// interface for all soqosmw application modules.
// These modules realize a data publisher or subscriber service.
//
// @author Timo Haeckel for HAW Hamburg
//
simple SOQoSMWApplicationBase like ISOQoSMWApplication
{
    parameters:
        // name of the serviceManager module
        string serviceManagerModulePath = default (".soqosmw.lsm");
    	// local IP address
    	string localAddress = default("");
    	// the tcp port for this service
    	int applicationTCPPort;
    	// the udp port for this service
    	int applicationUDPPort;
    	// The instance id of the service that will be subscribed
        int instanceID = default(0xFFFF);
        
    gates:
        // the connector gate to receive messages.
        input connectorIn @direct;
}
