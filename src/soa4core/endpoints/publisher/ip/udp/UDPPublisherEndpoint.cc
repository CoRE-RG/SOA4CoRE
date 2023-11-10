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

#include <soa4core/endpoints/publisher/ip/udp/UDPPublisherEndpoint.h>
#include "soa4core/applications/base/ServiceBase.h"
#include <inet/networklayer/common/L3AddressResolver.h>

using namespace inet;
using namespace std;

namespace SOA4CoRE {

Define_Module(UDPPublisherEndpoint);

ConnectionSpecificInformation* UDPPublisherEndpoint::getConnectionSpecificInformation() {
    CSI_UDP* connection = new CSI_UDP();
    connection->setAddress(_localAddress.c_str());
    connection->setPort(_localPort);
    return connection;
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
    // find UDP module and add another gate.
    cModule* udp = getParentModule()->getParentModule()->getSubmodule("udp");
    if(!udp){
        throw cRuntimeError("udp module required for udp publisher but not found");
    }
    //connect to transport via middleware
    connectToTransportGate(udp, "appIn", "appOut");

    // update server socket and listen
    _serverSocket.setOutputGate(gate(TRANSPORT_OUT_GATE_NAME));
    _serverSocket.setReuseAddress(true);

    _serverSocket.bind(_localAddress.c_str() ? L3AddressResolver().resolve(_localAddress.c_str()) : L3Address(), _localPort);
    _isConnected = true;
}

void UDPPublisherEndpoint::addRemote(ConnectionSpecificInformation* csi) {
    if(CSI_UDP* csiUdp = dynamic_cast<CSI_UDP*>(csi)){
        for(auto pair = _remotes.begin(); pair != _remotes.end(); pair++){
            if(pair->first == L3AddressResolver().resolve(csiUdp->getAddress())
                    && pair->second == csiUdp->getPort()) {
                return;
            }
        }
        emit(_remotesSignal,1);
        // create new processor and add to list
        _remotes.push_back(pair<L3Address,int>(L3AddressResolver().resolve(csiUdp->getAddress()), csiUdp->getPort()));
        checkAndCreateFilter(csiUdp);
    }
}

void UDPPublisherEndpoint::checkAndCreateFilter(ConnectionSpecificInformation* csi) {
    if(CSI_UDP* csiUdp = dynamic_cast<CSI_UDP*>(csi)){
        L3Address ipaddress = L3AddressResolver().resolve(csiUdp->getAddress());
        if(has8021QInformation()) {
            // install a traffic filter in the network layer to add the qtag
            auto destAddr = ipaddress.toIPv4();
            createAndInstallFilter(destAddr, _localPort, csiUdp->getPort());
            if(this->_registerStream) {
                registerTalker(destAddr);
            }
        }
    }
}

void UDPPublisherEndpoint::publish(cPacket* msg) {
    if(_isConnected) {
        for(auto pair = _remotes.begin(); pair != _remotes.end(); pair++){
            _serverSocket.sendTo(msg->dup(), pair->first, pair->second);
        }
    }
}


} /*end namespace SOA4CoRE*/
