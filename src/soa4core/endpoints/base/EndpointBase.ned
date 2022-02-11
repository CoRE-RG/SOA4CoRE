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
// along with this program. If not, see http://www.gnu.org/licenses/.
// 

package soa4core.endpoints.base;

import soa4core.utility.processing.ProcessingTimeSimulation;

//
// The EndpointBase provides a common interface for all endpoint modules.
// Endpoints are created by the QoSNegotiationProtocol module (@see~QoSNegotiationProtocol)
// during runtime to connect realise a local endpoint for a service. 
// Endpoints are connected to a specific transport and a connector module (@see~ConnectorBase)
// which loosely couples an endpoint to an application (@see~ServiceBase). 
//
// @author Timo Haeckel and Mehmet Mueller for HAW Hamburg
//
simple EndpointBase extends ProcessingTimeSimulation like IEndpoint
{
    parameters:
        // indicates which qos is realized in this endpoint one of RT, STD_UDP, STD_TCP, WEB
        string qos = default("");  
        
     	//The name of the endpoint as a path (e.g. "Reifendruck/links").
    	string endpointPath = default("");
    	
    	processingTime = default(0s);
        allowParallelProcessing = default(true);
        
        // Signal emitted when connector will be created
        @signal[endpointCreationTime](type=simtime_t);
        //Statistic of the simtime
        @statistic[endpointCreatedAt](title="Endpoint creation time"; source=endpointCreationTime; record=sum; unit=s);
    
    gates:
        // input gate for messegas from the application
        input connectorIn @directIn;
        
        // incoming traffic from transport layer        
        input transportIn @loose;
        // outgoing traffic to transport layer
        output transportOut @loose;
}
