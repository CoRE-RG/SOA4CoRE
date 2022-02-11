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

package soa4core.endpoints.subscriber.base;

import soa4core.endpoints.base.EndpointBase;

//
// The SubscriberEndpointBase implements the common interface for all endpoint modules (@see~EndpointBase).
// Subscribers are created by the QoSNegotiationProtocol module (@see~QoSNegotiationProtocol)
// during runtime to connect realise a subscribing endpoint for a service. 
// Subscribers are connected to a specific transport and a subscriber connector module (@see~SubscriberConnector)
// which loosely couples the subscriber endpoint with a subscriber application (@see~Subscriber). 
//
// @author Timo Haeckel and Mehmet Mueller for HAW Hamburg
//
simple SubscriberEndpointBase extends EndpointBase
{
    parameters:
        endpointPath = default("Subscriber");
        @display("i=block/arrival");
        
        //Signal emitted when a message is published
        @signal[msgRecv](type=omnetpp::cMessage);
        //Statistic of the amount of packets that were sent by the publisher
        @statistic[msgCount](title="Sent Packets"; source=msgRecv; record=count; unit=packets);
        //Statistic of the amount of bytes that were sent by the publisher
        @statistic[msgBytes](title="Sent Bytes"; source=msgRecv; unit=B; record="sum(packetBytes)","vector(packetBytes)", "histogram(packetBytes)?","floatingIntervalSumVector(packetBytes)?"; interpolationmode=none); //possible parameter e.g.: **.floatingintervalvectorrecorder-measure-interval = 20ms for floatingIntervalRecorders
        //End-to-end latency
        @statistic[msgLatency](title="End-to-end latency"; source="messageAge(msgRecv)"; unit=s; record=stats,histogram?,vector; interpolationmode=none);
}
