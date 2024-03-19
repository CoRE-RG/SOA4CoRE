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

package soa4core.applications.publisher.sync;

import soa4core.applications.publisher.base.Publisher;

//
// Synchronous source for a publisher application.
//
// @author Timo Haeckel for HAW Hamburg
//
simple SyncPublisher extends Publisher
{
    parameters:
        @class(SOA4CoRE::SyncPublisher);
        // note: the interval is ignored as it is determined by the period length and the module counter.
		interval = 0s;
        //Time in the period at which the message is being send to the connected buffers
        double actionTime @unit(s) = default(0s);
        //Modulo cycles: 1 means message is being send in each cycle, 2 means in every second cycle,
        //and so on. 0 means never
        int modulo = default(1);
        //Oscillator of Scheduler this app is assigned to
        string oscillator = default("^.scheduler.oscillator");
        //Timer of Scheduler this app is assigned to
        string timer = default("^.scheduler.timer");
        //Period of Scheduler this app is assigned to
        string period = default("^.scheduler.period[0]");
    gates:
        //Input gate for the incoming Scheduler messages
        input schedulerIn @directIn @label(SchedulerMessage); 
}
