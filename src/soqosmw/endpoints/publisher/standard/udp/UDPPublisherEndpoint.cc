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

#include "UDPPublisherEndpoint.h"

#include "soqosmw/applications/base/SOQoSMWApplicationBase.h"

//INET
#include <inet/networklayer/common/L3AddressResolver.h>

using namespace inet;
using namespace std;

namespace SOQoSMW {

Define_Module(UDPPublisherEndpoint);

ConnectionSpecificInformation* UDPPublisherEndpoint::getConnectionSpecificInformation() {
    CSI_UDP* connection = new CSI_UDP();
    connection->setAddress(_localAddress.c_str());
    connection->setPort(_localPort);
    return connection;
}

void UDPPublisherEndpoint::handleParameterChange(const char* parname) {
    STDPublisherEndpointBase::handleParameterChange(parname);

    if (!parname || !strcmp(parname, "localAddress"))
    {
        _localAddress = par("localAddress").stdstringValue();
    }
    if (!parname || !strcmp(parname, "localPort"))
    {
        _localPort = par("localPort");
    }
}

void UDPPublisherEndpoint::handleTransportIn(cMessage* msg) {
    UDPControlInfo *ind = dynamic_cast<UDPControlInfo *>(msg->getControlInfo());
    if (!ind) {
        delete msg;
        return;
    }
    //check if for server
    if (_serverSocket.belongsToSocket(msg)) {// match message and socket
        EV << "Notification: " << msg;
    } else {
        EV << "Notification isn't intended for this socket.";
    }
    delete msg;
}

void UDPPublisherEndpoint::initializeTransportConnection() {
    // get owning app
    SOQoSMWApplicationBase* app = _connector->getApplications()[0];
    if(!app){
        throw cRuntimeError("Owning application not found in init of publisher.");
    }

    _isConnected = false;

    // find UDP module and add another gate.
    cModule* udp = app->getParentModule()->getSubmodule("udp");
    if(!udp){
        throw cRuntimeError("udp module required for udp publisher but not found");
    }
    cGate* udpInGate = udp->getOrCreateFirstUnconnectedGate("appIn", 0, false, true);
    cGate* udpOutGate = udp->getOrCreateFirstUnconnectedGate("appOut", 0, false, true);

    // check and create new gates in middleware compound module
    cModule* middleware = this->getParentModule();
    if(!middleware->hasGate("udpEndpointsIn") && !middleware->hasGate("udpEndpointsOut")){
        middleware->addGate("udpEndpointsIn", cGate::INPUT, true);
        middleware->addGate("udpEndpointsOut", cGate::OUTPUT, true);
    }
    cGate* middlewareInGate = middleware->getOrCreateFirstUnconnectedGate("udpEndpointsIn", 0, true, true);
    cGate* middlewareOutGate = middleware->getOrCreateFirstUnconnectedGate("udpEndpointsOut", 0, true, true);

    // connect gates
    this->gate(TRANSPORT_OUT_GATE_NAME)->connectTo(middlewareOutGate);
    middlewareOutGate->connectTo(udpInGate);
    udpOutGate->connectTo(middlewareInGate);
    middlewareInGate->connectTo(this->gate(TRANSPORT_IN_GATE_NAME));

    // update server socket and listen
    _serverSocket.setOutputGate(gate(TRANSPORT_OUT_GATE_NAME));
    _serverSocket.setReuseAddress(true);

    _serverSocket.bind(_localAddress.c_str() ? L3AddressResolver().resolve(_localAddress.c_str()) : L3Address(), _localPort);
    _isConnected = true;
}

void UDPPublisherEndpoint::addRemote(ConnectionSpecificInformation* csi) {
    if(CSI_UDP* csiUdp = dynamic_cast<CSI_UDP*>(csi)){
        emit(_remotesSignal,1);
        // create new processor and add to list
        _remotes.push_back(pair<L3Address,int>(L3AddressResolver().resolve(csiUdp->getAddress()), csiUdp->getPort()));
    }
}

void UDPPublisherEndpoint::publish(cPacket* msg) {
    if(_isConnected) {
        for(auto pair = _remotes.begin(); pair != _remotes.end(); pair++){
            _serverSocket.sendTo(msg->dup(), pair->first, pair->second);
        }
    }
}


} /*end namespace SOQoSMW*/
