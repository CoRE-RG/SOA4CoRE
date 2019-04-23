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

#include <applications/gateway/gwsource/base/GWSourceAppBase.h>
#include <applications/publisherapp/base/PublisherAppBase.h>
#include <connector/pubsub/writer/PublisherWriter.h>
#include <qospolicy/avb/FramesizeQoSPolicy.h>
#include <qospolicy/avb/IntervalFramesQoSPolicy.h>
#include <qospolicy/avb/SRClassQoSPolicy.h>
#include <qospolicy/avb/StreamIDQoSPolicy.h>
#include <qospolicy/base/types/IntQoSPolicy.h>
#include <qospolicy/base/types/SizeQoSPolicy.h>
#include <qospolicy/base/types/UInt16QoSPolicy.h>
#include <qospolicy/management/QoSGroup.h>
#include <qospolicy/std/LocalAddressQoSPolicy.h>
#include <qospolicy/std/LocalPortQoSPolicy.h>
#include <servicemanager/LocalServiceManager.h>
#include <cstring>
#include <iostream>
#include <utility>

#include <core4inet/base/CoRE4INET_Defs.h>
#include <core4inet/utilities/ConfigFunctions.h>
#include <inet/linklayer/ethernet/Ethernet.h>
#include "signalsandgateways/gateway/messages/GatewayAggregationMessage.h"
#include "signalsandgateways/applications/ethernet/EthernetGatewayApplication.h"

