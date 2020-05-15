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

package soqosmw.discovery.someipservicediscovery;
import soqosmw.applications.someipapp.base.SomeIpAppBase;

//
// The SomeipSD provides a SOME/IP service discovery implementation of the SomeIpAppBase.
// This module realizes the SOME/IP service discovery.
//
// @author Mehmet Cakir
//
simple SomeIpSD extends SomeIpAppBase
{
    parameters:
        @class(SOQoSMW::SomeIpSD);
        
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
}
