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

#include <soa4core/applications/publisher/gateway/GatewayPublisher.h>
#include "soa4core/connector/base/ConnectorBase.h"
//SignalsAndGateways
#include "signalsandgateways/gateway/messages/GatewayAggregationMessage.h"
#include "signalsandgateways/applications/ethernet/EthernetGatewayApplication.h"

namespace SOA4CoRE {
using namespace inet;
using namespace CoRE4INET;
using namespace SignalsAndGateways;
using namespace std;


Define_Module(GatewayPublisher);

GatewayPublisher::GatewayPublisher() {
}

GatewayPublisher::~GatewayPublisher() {
}

void GatewayPublisher::initialize(int stage) {
    switch(stage){
    case INITSTAGE_LOCAL:
        Publisher::initialize();
        handleParameterChange(nullptr);
        break;

    case INITSTAGE_APPLICATION_LAYER:
        //get the EthernetGatewayApp to connect to it
        if(EthernetGatewayApplication* gwApp = dynamic_cast<EthernetGatewayApplication*>(this->getParentModule()->getSubmodule("gatewayApp"))) {
            //register CAN IDs to listen to
            for(vector<int>::iterator id = _canIds.begin(); id != _canIds.end(); id++){
                gwApp->registerForCANID(*id, this->gate("gwIn"));
            }
        } else {
            throw cRuntimeError("Service GWSourceApp can not find EthernetGatewayApplication under the name \"gatewayApp\"");
        }
        break;
    default:
        break;
    }
}

void GatewayPublisher::handleParameterChange(const char* parname) {
    Publisher::handleParameterChange(parname);

    if(!parname || !strcmp(parname, "canIds")){
        if (par("canIds").stdstringValue() != "") {
            //parse and add to cache
            cStringTokenizer dataFrameIDsTokenizer(par("canIds"), ",");
            _canIds = dataFrameIDsTokenizer.asIntVector();
        }
    }
}

void GatewayPublisher::handleMessage(cMessage *msg) {

    if(msg->arrivedOn("gwIn")){
        //msg is from gateway
        if (_connector) {
            SignalsAndGateways::GatewayAggregationMessage* gwam = dynamic_cast<SignalsAndGateways::GatewayAggregationMessage*>(msg);
            if(gwam){
                emit(this->_msgSentSignal, msg);
                sendDirect(gwam, _connector->gate("applicationIn"));
                EV_DEBUG << _publisherName << ": Message Published." << endl;
            } else {
                delete msg;
            }

        } else {
            throw cRuntimeError("No Publisher Registered for this app.");
            delete msg;
        }
    } else {
        Publisher::handleMessage(msg);
    }

}

void GatewayPublisher::scheduleNextMessage() {
    //we do not send messages ourself!
}

} /* end namespace SOA4CoRE */
