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

package soa4core.manager.qos;

import soa4core.manager.base.Manager;
//
// The QoSManager module creates, finds and removes local QoS services.
//
// @author Mehmet Mueller and Timo Haeckel for HAW Hamburg
//
simple QoSManager extends Manager
{
        parameters:
        @class(SOA4CoRE::QoSManager);
        @display("i=block/join");
        string qosnpmoduleName = default("qosnp");

        // ########## parameters for service creation AVB ########## :
        //Interval in which a subscription is updated
        double updateInterval @unit("s") = default(6s);
        //Interval in which a subscription is reregistered after a registration failed
        double retryInterval @unit("s") = default(1s);
        
        // The signal which is emitted when a requested service is found
        @signal[findResultSignal];
}
