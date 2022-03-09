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

#include "soa4core/applications/base/ServiceBase.h"
#include "UDPSubscriberEndpoint.h"
//INET
#include <inet/networklayer/common/L3AddressResolver.h>

using namespace inet;

namespace SOA4CoRE {

Define_Module(UDPSubscriberEndpoint);

ConnectionSpecificInformation* UDPSubscriberEndpoint::getConnectionSpecificInformation() {
    CSI_UDP* connection = new CSI_UDP();
    connection->setAddress(_localAddress.c_str());
    connection->setPort(_localPort);
    return connection;
}

void UDPSubscriberEndpoint::initializeTransportConnection() {
    // find UDP module and add another gate.
    cModule* udp = getParentModule()->getParentModule()->getSubmodule("udp");
    if(!udp){
        throw cRuntimeError("udp module required for udp subscriber but not found");
    }
    //connect to transport via middleware
    connectToTransportGate(udp, "appIn", "appOut");

    // update server socket and connect
    _socket.setOutputGate(gate(TRANSPORT_OUT_GATE_NAME));
    _socket.setReuseAddress(true);
    _socket.bind(_localAddress.c_str() ? L3AddressResolver().resolve(_localAddress.c_str()) : L3Address(), _localPort);
    _isConnected = true;
}

void UDPSubscriberEndpoint::handleParameterChange(const char* parname) {
    STDSubscriberEndpointBase::handleParameterChange(parname);

    if (!parname || !strcmp(parname, "localAddress"))
    {
        _localAddress = par("localAddress").stdstringValue();
    }
    if (!parname || !strcmp(parname, "localPort"))
    {
        _localPort = par("localPort");
    }
}

} /*end namespace SOA4CoRE*/