namespace soqosmw {
using namespace inet;
using namespace CoRE4INET;
using namespace SignalsAndGateways;
using namespace std;


#define MY_INIT_STAGE_FIRST 1
#define MY_INIT_STAGE_FINAL 14

simsignal_t GWSourceAppBase::sigPayload = registerSignal("payloadSignal");

Define_Module(GWSourceAppBase);

GWSourceAppBase::GWSourceAppBase() {
    this->_enabled = false;
    this->_payload = 0;
}

GWSourceAppBase::~GWSourceAppBase() {
    delete _writer;
}

bool GWSourceAppBase::isEnabled() {
    return this->_enabled;
}

size_t GWSourceAppBase::getPayloadBytes() {
    handleParameterChange("payload");
    emit(sigPayload, static_cast<unsigned long>(this->_payload));
    return this->_payload;
}

void GWSourceAppBase::initialize(int stage) {
    switch(stage){
    case MY_INIT_STAGE_FIRST:
        SOQoSMWApplicationBase::initialize();
        handleParameterChange(nullptr);
        if (getPayloadBytes()
                <= (MIN_ETHERNET_FRAME_BYTES - ETHER_MAC_FRAME_BYTES
                        - ETHER_8021Q_TAG_BYTES)) {
            _framesize = MIN_ETHERNET_FRAME_BYTES;
        } else {
            _framesize =
                    getPayloadBytes() + ETHER_MAC_FRAME_BYTES + ETHER_8021Q_TAG_BYTES;
        }
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

        //schedule service registration
        if (isEnabled()) {
            scheduleAt(simTime() + par("startTime").doubleValue(),
                    new cMessage(START_MSG_NAME));
            if (getEnvir()->isGUI()) {
                getDisplayString().setTagArg("i2", 0, "status/asleep");
            }
        } else {
            if (getEnvir()->isGUI()) {
                getDisplayString().setTagArg("i2", 0, "status/stop");
            }
        }
        break;
    default:
        break;
    }
}

void GWSourceAppBase::handleParameterChange(const char* parname) {
    SOQoSMWApplicationBase::handleParameterChange(parname);

    if (!parname || !strcmp(parname, "enabled")) {
        this->_enabled = par("enabled").boolValue();
    }
    if (!parname || !strcmp(parname, "startTime")) {
        this->_startTime = CoRE4INET::parameterDoubleCheckRange(
                par("startTime"), 0, SIMTIME_MAX.dbl());
    }
    if (!parname || !strcmp(parname, "payload")) {
        this->_payload = CoRE4INET::parameterULongCheckRange(par("payload"), 0,
        MAX_ETHERNET_DATA_BYTES);
    }
    if (!parname || !strcmp(parname, "serviceName")) {
        this->_serviceName = par("serviceName").stdstringValue();
    }
    if (!parname || !strcmp(parname, "interval")) {
        this->_interval = CoRE4INET::parameterDoubleCheckRange(par("interval"),
                0, SIMTIME_MAX.dbl());;
    }
    if (!parname || !strcmp(parname, "intervalFrames")) {
        this->_intervalFrames = par("intervalFrames");
    }
    if (!parname || !strcmp(parname, "srClass")) {
        if (strcmp(par("srClass").stringValue(), "A") == 0) {
            this->_srClass = SR_CLASS::A;
        } else if (strcmp(par("srClass").stringValue(), "B") == 0) {
            this->_srClass = SR_CLASS::B;
        } else {
            throw cRuntimeError(
                    "Parameter srClass of %s is %s and is only allowed to be A or B",
                    getFullPath().c_str(), par("srClass").stringValue());
        }
    }
    if(!parname || !strcmp(parname, "canIds")){
        if (par("canIds").stdstringValue() != "") {
            //parse and add to cache
            cStringTokenizer dataFrameIDsTokenizer(par("canIds"), ",");
            _canIds = dataFrameIDsTokenizer.asIntVector();
        }
    }
    if (!parname || !strcmp(parname, "streamID")) {
        this->_streamID = parameterULongCheckRange(par("streamID"), 0,
                MAX_STREAM_ID);
    }
}

void GWSourceAppBase::handleMessage(cMessage *msg) {

    SOQoSMWApplicationBase::handleMessage(msg);
    if (msg->isSelfMessage() && (strcmp(msg->getName(), START_MSG_NAME) == 0)) {

        setQoS();
        //printQoS();

        //register this as new publisher app!
        _writer = getLocalServiceManager()->createPublisher(
                this->_serviceName, this->_qosPolicies, this);

        //schedule next send event
//        scheduleAt(simTime() + (this->_interval / this->_intervalFrames),
//                new cMessage(SEND_MSG_NAME));
        delete msg;
    }  else if(msg->arrivedOn("gwIn")){
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
//        cout << "Publisher " << _serviceName << " arrived on: " << msg->getArrivalGate()->getFullName() << ", on path: " <<
//                                msg->getFullPath() << endl;
        if(msg->arrivedOn("std_tcpIn") || msg->arrivedOn("std_udpIn")){
            //send(msg, gate("std_tcpIn")->getNextGate());
            _writer->notify(msg);
        } else{
            delete msg;
        }
    }

}

void GWSourceAppBase::setQoS() {
    std::string qosGroup = par("qosGroup").stdstringValue();
    if(qosGroup == "STD_TCP") {
        _qosPolicies[QoSPolicyNames::QoSGroup] = new QoSGroup (QoSGroup::STD_TCP);
        _qosPolicies[QoSPolicyNames::LocalPort] = new LocalPortQoSPolicy(getTcpPort());
    } else if(qosGroup == "STD_UDP") {
        _qosPolicies[QoSPolicyNames::QoSGroup] = new QoSGroup (QoSGroup::STD_UDP);
        _qosPolicies[QoSPolicyNames::LocalPort] = new LocalPortQoSPolicy(getUdpPort());
    } else {
        cRuntimeError("Not a valid connection type");
    }
    _qosPolicies[QoSPolicyNames::LocalAddress] = new LocalAddressQoSPolicy(getLocalAddress());
    _qosPolicies[QoSPolicyNames::StreamID] = new StreamIDQoSPolicy(_streamID);
    _qosPolicies[QoSPolicyNames::SRClass] = new SRClassQoSPolicy(_srClass);
    _qosPolicies[QoSPolicyNames::Framesize] = new FramesizeQoSPolicy(_framesize);
    _qosPolicies[QoSPolicyNames::IntervalFrames] = new IntervalFramesQoSPolicy(_intervalFrames);
}

void GWSourceAppBase::printQoS() {
    cout << "printing qos policies: [ ";
    for (auto policy : _qosPolicies){
        cout << policy.first << " ";
    }
    cout << "]" << endl << endl;

    cout << endl;
    cout << "checking values: " << endl;
    cout << "QoSGroup: " << (dynamic_cast<QoSGroup*>(_qosPolicies[QoSPolicyNames::QoSGroup]))->getValue() << endl;
    cout << "StreamID: " << (dynamic_cast<StreamIDQoSPolicy*>(_qosPolicies[QoSPolicyNames::StreamID]))->getValue() << endl;
    int value = (int)(dynamic_cast<SRClassQoSPolicy*>(_qosPolicies[QoSPolicyNames::SRClass]))->getValue();
    cout << "SRClass: " << value << endl;
    cout << "Framesize: " << (dynamic_cast<FramesizeQoSPolicy*>(_qosPolicies[QoSPolicyNames::Framesize]))->getValue() << endl;
    cout << "IntervalFrames: " << (dynamic_cast<IntervalFramesQoSPolicy*>(_qosPolicies[QoSPolicyNames::IntervalFrames]))->getValue() << endl;

}

} /* end namespace soqosmw */
