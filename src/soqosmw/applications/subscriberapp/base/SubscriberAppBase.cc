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
#include <omnetpp/cdisplaystring.h>
#include <omnetpp/cenvir.h>
#include <omnetpp/cmessage.h>
#include <omnetpp/cnamedobject.h>
#include <omnetpp/cobjectfactory.h>
#include <omnetpp/cpar.h>
#include <omnetpp/csimulation.h>
#include <omnetpp/regmacros.h>
#include <omnetpp/simtime.h>
#include <omnetpp/simtime_t.h>
#include <qospolicy/base/QoSPolicyFactory.h>
#include <qospolicy/group/QoSGroup.h>
#include <servicemanager/LocalServiceManager.h>
#include <cstring>
#include <iostream>

#include <core4inet/utilities/ConfigFunctions.h>
#include <inet/linklayer/ethernet/EtherFrame_m.h>


namespace soqosmw {
using namespace std;
using namespace CoRE4INET;

#define START_MSG_NAME "Start Message"

Define_Module(SubscriberAppBase);

simsignal_t SubscriberAppBase::_rxPkSignal = registerSignal("rxPk");

SubscriberAppBase::SubscriberAppBase() {
    _subscriber = nullptr;
}

SubscriberAppBase::~SubscriberAppBase() {
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
        _subscriber = getLocalServiceManager()->createSubscriber(this->_subscriberName, this->_publisherName, this->_qosPolicies, this);

    } else {
        cout << "SUBSCRIBER RECEIVED A MESSAGE :-D" << endl;

        if (inet::EtherFrame *frame = dynamic_cast<inet::EtherFrame*>(msg))
        {
            emit(_rxPkSignal, frame);
        }

    }

    delete msg;
}

void SubscriberAppBase::setQoS() {
    _qosPolicies[QoSGroup::getName()] = QoSGroup (QoSGroups::RT);
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

    //TODO add real QoS Policies
    _qosPolicies = QoSPolicyFactory::extractPolicyFromPar(par("qosPolicies"));
}

}/* end namespace soqosmw */
