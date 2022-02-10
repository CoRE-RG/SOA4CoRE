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

#include "SOMEIPUDPPublisherEndpoint.h"
#include "soa4core/messages/someip/SomeIpHeader_m.h"

namespace SOA4CoRE {

Define_Module(SOMEIPUDPPublisherEndpoint);

ConnectionSpecificInformation* SOMEIPUDPPublisherEndpoint::getConnectionSpecificInformation() {
    CSI_SOMEIP_UDP* connection = new CSI_SOMEIP_UDP();
    connection->setAddress(_localAddress.c_str());
    connection->setPort(_localPort);
    return connection;
}

void SOMEIPUDPPublisherEndpoint::publish(cPacket* msg) {
    if(_isConnected) {
        uint16_t serviceID = atoi(msg->getName());
        cPacket* someipPacket = SOMEIPPublisherEndpointBase::createSOMEIPPacket(serviceID, msg->dup());
        if (SomeIpHeader* someipheader = dynamic_cast<SomeIpHeader*>(someipPacket)){
            for(auto pair = _remotes.begin(); pair != _remotes.end(); pair++){
                _serverSocket.sendTo(someipheader->dup(), pair->first, pair->second);
            }
            delete(someipheader);
        } else {
            throw cRuntimeError("Expected SomeIpHeader");
        }
    }
}

} /* end namespace SOA4CoRE */
