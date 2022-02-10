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

package soa4core.connector.pubsub.writer;

import soa4core.connector.base.ConnectorBase;

//
// The PublisherConnector implements the common interface for all connctor modules @see~ConnectorBase.
// This module is created by the LocalServiceManager module (@see~LocalServiceManager)
// during runtime to connect a publisher application to its protocol specific publisher endpoints.
//
// @author Timo Haeckel for HAW Hamburg
//
simple PublisherConnector extends ConnectorBase
{
    parameters:
        @class(SOA4CoRE::PublisherConnector);
        // maximum number of endpoints allowed to connect to this connector.
        // -1 = infinite
        int maxEndpoints = default(-1);
        
        // Signal emitted when a message is forwarded to Endpoints
        @signal[forwardedToEPs](type=omnetpp::cPacket);
        //Statistic of the amount of messages that were forwarded to endpoints
        @statistic[fwdToEPs](title="Forwarded to EPs"; source=forwardedToEPs; record=count; unit=packets);
        //Statistic of the amount of bytes that were forwarded to endpoints
        @statistic[fwdToEPsBytes](title="Fowarded Bytes"; source=forwardedToEPs; unit=B; record="sum(packetBytes)","vector(packetBytes)","histogram(packetBytes)?","floatingIntervalSumVector(packetBytes)?"; interpolationmode=none); //possible parameter e.g.: **.floatingintervalvectorrecorder-measure-interval = 20ms for floatingIntervalRecorders
        //End-to-end latency
        @statistic[fwdToEPsLatency](title="End-to-end latency"; source="messageAge(forwardedToEPs)"; unit=s; record=stats,histogram?,vector; interpolationmode=none);
        
    gates:
        // input gate for messages from the application
        input applicationIn @directIn;
}
