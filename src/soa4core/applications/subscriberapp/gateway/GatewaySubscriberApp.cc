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

#include "soa4core/applications/subscriberapp/gateway/GatewaySubscriberApp.h"
//CoR4INET
#include "core4inet/utilities/ConfigFunctions.h"
//AUTO-GENERATED MESSAGES
#include "core4inet/linklayer/ethernet/avb/AVBFrame_m.h"
//SignalsAndGateways
#include "signalsandgateways/applications/ethernet/EthernetGatewayApplication.h"
#include "signalsandgateways/gateway/messages/GatewayAggregationMessage.h"
//INET
//AUTO-GENERATED MESSAGES
#include "inet/linklayer/ethernet/EtherFrame_m.h"


namespace SOA4CoRE {
using namespace std;
using namespace SignalsAndGateways;
using namespace CoRE4INET;

Define_Module(GatewaySubscriberApp);

GatewaySubscriberApp::GatewaySubscriberApp() {
}

GatewaySubscriberApp::~GatewaySubscriberApp() {
}

void GatewaySubscriberApp::initialize()
{
    SubscriberAppBase::initialize();
    handleParameterChange(nullptr);

    //find the gateway to deliver messages
    if(EthernetGatewayApplication* gwApp = dynamic_cast<EthernetGatewayApplication*>(this->getParentModule()->getSubmodule("gatewayApp"))) {
        _toGateway = gwApp->gate("in");
    } else {
        throw cRuntimeError("Service GWSinkAppBase can not find EthernetGatewayApplication under the name \"gatewayApp\"");
    }
}

void GatewaySubscriberApp::handleMessage(cMessage* msg) {

    if(msg->arrivedOn("connectorIn")){

        if(GatewayAggregationMessage* gwam = dynamic_cast<GatewayAggregationMessage*>(msg)){
            //todo emit packet that was received

            //unpack can frames and forward
            sendDirect(gwam->dup(), _toGateway);
        } else {
            throw cRuntimeError("Gateway subscriber received message that is not of type GatewayAggregationMessage");
        }
    }

    SubscriberAppBase::handleMessage(msg);
}

}/* end namespace SOA4CoRE */
