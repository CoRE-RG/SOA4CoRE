//
// c Timo Haeckel, for HAW Hamburg
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

#include "PublisherEndpointBase.h"


namespace soqosmw {

void PublisherEndpointBase::handleTransportIn(cMessage* msg) {
    //ignore messages from transport layer... shouldnt actually be called..
    throw cRuntimeError("Received a message from transport, this should not happen...");
}

void PublisherEndpointBase::handleConnectorIn(cMessage* msg) {
    if(cPacket* packet = dynamic_cast<cPacket*>(msg)){
        if(_isConnected){
            publish(packet);
        }
    }
    delete msg;
}



} /*end namespace soqosmw*/
