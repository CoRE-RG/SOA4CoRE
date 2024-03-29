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

package soa4core.discovery.someip;

import inet.applications.udpapp.UDPBasicApp;
import soa4core.discovery.base.IServiceDiscovery;

//
// The SomeipSD provides a SOME/IP service discovery implementation of the SomeIpAppBase.
// This module realizes the SOME/IP service discovery.
//
// @author Mehmet Mueller for HAW Hamburg
//
simple SomeIpSD extends UDPBasicApp like IServiceDiscovery 
{
    parameters:
        @class(SOA4CoRE::SomeIpSD);
        // The signal which is emitted when a find is received
        @signal[serviceFindSignal];
        // The signal which is emitted when a offer is received
        @signal[serviceOfferSignal];
        //The signal which is emitted when a subscribeEventGroup is received
        @signal[subscribeEventGroupSignal];
        // The signal which is emitted when a subscribeEventGroupAck is received
        @signal[subscribeEventGroupAckSignal];
        
        // Broadcasts will be received by default
        receiveBroadcast = default(true);
        // The local port of the SOME/IP service discovery module
        localPort = default(30490);
        // The destination port of the remote service discovery module
        destPort = default(30490);
        // The message length 
        messageLength = default(16B);
        // The send interval of a message
        sendInterval = default(0.001s);
        // Multicast interface
      	multicastInterface = default("eth0");
        // Join mcast
        joinLocalMulticastGroups = default(true);
        // SOME/IP multicast address
        string mcastDestAddress = default("224.0.2.254");
      
}
