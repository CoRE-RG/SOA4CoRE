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

#include "TCPPublisherEndpoint.h"

#include "soqosmw/applications/base/SOQoSMWApplicationBase.h"

//INET
#include <inet/networklayer/common/L3AddressResolver.h>

using namespace inet;

namespace SOQoSMW {

Define_Module(TCPPublisherEndpoint);

TCPPublisherEndpoint::~TCPPublisherEndpoint() {
    this->deleteSockets();
}

void TCPPublisherEndpoint::handleParameterChange(const char* parname) {
    STDPublisherEndpointBase::handleParameterChange(parname);

    if (!parname || !strcmp(parname, "localAddress")) {
        _localAddress = par("localAddress").stdstringValue();
    }
    if (!parname || !strcmp(parname, "localPort")) {
        _localPort = par("localPort");
    }
}

ConnectionSpecificInformation* TCPPublisherEndpoint::getConnectionSpecificInformation() {
    CSI_TCP* connection = new CSI_TCP();
    connection->setAddress(_localAddress.c_str());
    connection->setPort(_localPort);
    return connection;
}

void TCPPublisherEndpoint::initializeTransportConnection() {
    // find TCP module and add another gate.
    cModule* tcp = getParentModule()->getParentModule()->getSubmodule("tcp");
    if (!tcp) {
        throw cRuntimeError(
                "tcp module required for tcp publisher but not found");
    }
    //connect to transport via middleware
    connectToTransportGate(tcp, "appIn", "appOut");

    // update server socket and listen
    _serverSocket.renewSocket();
    _serverSocket.setDataTransferMode(TCPDataTransferMode::TCP_TRANSFER_OBJECT);
    _serverSocket.bind(
            _localAddress.c_str() ?
                    L3AddressResolver().resolve(_localAddress.c_str()) :
                    L3Address(), _localPort);

    _serverSocket.setOutputGate(gate(TRANSPORT_OUT_GATE_NAME));

    _serverSocket.listen();
}

void TCPPublisherEndpoint::handleTransportIn(cMessage* msg) {
    TCPCommand *ind = dynamic_cast<TCPCommand *>(msg->getControlInfo());
    if (!ind) {
        delete msg;
        return;
    }

    //check if for server
    if (_serverSocket.belongsToSocket(msg)) { // match message and socket
        _serverSocket.processMessage(msg);
    } else {
        //check if it belongs to a socket in the map
        TCPSocket *socket = this->findSocketFor(msg);

        //if not open a new one!
        if (!socket) {
            emit(_remotesSignal, 1);
            _isConnected = true;
            // new connection -- create new socket object and server process
            socket = new TCPSocket(msg);
            socket->setOutputGate(gate(TRANSPORT_OUT_GATE_NAME));
            this->addSocket(socket);
        }
        socket->processMessage(msg); // invoke callback interface
    }
}

void TCPPublisherEndpoint::publish(cPacket* msg) {
    if (_isConnected) {
        for (auto iter = socketMap.begin(); iter != socketMap.end(); iter++) {
            iter->second->send(msg->dup());
        }
    }
}

} /*end namespace SOQoSMW*/
