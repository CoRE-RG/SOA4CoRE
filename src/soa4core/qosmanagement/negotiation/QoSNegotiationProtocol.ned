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

package soa4core.qosmanagement.negotiation;

import soa4core.utility.processing.ProcessingTimeSimulation;

import inet.applications.contract.IUDPApp;

//
// Manages the Quality-of-Service (QoS) Negotiation for a middleware host and several services.
//
// @author Timo Haeckel and Mehmet Mueller for HAW Hamburg
//
simple QoSNegotiationProtocol extends ProcessingTimeSimulation like IUDPApp
{
    parameters:
        @class(SOA4CoRE::QoSNegotiationProtocol);

        @signal[qosNt](type=simtime_t);
        @statistic[NegotiationTime](title="QosNegotiation Time"; source="qosNt"; record=vector; interpolationmode=none);

        //Signal emitted when a frame is received, contains received QoSNP-Message
        @signal[rxPk](type=SOA4CoRE::Envelope);
        //Statistic of the amount of packets that were received by the application
        @statistic[rxPk](title="Received Packets"; source=rxPk; record=count; unit=packets);
        //Statistic of the amount of bytes that were received by the application
        @statistic[rxBytes](title="Received Bytes"; source=rxPk; unit=B; record="sum(packetBytes)","vector(packetBytes)","histogram(packetBytes)?","floatingIntervalSumVector(packetBytes)?"; interpolationmode=none); //possible parameter e.g.: **.floatingintervalvectorrecorder-measure-interval = 20ms for floatingIntervalRecorders
        //End-to-end latency
        @statistic[rxLatency](title="End-to-end latency"; source="messageAge(rxPk)"; unit=s; record=stats,histogram?,vector; interpolationmode=none);

        //UDP common parameters
        string interfaceTableModule;   // The path to the InterfaceTable module
        //local specifics
        int protocolPort = default(5005);  // local port (-1: use ephemeral port)
        string localAddress = default("");
        string lsmmoduleName = default("lsm");

        processingTime = default(0s);
        allowParallelProcessing = default(true);

        @display("i=block/classifier");
    gates:
        //UDP Transport Protocol gates
        input udpIn @labels(UDPControlInfo/up);
        output udpOut @labels(UDPControlInfo/down);
}
