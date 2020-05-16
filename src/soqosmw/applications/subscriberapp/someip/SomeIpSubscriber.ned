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

package soqosmw.applications.subscriberapp.someip;

import soqosmw.applications.someipapp.base.SomeIpAppBase;

//
// The SomeIpSubscriber provides a subscriber implementation of the SomeIpAppBase.
// These module realizes a SOME/IP subscriber service.
//
// @author Mehmet Cakir
//
simple SomeIpSubscriber extends SomeIpAppBase
{
    parameters:
        @class(SOQoSMW::SomeIpSubscriber);
        
        // The service id of the service that will be subscribed
        int subscribeServiceID;
        // The instance id of the service that will be subscribed
        int instanceID;
        // The destination port
        destPort = default(-1);
        // The message length
        messageLength = default(16B);
        // The send interval
        sendInterval = default(0.001s);
        // The instance id of service that will be subscribed
        instanceID = 0xFFFF;
        
}
