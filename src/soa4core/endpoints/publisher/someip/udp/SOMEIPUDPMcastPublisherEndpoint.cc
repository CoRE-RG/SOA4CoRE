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

#include "SOMEIPUDPMcastPublisherEndpoint.h"

//AUTO-GENERATED MESSAGES
#include "soa4core/messages/someip/SomeIpHeader_m.h"

using namespace inet;
using namespace std;

namespace SOA4CoRE {

Define_Module(SOMEIPUDPMcastPublisherEndpoint);

ConnectionSpecificInformation* SOMEIPUDPMcastPublisherEndpoint::getConnectionSpecificInformation() {
    CSI_SOMEIP_UDP_MCAST* connection = new CSI_SOMEIP_UDP_MCAST();
    connection->setAddress(_localAddress.c_str());
    connection->setPort(_localPort);
    connection->setDestAddress(_mcastDestAddress.c_str());
    connection->setDestPort(_mcastDestPort);
    return connection;
}

void SOMEIPUDPMcastPublisherEndpoint::publish(cPacket* msg) {
    if(_isConnected) { // && _remotes.size()>0) { // TODO check if there are still remotes interested.
        uint16_t serviceID = atoi(msg->getName());
        cPacket* someipPacket = SOMEIPPublisherEndpointBase::createSOMEIPPacket(serviceID, msg->dup());
        if (SomeIpHeader* someipheader = dynamic_cast<SomeIpHeader*>(someipPacket)){
            _serverSocket.sendTo(someipheader, L3Address(_mcastDestAddress.c_str()), _mcastDestPort);
        } else {
            throw cRuntimeError("Expected SomeIpHeader");
        }
    }
}

uint16_t SOMEIPUDPMcastPublisherEndpoint::calculateL1Framesize(uint16_t payload) {
    return UDPPublisherEndpoint::calculateL1Framesize(payload) + SOMEIP_HEADER_BYTES;
}


} /*end namespace SOA4CoRE*/
