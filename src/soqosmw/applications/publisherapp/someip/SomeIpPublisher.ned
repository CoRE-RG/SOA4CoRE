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

package soqosmw.applications.publisherapp.someip;

import soqosmw.applications.publisherapp.base.PublisherAppBase;

//
// The SomeipPublisher provides a publisher implementation of the SomeIpAppBase.
// These module realizes a SOME/IP publisher service.
//
// @author Mehmet Cakir
//
simple SomeIpPublisher extends PublisherAppBase
{
    parameters:
        @class(SOQoSMW::SomeIpPublisher);
        
        // The instance id of the service that will be subscribed
        int instanceID = default(0xFFFF);
}
