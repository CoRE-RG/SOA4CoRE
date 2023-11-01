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

#include <soa4core/endpoints/subscriber/ip/udp/UDPMcastSubscriberEndpoint.h>

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
    // switcheroo workaround to bind the socket to mcast port
    int tmp = _localPort;
    _localPort = _mcastDestPort;
    // let the base init the socket first
    UDPSubscriberEndpoint::initializeTransportConnection();
    _localPort = tmp;

    // set input interface on socket
    IInterfaceTable *ift = dynamic_cast<IInterfaceTable*>(this->getParentModule()->getParentModule()->getSubmodule("interfaceTable"));
    if(!ift)
        throw cRuntimeError("Could not locate interface table at relative path from endpoint \"^.^.interfaceTable\"");
    InterfaceEntry *ie = ift->getInterfaceByName("eth0");
    if (!ie) {
        ie = ift->getInterfaceByName("phy0");
    }
    if(!ie)
        throw cRuntimeError("Wrong multicastInterface setting: no interface named \"eth0\" or \"phy0\"");

    _socket.joinMulticastGroup(L3Address(_mcastDestAddress.c_str()), ie->getInterfaceId());
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
