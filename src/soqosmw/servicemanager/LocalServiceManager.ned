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

package soqosmw.servicemanager;

import soqosmw.servicemanager.base.ILocalServiceManager;

//
// The LocalServiceManager.
//
// @author Timo Haeckel for HAW Hamburg
//
simple LocalServiceManager like ILocalServiceManager
{
    parameters:
        @class(SOQoSMW::LocalServiceManager);
        @display("i=block/cogwheel");
        
        // paths to used modules.
        string sdmoduleName = default("sd");
        string qosnpmoduleName = default("qosnp");

        string localAddress = default("");

        //parameters for service creation:
        	//AVB:
        	//Interval in which a subscription is updated
        double updateInterval @unit("s") = default(6s);
        //Interval in which a subscription is reregistered after a registration failed
        double retryInterval @unit("s") = default(1s);
}
