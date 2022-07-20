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

#include "UDPMcastSubscriberEndpoint.h"
//INET
#include <inet/networklayer/common/L3AddressResolver.h>
//AUTO-GENERATED MESSAGES
#include "soa4core/messages/qosnegotiation/ConnectionSpecificInformation_m.h"

using namespace inet;

namespace SOA4CoRE {

Define_Module(UDPMcastSubscriberEndpoint);

ConnectionSpecificInformation* UDPMcastSubscriberEndpoint::getConnectionSpecificInformation() {
    CSI_UDP_MCAST* connection = new CSI_UDP_MCAST();
    connection->setAddress(_localAddress.c_str());
    connection->setPort(_localPort);
    connection->setDestAddress(_mcastDestAddress.c_str());
    connection->setDestPort(_mcastDestPort);
    return connection;
}

void UDPMcastSubscriberEndpoint::initializeTransportConnection() {
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
//
//    const char *multicastInterface = par("multicastInterface");
//    if (multicastInterface[0]) {
//        IInterfaceTable *ift = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this);
//        InterfaceEntry *ie = ift->getInterfaceByName(multicastInterface);
//        if (!ie)
//            throw cRuntimeError("Wrong multicastInterface setting: no interface named \"%s\"", multicastInterface);
//        socket.setMulticastOutputInterface(ie->getInterfaceId());
//    }
//
//    bool receiveBroadcast = par("receiveBroadcast");
//    if (receiveBroadcast)
//        socket.setBroadcast(true);
//
//    bool joinLocalMulticastGroups = par("joinLocalMulticastGroups");
//    if (joinLocalMulticastGroups) {
//        MulticastGroupList mgl = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this)->collectMulticastGroups();
//        socket.joinLocalMulticastGroups(mgl);
//    }
}

void UDPMcastSubscriberEndpoint::handleParameterChange(const char* parname) {
    UDPSubscriberEndpoint::handleParameterChange(parname);

    if (!parname || !strcmp(parname, "mcastDestAddress"))
    {
        _mcastDestAddress = par("mcastDestAddress").stdstringValue();
    }
    if (!parname || !strcmp(parname, "mcastDestPort"))
    {
        _mcastDestPort = par("mcastDestPort");
    }
}


} /*end namespace SOA4CoRE*/
