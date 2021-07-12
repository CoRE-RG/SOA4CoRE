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

package soqosmw.applications.publisherapp.base;

import soqosmw.applications.base.SOQoSMWApplicationBase;

//
// The PublisherAppBase provides a publisher implementation of the soqosmw application base.
// These modules realize a data publisher service.
//
// @author Timo Haeckel for HAW Hamburg
//
simple PublisherAppBase extends SOQoSMWApplicationBase
{
    parameters:
        @class(SOQoSMW::PublisherAppBase);
        @display("i=block/source");
        
        // PublisherApp enabled or disabled
        bool enabled = default(true);
        // Start time
        double startTime @unit(s) = default(0s);
        // Interval of new messages
        double interval @unit(s) = default(125us);
        // Number of frames per interval
        int intervalFrames = default(1);     
        //Size of the payload of the message (size of encapsulated cPacket) that is being generated
        volatile int payload @unit(Byte) = default(46Byte);
        // ID of the AVB Stream
        int streamID = default(0);     
        // Stream Reservation Class of the AVB Stream
        string srClass = default("A");
        
        @signal[payloadSignal];
        
        //Signal emitted when a message is published
        @signal[msgSent](type=omnetpp::cMessage);
        //Statistic of the amount of packets that were sent by the publisher
        @statistic[msgCount](title="Sent Packets"; source=msgSent; record=count; unit=packets);
        //Statistic of the amount of bytes that were sent by the publisher
        @statistic[msgBytes](title="Sent Bytes"; source=msgSent; unit=B; record="sum(packetBytes)","vector(packetBytes)", "histogram(packetBytes)?","floatingIntervalSumVector(packetBytes)?"; interpolationmode=none); //possible parameter e.g.: **.floatingintervalvectorrecorder-measure-interval = 20ms for floatingIntervalRecorders
        //End-to-end latency
        @statistic[msgLatency](title="End-to-end latency"; source="messageAge(msgSent)"; unit=s; record=stats,histogram?,vector; interpolationmode=none);
    
    	//Service Description
    	string serviceName = default("PublisherService");
    	
    	//Service ID
    	int serviceId = default(0);
    	
    	//Connection type {connectionbased, connectionless}
    	string qosGroup = default("");
    
}
