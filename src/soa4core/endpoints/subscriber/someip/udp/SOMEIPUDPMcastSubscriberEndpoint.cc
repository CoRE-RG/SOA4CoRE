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

#include <soa4core/endpoints/subscriber/someip/udp/SOMEIPUDPMcastSubscriberEndpoint.h>

//AUTO-GENERATED MESSAGES
#include "soa4core/messages/someip/SomeIpHeader_m.h"

using namespace inet;

namespace SOA4CoRE {

Define_Module(SOMEIPUDPMcastSubscriberEndpoint);

ConnectionSpecificInformation* SOMEIPUDPMcastSubscriberEndpoint::getConnectionSpecificInformation() {
    CSI_SOMEIP_UDP_MCAST* connection = new CSI_SOMEIP_UDP_MCAST();
    connection->setAddress(_localAddress.c_str());
    connection->setPort(_localPort);
    connection->setDestAddress(_mcastDestAddress.c_str());
    connection->setDestPort(_mcastDestPort);
    return connection;
}

void SOMEIPUDPMcastSubscriberEndpoint::handleTransportIn(cMessage* msg) {
    if (SomeIpHeader* someip = dynamic_cast<SomeIpHeader*>(msg)) {
        //todo check for service ID!
        UDPMcastSubscriberEndpoint::handleTransportIn(someip->decapsulate());
        delete someip;
    }
}


} /*end namespace SOA4CoRE*/
