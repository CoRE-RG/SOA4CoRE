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

#include "soa4core/connector/subscriber/SubscriberConnector.h"
#include "SubscriberEndpointBase.h"

namespace SOA4CoRE {

void SubscriberEndpointBase::initialize(){
    this->_msgRecv = registerSignal("msgRecv");
    EndpointBase::initialize();
}

void SubscriberEndpointBase::handleTransportIn(cMessage* msg) {
    sendDirect(msg, _subscriberConnector->gate(CONNECTOR_OUT_GATE_NAME));
    emit(this->_msgRecv, msg);
}

void SubscriberEndpointBase::handleConnectorIn(cMessage* msg) {
    //ignore messages from connector ... shouldnt actually be called..
    throw cRuntimeError("Received a message from connector, this should not happen...");
}

const SubscriberConnector* SubscriberEndpointBase::getSubscriberConnector() const {
    return _subscriberConnector;
}

void SubscriberEndpointBase::setSubscriberConnector(SubscriberConnector* subscriberConnector) {
    _subscriberConnector = subscriberConnector;
}


} /*end namespace SOA4CoRE*/
