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

#include "SOMEIPTCPPublisherEndpoint.h"
//AUTO-GENERATED MESSAGES
#include "soa4core/messages/someip/SomeIpHeader_m.h"
#include "soa4core/applications/publisher/base/Publisher.h"
#include "soa4core/applications/base/ServiceBase.h"
#include "soa4core/connector/publisher/PublisherConnector.h"
#include "soa4core/utility/comfortFunctions.h"

namespace SOA4CoRE {

Define_Module(SOMEIPTCPPublisherEndpoint);

ConnectionSpecificInformation* SOMEIPTCPPublisherEndpoint::getConnectionSpecificInformation() {
    CSI_SOMEIP_TCP* connection = new CSI_SOMEIP_TCP();
    connection->setAddress(_localAddress.c_str());
    connection->setPort(_localPort);
    return connection;
}

void SOMEIPTCPPublisherEndpoint::publish(cPacket* msg) {
    if(_isConnected) {
        uint16_t serviceID = getServiceId();
        cPacket* someipPacket = SOMEIPPublisherEndpointBase::createSOMEIPPacket(serviceID, msg->dup());
        if (*(msg->getName())) // check if empty
        {
            someipPacket->setName(msg->getName());
        }
        if (SomeIpHeader* someipheader = dynamic_cast<SomeIpHeader*>(someipPacket)){
            for (auto iter = socketMap.begin(); iter != socketMap.end(); iter++) {
                iter->second->send(someipheader->dup());
            }
            delete(someipheader);
        } else {
            throw cRuntimeError("Expected SomeIpHeader");
        }
    }
}

uint16_t SOMEIPTCPPublisherEndpoint::calculateL2Framesize(uint16_t payload) {
    return TCPPublisherEndpoint::calculateL2Framesize(payload) + SOMEIP_HEADER_BYTES;
}

uint64_t SOMEIPTCPPublisherEndpoint::createStreamId(
        inet::IPv4Address destAddress) {
    Publisher* app = dynamic_cast<Publisher*>(_publisherConnector->getApplication());
    if(!app) {
        throw cRuntimeError("Publisher could not be resolved.");
    }
    return buildStreamIDForService(app->getServiceId(), app->getInstanceId(), destAddress);
}

uint16_t SOMEIPTCPPublisherEndpoint::getServiceId() {
    return this->getPublisherConnector()->getApplication()->getServiceId();
}

} /*end namespace SOA4CoRE*/
