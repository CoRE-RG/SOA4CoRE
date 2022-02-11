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

package soa4core.connector.subscriber;

import soa4core.connector.base.ConnectorBase;

//
// The SubscriberConnector implements the common interface for all connctor modules @see~ConnectorBase.
// This module is created by the Manager module (@see~Manager)
// during runtime to connect a subscriber application to its protocol specific subscriper endpoints.
//
// @author Timo Haeckel for HAW Hamburg
//
simple SubscriberConnector extends ConnectorBase
{
    parameters:
        @class(SOA4CoRE::SubscriberConnector);
        // maximum number of appilcations allowed to connect to this connector.
        // -1 = infinite
        int maxApplications = default(-1);

        // Signal emitted when a message is forwarded to Applications
        @signal[forwardedToApps](type=omnetpp::cPacket);
        //Statistic of the amount of messages that were forwarded to applications
        @statistic[fwdToApps](title="Forwarded to Apps"; source=forwardedToApps; record=count; unit=packets);
        //Statistic of the amount of bytes that were forwarded to applications
        @statistic[fwdToAppsBytes](title="Forwarded Bytes"; source=forwardedToApps; unit=B; record="sum(packetBytes)","vector(packetBytes)","histogram(packetBytes)?","floatingIntervalSumVector(packetBytes)?"; interpolationmode=none); //possible parameter e.g.: **.floatingintervalvectorrecorder-measure-interval = 20ms for floatingIntervalRecorders
        //End-to-end latency
        @statistic[fwdToAppsLatency](title="End-to-end latency"; source="messageAge(forwardedToApps)"; unit=s; record=stats,histogram?,vector; interpolationmode=none);
        
    gates:
        // input gate for messages from the endpoint
        input endpointIn @directIn;
}
