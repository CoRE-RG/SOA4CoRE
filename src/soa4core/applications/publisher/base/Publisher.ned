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

package soa4core.applications.publisher.base;

import soa4core.applications.base.ServiceBase;

//
// The Publisher provides a publisher implementation of the middleware application base.
// These modules realize a data publisher service.
//
// @author Timo Haeckel for HAW Hamburg
//
simple Publisher extends ServiceBase
{
    parameters:
        @class(SOA4CoRE::Publisher);
        @display("i=block/source");
        
        // Interval of new messages
        double intervalMin @unit(s) = default(125us);// must be set for realtime publishers
        double intervalMax @unit(s) = default(intervalMin);
        volatile double interval @unit(s) = default(uniform(intervalMin, intervalMax)); // setting this directly overrides interval min and max values
        // Number of frames per interval
        int intervalFrames = default(1);     
        //Size of the payload of the message (size of encapsulated cPacket) that is being generated
        int payloadMax @unit(Byte) = default(46Byte);// must be set for realtime publishers
        int payloadMin @unit(Byte) = default(payloadMax);
        volatile int payload @unit(Byte) = default(intWithUnit(uniform(payloadMin, payloadMax)));
        // ID of the AVB Stream
        int streamID = default(serviceId);     
        // Stream Reservation Class of the AVB Stream
        string srClass = default("A");   
        // VLAN identifier of the AVB Stream
        int vlan_id = default(-1);
        // Priority code point of the AVB Stream
        int pcp = default(-1); // -1 = use srClass default priority
        // IPv4 Multicast destination address
        string mcastDestAddress = default(""); // Requires a valid IP Mcast address (prefix 224/4), format "XXX.XXX.XXX.XXX", if empty no mcast is supported by publisher 
        // Multicast destination port for UDP mcast
        int mcastDestPort = default(-1); // Requires a valid UDP port, if -1 no udp mcast is supported by publisher 
        
        @signal[payloadSignal];
        
        //Signal emitted when a message is published
        @signal[msgSent](type=omnetpp::cMessage);
        //Statistic of the amount of packets that were sent by the publisher
        @statistic[msgCount](title="Sent Packets"; source=msgSent; record=count; unit=packets);
        //Statistic of the amount of bytes that were sent by the publisher
        @statistic[msgBytes](title="Sent Bytes"; source=msgSent; unit=B; record="sum(packetBytes)","vector(packetBytes)", "histogram(packetBytes)?","floatingIntervalSumVector(packetBytes)?"; interpolationmode=none); //possible parameter e.g.: **.floatingintervalvectorrecorder-measure-interval = 20ms for floatingIntervalRecorders
    
        //Offered instance id, must not be 0 or 0xFFFF, default is 1
        instanceID = default(0x0001);
        
    	//Service Description
    	serviceName = default("PublisherService");
    	
    	//The QoS types this application can serve (one space separated)
    	string qosGroups = default("");
    
}
