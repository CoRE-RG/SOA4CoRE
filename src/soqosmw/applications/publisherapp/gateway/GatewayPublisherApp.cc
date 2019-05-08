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

#include <applications/publisherapp/gateway/GatewayPublisherApp.h>
#include <connector/pubsub/writer/PublisherWriter.h>
#include <qospolicy/base/qospolicy.h>

#include "signalsandgateways/gateway/messages/GatewayAggregationMessage.h"
#include "signalsandgateways/applications/ethernet/EthernetGatewayApplication.h"

namespace soqosmw {
using namespace inet;
using namespace CoRE4INET;
using namespace SignalsAndGateways;
using namespace std;


#define MY_INIT_STAGE_FIRST 1
#define MY_INIT_STAGE_FINAL 14

Define_Module(GatewayPublisherApp);

GatewayPublisherApp::GatewayPublisherApp() {
}

GatewayPublisherApp::~GatewayPublisherApp() {
}

void GatewayPublisherApp::initialize(int stage) {
    switch(stage){
    case MY_INIT_STAGE_FIRST:
        PublisherAppBase::initialize();
        handleParameterChange(nullptr);
        break;

    case MY_INIT_STAGE_FINAL:
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

void GatewayPublisherApp::handleParameterChange(const char* parname) {
    PublisherAppBase::handleParameterChange(parname);

    if(!parname || !strcmp(parname, "canIds")){
        if (par("canIds").stdstringValue() != "") {
            //parse and add to cache
            cStringTokenizer dataFrameIDsTokenizer(par("canIds"), ",");
            _canIds = dataFrameIDsTokenizer.asIntVector();
        }
    }
}

void GatewayPublisherApp::handleMessage(cMessage *msg) {

    if(msg->arrivedOn("gwIn")){
        //msg is from gateway
        if (_writer) {
            SignalsAndGateways::GatewayAggregationMessage* gwam = dynamic_cast<SignalsAndGateways::GatewayAggregationMessage*>(msg);
            if(gwam){
                _writer->write(gwam);
                EV_DEBUG << _serviceName << ": Message Published." << endl;
            } else {
                delete msg;
            }

        } else {
            throw cRuntimeError("No Publisher Registered for this app.");
            delete msg;
        }
    } else {
        PublisherAppBase::handleMessage(msg);
    }

}

void GatewayPublisherApp::scheduleNextMessage() {
    //we do not send messages ourself!
}

} /* end namespace soqosmw */
