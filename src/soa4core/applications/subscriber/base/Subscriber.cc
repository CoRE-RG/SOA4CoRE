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

#include "soa4core/applications/subscriber/base/Subscriber.h"
#include "soa4core/connector/subscriber/SubscriberConnector.h"
#include "soa4core/serviceidentifier/ServiceIdentifier.h"
#include "soa4core/messages/qosnegotiation/QoSNegotiationProtocol_m.h"
//CORE4INET
#include <core4inet/utilities/ConfigFunctions.h>
#include <core4inet/base/avb/AVBDefs.h>
//INET
#include <inet/linklayer/ethernet/EtherFrame_m.h>
#include <inet/networklayer/common/L3AddressResolver.h>
//STD
#include <cstring>
#include <iostream>

namespace SOA4CoRE {
using namespace std;
using namespace CoRE4INET;

Define_Module(Subscriber);

Subscriber::Subscriber() {
}

Subscriber::~Subscriber() {
}

void Subscriber::initialize()
{
    ServiceBase::initialize();
    this->_rxPkSignal = registerSignal("rxPk");
}

void Subscriber::handleMessage(cMessage *msg)
{
    if(msg->isSelfMessage()) {
        ServiceBase::handleMessage(msg);
    } else {
        EV_DEBUG << "Subscriber " << _serviceName << " received a message."  << endl;
        //this is a subscription message so handle it.
        if (omnetpp::cPacket *frame = dynamic_cast<omnetpp::cPacket*>(msg))
        {
            emit(_rxPkSignal, frame);
        }
        delete msg;
    }
}

void Subscriber::handleParameterChange(const char* parname)
{
    ServiceBase::handleParameterChange(parname);

    if (!parname || !strcmp(parname, "qosGroup")) {
        string group = par("qosGroup").stdstringValue();
        if(group == "WEB"){
            //_qosGroup = QoSGroups::WEB;
            throw cRuntimeError("WEB QoS is not implemented yet.");
        } else if(group == "IP_TCP" || group == "STD_TCP"){
            _qosGroup = QoSGroup::IP_TCP;
        } else if(group == "IP_UDP" || group == "STD_UDP"){
            _qosGroup = QoSGroup::IP_UDP;
        } else if(group == "IP_UDP_MCAST" || group == "STD_UDP_MCAST"){
            _qosGroup = QoSGroup::IP_UDP_MCAST;
        } else if(group == "RT"){
            _qosGroup = QoSGroup::RT;
        } else if(group == "SOMEIP_TCP"){
            _qosGroup = QoSGroup::SOMEIP_TCP;
        } else if(group == "SOMEIP_UDP") {
            _qosGroup = QoSGroup::SOMEIP_UDP;
        } else if(group == "SOMEIP_UDP_MCAST") {
            _qosGroup = QoSGroup::SOMEIP_UDP_MCAST;
        } else {
            throw cRuntimeError("Unknown QoS");
        }
    }
    if (!parname || !strcmp(parname, "mcastDestAddress")) {
        _mcastDestAddr = par("mcastDestAddress").stringValue();
    }
    if (!parname || !strcmp(parname, "mcastDestPort")) {
        _mcastDestPort = par("mcastDestPort");
    }
}


void Subscriber::handleStart() {
    //create a subscriber
    ServiceIdentifier publisherServiceIdentifier = ServiceIdentifier(this->_serviceId,this->_instanceId);
    _connector = dynamic_cast<ConnectorBase*>(_localServiceManager->registerSubscriberServiceAndSubscribeService(publisherServiceIdentifier, this));
    if (!_connector) {
        throw cRuntimeError("No subscriber connector created.");
    }
}

QoSGroup Subscriber::getQoSGroup() {
    return _qosGroup;
}

}/* end namespace SOA4CoRE */
