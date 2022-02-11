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

package soa4core.endpoints.publisher.base;

import soa4core.endpoints.base.EndpointBase;

//
// The PublisherEndpointBase implements the common interface for all endpoint modules (@see~EndpointBase).
// Publishers are created by the QoSNegotiationProtocol module (@see~QoSNegotiationProtocol)
// during runtime to connect realise a publishing endpoint for a service. 
// Publishers are connected to a specific transport and a publisher connector module (@see~PublisherConnector)
// which loosely couples the publisher endpoint with a publisher application (@see~Publisher). 
//
// @author Timo Haeckel and Mehmet Mueller for HAW Hamburg
//
simple PublisherEndpointBase extends EndpointBase
{
    parameters:
        endpointPath = default("Publisher");
        @display("i=block/departure");
        
        //Signal emitted when a message is published
        @signal[msgSent](type=omnetpp::cMessage);
        //Statistic of the amount of packets that were sent by the publisher
        @statistic[msgCount](title="Sent Packets"; source=msgSent; record=count; unit=packets);
        //Statistic of the amount of bytes that were sent by the publisher
        @statistic[msgBytes](title="Sent Bytes"; source=msgSent; unit=B; record="sum(packetBytes)","vector(packetBytes)", "histogram(packetBytes)?","floatingIntervalSumVector(packetBytes)?"; interpolationmode=none); //possible parameter e.g.: **.floatingintervalvectorrecorder-measure-interval = 20ms for floatingIntervalRecorders
        //End-to-end latency
        @statistic[msgLatency](title="End-to-end latency"; source="messageAge(msgSent)"; unit=s; record=stats,histogram?,vector; interpolationmode=none);
        
        //Signal emitted when a message is dropped
        @signal[msgDrop](type=omnetpp::cMessage);
        //Statistic of the amount of packets that were sent by the publisher
        @statistic[msgDropCount](title="Sent Packets"; source=msgDrop; record=count; unit=packets);
        //Statistic of the amount of bytes that were sent by the publisher
        @statistic[msgDropBytes](title="Sent Bytes"; source=msgDrop; unit=B; record="sum(packetBytes)","vector(packetBytes)", "histogram(packetBytes)?","floatingIntervalSumVector(packetBytes)?"; interpolationmode=none); //possible parameter e.g.: **.floatingintervalvectorrecorder-measure-interval = 20ms for floatingIntervalRecorders
        //End-to-end latency
        @statistic[msgDropLatency](title="End-to-end latency"; source="messageAge(msgDrop)"; unit=s; record=stats,histogram?,vector; interpolationmode=none);
        
        //Signal emitted when a new remote connection is established
        @signal[remotes]();
        //Statistic of the total amount of remotes
        @statistic[remotesCount](title="Conencted remotes"; source=remotes; record=count);
        //Connection times
        @statistic[remotes](title="Remote connection times"; source="remotes"; unit=s; record=stats,histogram?,"vector(constant1)"; interpolationmode=none);
}
