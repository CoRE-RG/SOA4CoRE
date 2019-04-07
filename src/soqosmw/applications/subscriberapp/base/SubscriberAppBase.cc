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

#include <applications/subscriberapp/base/SubscriberAppBase.h>
#include <connector/pubsub/reader/SubscriptionReader.h>
#include <messages/QoSNegotiationProtocol/QoSNegotiationProtocol_m.h>
#include <omnetpp/cdisplaystring.h>
#include <omnetpp/cenvir.h>
#include <omnetpp/cgate.h>
#include <omnetpp/clog.h>
#include <omnetpp/cmessage.h>
#include <omnetpp/cnamedobject.h>
#include <omnetpp/cobjectfactory.h>
#include <omnetpp/cpar.h>
#include <omnetpp/csimplemodule.h>
#include <omnetpp/csimulation.h>
#include <omnetpp/regmacros.h>
#include <omnetpp/simtime.h>
#include <omnetpp/simtime_t.h>
#include <omnetpp/simutil.h>
#include <qospolicy/management/QoSGroup.h>
#include <qospolicy/std/LocalAddressQoSPolicy.h>
#include <qospolicy/std/LocalPortQoSPolicy.h>
#include <servicemanager/LocalServiceManager.h>
#include <cstring>
#include <iostream>

#include <core4inet/utilities/ConfigFunctions.h>
#include <inet/linklayer/ethernet/EtherFrame_m.h>
#include <core4inet/base/avb/AVBDefs.h>

namespace soqosmw {
using namespace std;
using namespace CoRE4INET;

#define START_MSG_NAME "Start Message"

Define_Module(SubscriberAppBase);

simsignal_t SubscriberAppBase::_rxPkSignal = registerSignal("rxPk");

SubscriberAppBase::SubscriberAppBase() {
    _reader = nullptr;
}

SubscriberAppBase::~SubscriberAppBase() {

    delete _reader;
}

void SubscriberAppBase::initialize()
{
    SOQoSMWApplicationBase::initialize();
    handleParameterChange(nullptr);

    scheduleAt(simTime() + par("startTime").doubleValue(), new cMessage(START_MSG_NAME));
    if (getEnvir()->isGUI())
    {
        getDisplayString().setTagArg("i2", 0, "status/asleep");
    }

    if (getEnvir()->isGUI())
    {
        getDisplayString().setTagArg("i2", 0, "status/hourglass");
    }
}

void SubscriberAppBase::handleMessage(cMessage *msg)
{
    SOQoSMWApplicationBase::handleMessage(msg);
    if(msg->isSelfMessage() && (strcmp(msg->getName(), START_MSG_NAME) == 0)){
        setQoS();
        //create a subscriber
        _reader = getLocalServiceManager()->createSubscriber(this->_subscriberName, this->_publisherName, this->_qosPolicies, this);

        //TODO set the gate at the reader to get all messages

        delete msg;
    }else {
        EV_DEBUG << "Subscriber " << _subscriberName << " received a message."  << endl;


        if(msg->arrivedOn("std_tcpIn") || msg->arrivedOn("std_udpIn")){
            //send(msg, gate("std_tcpIn")->getNextGate());

            _reader->notify(msg);

        }else if (inet::EtherFrame *frame = dynamic_cast<inet::EtherFrame*>(msg))
        {
            emit(_rxPkSignal, frame);
            delete msg;
        } else {
            delete msg;
        }

    }
}

void SubscriberAppBase::notify(cPacket* msg) {
    Enter_Method("SubscriberAppBase::notify()");
    EV_DEBUG << "Subscriber " << _subscriberName << " received a message."  << endl;

    if (inet::EtherFrame *frame = dynamic_cast<inet::EtherFrame*>(msg))
    {
        emit(_rxPkSignal, frame);
    }

    delete msg;
}

void SubscriberAppBase::setQoS() {
    _qosPolicies[QoSPolicyNames::QoSGroup] = _qosGroup;
    _qosPolicies[QoSPolicyNames::LocalAddress] = new LocalAddressQoSPolicy(getLocalAddress());
    std::string connectiontype = par("connectionType").stdstringValue();
    if(connectiontype == "connectionbased") {
            _qosPolicies[QoSPolicyNames::LocalPort] = new LocalPortQoSPolicy(getTcpPort());
    } else if(connectiontype == "connectionless") {
            _qosPolicies[QoSPolicyNames::LocalPort] = new LocalPortQoSPolicy(getUdpPort());
    } else {
        cRuntimeError("Not a valid connection type");
    }
}

void SubscriberAppBase::handleParameterChange(const char* parname)
{
    SOQoSMWApplicationBase::handleParameterChange(parname);

    if (!parname || !strcmp(parname, "subscriberName"))
    {
        this->_subscriberName = par("subscriberName").stdstringValue();
    }

    if (!parname || !strcmp(parname, "publisherName"))
    {
        this->_publisherName = par("publisherName").stdstringValue();
    }
    if (!parname || !strcmp(parname, "startTime"))
    {
        this->_startTime = CoRE4INET::parameterDoubleCheckRange(par("startTime"), 0, SIMTIME_MAX.dbl());
    }

    if (!parname || !strcmp(parname, "qosGroup")) {
        string group = par("qosGroup").stdstringValue();
        if(group == "WS"){
            _qosGroup = new QoSGroup(QoSGroup::WEB);
        } else if(group == "STD_TCP"){
            _qosGroup = new QoSGroup(QoSGroup::STD_TCP);
        } else if(group == "STD_UDP"){
            _qosGroup = new QoSGroup(QoSGroup::STD_UDP);
        } else if(group == "RT"){
            _qosGroup = new QoSGroup(QoSGroup::RT);
        }

    }




}

}/* end namespace soqosmw */
