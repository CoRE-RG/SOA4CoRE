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

package soa4core.applications.base;

import soa4core.applications.base.IService;

//
// The ServiceBase provides a base implementation of the common
// interface for all soa4core application modules.
// These modules realize a data publisher or subscriber service.
//
// @author Timo Haeckel for HAW Hamburg
//
simple ServiceBase like IService
{
    parameters:
        // name of the serviceManager module
        string serviceManagerModulePath = default (".middleware.sm");
    	// local IP address
    	string localAddress = default("");
    	// the tcp port for this service
    	int applicationTCPPort;
    	// the udp port for this service
    	int applicationUDPPort;
    	
        // PublisherApp enabled or disabled
        bool enabled = default(true);
        // Start time
        double startTime @unit(s) = default(0s);
        // Stop time <= 0 means   
        double stopTime @unit(s) = default(0s);
    	
    	// The instance id of the service that will be subscribed/offered
        int instanceID; // see [PRS_SOMEIPSD_00268], [PRS_SOMEIPSD_00270], [PRS_SOMEIPSD_00351]
    	//Service ID that will be subscribed/offered
    	int serviceId = default(0);
    	//Service Description
    	string serviceName = default("");
    	
    	// Optional deadline for messages. 
    	// Can be set for both publishers and subscribers to be communicated
    	// during connection setup and, e.g., forwarded to the network.
		double deadline @unit(s) = default(0s); // deadline <= 0 means no deadline    
		
		// Skip discovery and use app parameters to create static endpoint
		// note this might not be supported for all QoS types
		bool isStatic = default(false);
		    
    gates:
        // the connector gate to receive messages.
        input connectorIn @direct;
}
