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

package soqosmw.servicemanager.qosservicemanager;

import soqosmw.servicemanager.LocalServiceManager;
//
// The QoSLocalServiceManager.
//
// @author Mehmet Cakir
//
simple QoSLocalServiceManager extends LocalServiceManager
{
        parameters:
        @class(SOQoSMW::QoSLocalServiceManager);
        @display("i=block/join");
        
        // The signal which is emitted when a requested service is found
        @signal[findResultSignal];
}
