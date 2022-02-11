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

#include <soa4core/applications/subscriber/base/Subscriber.h>
#include <soa4core/manager/Manager.h>
#include <soa4core/manager/Manager.h>
#include "soa4core/connector/base/ConnectorBase.h"
#include "soa4core/service/qosserviceidentifier/QoSServiceIdentifier.h"
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

#define START_MSG_NAME "Start Message"

Define_Module(Subscriber);

Subscriber::Subscriber() {
}

Subscriber::~Subscriber() {
}

void Subscriber::initialize()
{
    ServiceBase::initialize();
    handleParameterChange(nullptr);
    this->_rxPkSignal = registerSignal("rxPk");

    scheduleAt(simTime() + par("startTime").doubleValue(), new cMessage(START_MSG_NAME));
    if (getEnvir()->isGUI()) {
        getDisplayString().setTagArg("i2", 0, "status/asleep");
    }
}

void Subscriber::handleMessage(cMessage *msg)
{
    if(msg->isSelfMessage() && (strcmp(msg->getName(), START_MSG_NAME) == 0)){
        setQoS();
        //create a subscriber
        Manager* localServiceManager = nullptr;
        if (!(localServiceManager = dynamic_cast<Manager*>(_localServiceManager))){
            throw cRuntimeError("No Manager found.");
        }
        localServiceManager->registerSubscriberService(_subscriberApplicationInformation, this);
        std::vector<SubscriberConnector*> connectors = localServiceManager->getSubscriberConnectorsForServiceId(this->_publisherServiceId);
        _connector = nullptr;
        for (SubscriberConnector* subscriberConnector : connectors) {
            if (subscriberConnector->getSubscriberApplicationInformation().getQoSGroup() == _qosGroup) {
                _connector = subscriberConnector;
                break;
            }
        }
        if (!_connector) {
            throw cRuntimeError("No subscriber connector created.");
        }
        QoSServiceIdentifier publisherServiceIdentifier = QoSServiceIdentifier(this->_publisherServiceId,this->_instanceId);
        _localServiceManager->subscribeService(publisherServiceIdentifier, _subscriberApplicationInformation);
        if (getEnvir()->isGUI()) {
            getDisplayString().setTagArg("i2", 0, "status/active");
        }

    } else {
        EV_DEBUG << "Subscriber " << _subscriberName << " received a message."  << endl;
        //this is a subscription message so handle it.
        if (omnetpp::cPacket *frame = dynamic_cast<omnetpp::cPacket*>(msg))
        {
            emit(_rxPkSignal, frame);
        }
    }
    delete msg;
}

void Subscriber::setQoS() {
    _subscriberApplicationInformation = SubscriberApplicationInformation(_publisherServiceId, inet::L3AddressResolver().resolve(_localAddress.c_str()),
                                     _instanceId, _qosGroup, _tcpPort, _udpPort);
}

void Subscriber::handleParameterChange(const char* parname)
{
    ServiceBase::handleParameterChange(parname);

    if (!parname || !strcmp(parname, "publisherServiceId"))
    {
        this->_publisherServiceId = par("publisherServiceId").intValue();
    }

    if (!parname || !strcmp(parname, "subscriberName"))
    {
        this->_subscriberName = par("subscriberName").stdstringValue();
    }

    if (!parname || !strcmp(parname, "startTime"))
    {
        this->_startTime = CoRE4INET::parameterDoubleCheckRange(par("startTime"), 0, SIMTIME_MAX.dbl());
    }

    if (!parname || !strcmp(parname, "qosGroup")) {
        string group = par("qosGroup").stdstringValue();
        if(group == "WEB"){
            //_qosGroup = QoSGroups::WEB;
            throw cRuntimeError("WEB QoS is not implemented yet.");
        } else if(group == "STD_TCP"){
            _qosGroup = QoSGroup::STD_TCP;
        } else if(group == "STD_UDP"){
            _qosGroup = QoSGroup::STD_UDP;
        } else if(group == "RT"){
            _qosGroup = QoSGroup::RT;
        } else if(group == "SOMEIP_TCP"){
            _qosGroup = QoSGroup::SOMEIP_TCP;
        } else if(group == "SOMEIP_UDP") {
            _qosGroup = QoSGroup::SOMEIP_UDP;
        } else {
            throw cRuntimeError("Unknown QoS");
        }

    }

}

}/* end namespace SOA4CoRE */
