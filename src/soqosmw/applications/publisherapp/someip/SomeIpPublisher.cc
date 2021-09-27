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

#include <soqosmw/applications/publisherapp/someip/SomeIpPublisher.h>
#include "soqosmw/messages/someip/SomeIpHeader_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"

namespace SOQoSMW {

Define_Module(SomeIpPublisher);

void SomeIpPublisher::initialize() {
    PublisherAppBase::initialize();
    _instanceID = par("instanceID").intValue();
}

SomeIpPublisher::SomeIpPublisher() : PublisherAppBase(){
}

SomeIpPublisher::~SomeIpPublisher() {

}

/*
void SomeIpPublisher::handleMessageWhenUp(cMessage *msg) {
    if (!_destinations.empty()) {
        std::string headerName = "RESPONSE - " + std::to_string(getServiceID());
        SomeIpHeader* someipHeader = new SomeIpHeader(headerName.c_str());
        someipHeader->setMessageID(0x00000042);
        someipHeader->setLength(someipHeader->getByteLength()-UNCOVEREDBYTESBYLENGTH);
        someipHeader->setRequestID(0x00000042);
        someipHeader->setProtocolVersion(SOQoSMW::ProtocolVersion::PV_1);
        someipHeader->setInterfaceVersion(SOQoSMW::InterfaceVersion::IV_1);
        someipHeader->setMessageType(SOQoSMW::MessageType::RESPONSE);
        someipHeader->setReturnCode(SOQoSMW::ReturnCode::E_OK);
        for (std::pair<inet::L3Address,uint16_t> destination : _destinations) {
            socket.sendTo(someipHeader->dup(), destination.first, destination.second);
        }
        delete someipHeader;
    }
    delete msg;
    SomeIpAppBase::scheduleSelfMsg(_sendInterval);
}
*/

uint16_t SomeIpPublisher::getInstanceId() {
    return _instanceID;
}

} /* end namespace SOQoSMW */
