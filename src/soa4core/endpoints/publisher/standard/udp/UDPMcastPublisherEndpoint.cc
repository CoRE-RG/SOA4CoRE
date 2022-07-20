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
#include "UDPMcastPublisherEndpoint.h"
//INET
#include <inet/networklayer/common/L3AddressResolver.h>
//AUTO-GENERATED MESSAGES
#include "soa4core/messages/qosnegotiation/ConnectionSpecificInformation_m.h"

using namespace inet;
using namespace std;

namespace SOA4CoRE {

Define_Module(UDPMcastPublisherEndpoint);

ConnectionSpecificInformation* UDPMcastPublisherEndpoint::getConnectionSpecificInformation() {
    CSI_UDP_MCAST* connection = new CSI_UDP_MCAST();
    connection->setAddress(_localAddress.c_str());
    connection->setPort(_localPort);
    connection->setDestAddress(_mcastDestAddress.c_str());
    connection->setDestPort(_mcastDestPort);
    return connection;
}

void UDPMcastPublisherEndpoint::handleParameterChange(const char* parname) {
    UDPPublisherEndpoint::handleParameterChange(parname);

    if (!parname || !strcmp(parname, "mcastDestAddress"))
    {
        _mcastDestAddress = par("mcastDestAddress").stdstringValue();
    }
    if (!parname || !strcmp(parname, "mcastDestPort"))
    {
        _mcastDestPort = par("mcastDestPort");
    }
}

void UDPMcastPublisherEndpoint::initializeTransportConnection() {
    // find UDP module and add another gate.
    cModule* udp = getParentModule()->getParentModule()->getSubmodule("udp");
    if(!udp){
        throw cRuntimeError("udp module required for udp publisher but not found");
    }
    //connect to transport via middleware
    connectToTransportGate(udp, "appIn", "appOut");

    // update server socket and listen
    _serverSocket.setOutputGate(gate(TRANSPORT_OUT_GATE_NAME));
    _serverSocket.setReuseAddress(true);

    _serverSocket.bind(_localAddress.c_str() ? L3AddressResolver().resolve(_localAddress.c_str()) : L3Address(), _localPort);
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

void UDPMcastPublisherEndpoint::publish(cPacket* msg) {
    if(_isConnected) { // && _remotes.size()>0) { // TODO check if there are still remotes interested.
        _serverSocket.sendTo(msg->dup(), L3Address(_mcastDestAddress.c_str()), _mcastDestPort);
    }
}


} /*end namespace SOA4CoRE*/
