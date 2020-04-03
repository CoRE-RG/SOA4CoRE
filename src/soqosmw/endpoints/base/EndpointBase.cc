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

#include "EndpointBase.h"

namespace SOQoSMW {

//Gate names
const char EndpointBase::CONNECTOR_OUT_GATE_NAME[] = "endpointIn";
const char EndpointBase::CONNECTOR_IN_GATE_NAME[] = "connectorIn";

const char EndpointBase::TRANSPORT_IN_GATE_NAME[] = "transportIn";
const char EndpointBase::TRANSPORT_OUT_GATE_NAME[] = "transportOut";


void EndpointBase::initialize()
{
    ProcessingTimeSimulation::initialize();
    handleParameterChange(nullptr);
    //nothing to do
    initializeTransportConnection();
    this->_creationTimeSignal = registerSignal("endpointCreationTime");
    this->_creationTime = simTime();
    emit(this->_creationTimeSignal, this->_creationTime);
}

simtime_t EndpointBase::getCreationTime() {
    return this->_creationTime;
}

void EndpointBase::processScheduledMessage(cMessage* msg) {
    if(msg->arrivedOn(CONNECTOR_IN_GATE_NAME)){
        // from connector
        handleConnectorIn(msg);

    }else if(msg->arrivedOn(TRANSPORT_IN_GATE_NAME)){
        // from transport
        handleTransportIn(msg);
    } else {
        delete msg;
    }
}

void EndpointBase::handleParameterChange(const char* parname) {
    ProcessingTimeSimulation::handleParameterChange(parname);

    if (!parname || !strcmp(parname, "qos")) {
        if (!strcmp(par("qos"), "RT")) {
            _qos = QoSGroups::RT;
        } else if (!strcmp(par("qos"), "WEB")) {
            _qos = QoSGroups::WEB;
        } else if (!strcmp(par("qos"), "STD_TCP")) {
            _qos = QoSGroups::STD_TCP;
        } else if (!strcmp(par("qos"), "STD_UDP")) {
            _qos = QoSGroups::STD_UDP;
        } else if (!strcmp(par("qos"), "SOMEIP")) {
            _qos = QoSGroups::SOMEIP;
        } else {
            throw cRuntimeError("Endpoint QoS parameter set to unknown value");
        }
    }
    if (!parname || !strcmp(parname, "endpointPath")) {
        _endpointPath = par("endpointPath").stdstringValue();
    }
}


} /*end namespace SOQoSMW*/
