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

package soa4core.servicemanager.someipservicemanager;

import soa4core.servicemanager.LocalServiceManager;

//
// The SomeIpLocalServiceManager.
//
// @author Mehmet Mueller for HAW Hamburg
//

simple SomeIpLocalServiceManager extends LocalServiceManager
{
    parameters:
        @class(SOA4CoRE::SomeIpLocalServiceManager);
        @display("i=block/join");
        
        // The signal which is emitted when a requested service is found
        @signal[findResultSignal];
        // The signal which is emitted when a service is subscribed
        @signal[subscribeSignal];
        // The signal which is emitted when a subscription has been acknowledged
        @signal[subscribeAckSignal];
}
