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

package soa4core.applications.subscriber.base;

import soa4core.applications.base.ServiceBase;

//
// The Subscriber provides a subscriber implementation of the middleware application base.
// These modules realize a data subscriber service.
//
// @author Timo Haeckel and Mehmet Mueller for HAW Hamburg
//
simple Subscriber extends ServiceBase
{
    parameters:
        @class(SOA4CoRE::Subscriber);
        @display("i=block/sink,blue");
        
        //Signal emitted when a frame is received, contains received EtherFrame
        @signal[rxPk](type=omnetpp::cPacket);
        //Statistic of the amount of packets that were received by the application
        @statistic[rxPk](title="Received Packets"; source=rxPk; record=count; unit=packets);
        //Statistic of the amount of bytes that were received by the application
        @statistic[rxBytes](title="Received Bytes"; source=rxPk; unit=B; record="sum(packetBytes)","vector(packetBytes)", "histogram(packetBytes)?","floatingIntervalSumVector(packetBytes)?"; interpolationmode=none); //possible parameter e.g.: **.floatingintervalvectorrecorder-measure-interval = 20ms for floatingIntervalRecorders
        //End-to-end latency
        @statistic[rxLatency](title="End-to-end latency"; source="messageAge(rxPk)"; unit=s; record=stats,histogram?,vector; interpolationmode=none);
        //Uses encapsulated packet | Frame when nothing is encapsulated
        @statistic[rxLatencyInner](title="End-to-end latency inner"; source="messageAge(innerMessage(rxPk))"; unit=s; record=stats,histogram?,vector; interpolationmode=none);
        
        // Start time
        double startTime @unit(s) = default(0s);  
        
        //Subscriber name
        string subscriberName = default("SubscriberService");
        
        //Service ID
    	int publisherServiceId = default(0);
        
        //The QoS group which is requestes/subscribed
        string qosGroup = default("STD_TCP");
}
