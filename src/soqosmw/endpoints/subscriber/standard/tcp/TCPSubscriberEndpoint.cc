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

#include "TCPSubscriberEndpoint.h"

#include "soqosmw/applications/base/SOQoSMWApplicationBase.h"

//INET
#include <inet/networklayer/common/L3AddressResolver.h>
#include <inet/transportlayer/contract/tcp/TCPCommand_m.h>

using namespace inet;

namespace SOQoSMW {

Define_Module(TCPSubscriberEndpoint);

ConnectionSpecificInformation* TCPSubscriberEndpoint::getConnectionSpecificInformation() {
    CSI_TCP* connection = new CSI_TCP();
    connection->setAddress(_localAddress.c_str());
    connection->setPort(_localPort);
    return connection;
}

void TCPSubscriberEndpoint::handleTransportIn(cMessage* msg) {

    if(msg->getKind() == TCP_I_ESTABLISHED){
        _socket.processMessage(msg);
    } else {
        STDSubscriberEndpointBase::handleTransportIn(msg);
    }
}

void TCPSubscriberEndpoint::initializeTransportConnection() {
    // find TCP module and add another gate.
    cModule* tcp = getParentModule()->getParentModule()->getSubmodule("tcp");
    if(!tcp){
        throw cRuntimeError("tcp module required for tcp subscriber but not found");
    }
    //connect to transport via middleware
    connectToTransportGate(tcp, "appIn", "appOut");

    // update server socket and connect
    _socket.renewSocket();
    _socket.setDataTransferMode(TCPDataTransferMode::TCP_TRANSFER_OBJECT);
    _socket.bind(_localAddress.c_str() ? L3AddressResolver().resolve(_localAddress.c_str()) : L3Address(), _localPort);

    _socket.setOutputGate(gate(TRANSPORT_OUT_GATE_NAME));

    _socket.connect(L3AddressResolver().resolve(_remoteAddress.c_str()), _remotePort);
}

void TCPSubscriberEndpoint::handleParameterChange(const char* parname) {
    STDSubscriberEndpointBase::handleParameterChange(parname);

    if (!parname || !strcmp(parname, "localAddress"))
    {
        _localAddress = par("localAddress").stdstringValue();
    }
    if (!parname || !strcmp(parname, "localPort"))
    {
        _localPort = par("localPort");
    }
    if (!parname || !strcmp(parname, "remoteAddress"))
    {
        _remoteAddress = par("remoteAddress").stdstringValue();
    }
    if (!parname || !strcmp(parname, "remotePort"))
    {
        _remotePort = par("remotePort");
    }
}


} /*end namespace SOQoSMW*/
