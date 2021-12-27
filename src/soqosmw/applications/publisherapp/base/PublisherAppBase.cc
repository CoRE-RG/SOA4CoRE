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

#include "soqosmw/applications/publisherapp/base/PublisherAppBase.h"
#include "soqosmw/qospolicy/base/qospolicy.h"
#include "soqosmw/servicemanager/LocalServiceManager.h"
#include "soqosmw/connector/base/ConnectorBase.h"
//CoRE4INET
#include "core4inet/base/CoRE4INET_Defs.h"
#include "core4inet/utilities/ConfigFunctions.h"
//INET
#include "inet/linklayer/ethernet/Ethernet.h"
//STD
#include <cstring>
#include <iostream>
#include <utility>


using namespace inet;
using namespace CoRE4INET;
using namespace std;

namespace SOQoSMW {

Define_Module(PublisherAppBase);

PublisherAppBase::PublisherAppBase() {
    this->_enabled = true;
    this->_sigPayload = 0;
}

PublisherAppBase::~PublisherAppBase() {
}

bool PublisherAppBase::isEnabled() {
    return this->_enabled;
}

size_t PublisherAppBase::getPayloadBytes() {
    handleParameterChange("payload");
    emit(this->_sigPayload, static_cast<unsigned long>(this->_payload));
    return this->_payload;
}

void PublisherAppBase::initialize() {
    SOQoSMWApplicationBase::initialize();
    handleParameterChange(nullptr);

    this->_msgSentSignal = registerSignal("msgSent");
    this->_sigPayload = registerSignal("payloadSignal");
    _framesize = getPayloadBytes();
    if (getPayloadBytes()
            <= (MIN_ETHERNET_FRAME_BYTES - ETHER_MAC_FRAME_BYTES
                    - ETHER_8021Q_TAG_BYTES)) {
        _framesize = MIN_ETHERNET_FRAME_BYTES;
    } else {
        _framesize =
                getPayloadBytes() + ETHER_MAC_FRAME_BYTES + ETHER_8021Q_TAG_BYTES;
    }

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
}

void PublisherAppBase::handleParameterChange(const char* parname) {
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
        this->_publisherName = par("serviceName").stdstringValue();
    }
    if (!parname || !strcmp(parname, "serviceId"))
    {
        this->_publisherServiceId = par("serviceId").intValue();
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
    if (!parname || !strcmp(parname, "streamID")) {
        this->_streamID = parameterULongCheckRange(par("streamID"), 0,
                static_cast<unsigned long>(MAX_STREAM_ID));
    }
}

void PublisherAppBase::createPublisherWithQoS() {
    setQoS();
    //printQoS();

    //register this as new publisher app!
    LocalServiceManager* localServiceManager = nullptr;
    if (!(localServiceManager = dynamic_cast<LocalServiceManager*>(_localServiceManager))){
        throw cRuntimeError("No LocalServiceManager found.");
    }
    localServiceManager->registerPublisherService(this->_publisherServiceId, this->_qosPolicies, this, _instanceId);
    if ((_connector = localServiceManager->getPublisherConnector(this->_publisherServiceId)) == nullptr){
        throw cRuntimeError("PublisherConnector is null. Call registerPublisherService before.");
    }
}

void PublisherAppBase::scheduleNextMessage() {
    //schedule next send event
    scheduleAt(simTime() + (this->_interval / this->_intervalFrames),
            new cMessage(SEND_MSG_NAME));
}

void PublisherAppBase::handleMessage(cMessage *msg) {

    if (msg->isSelfMessage() && (strcmp(msg->getName(), START_MSG_NAME) == 0)) {

        createPublisherWithQoS();

        //schedule next send event
        scheduleNextMessage();
        if (getEnvir()->isGUI()) {
            getDisplayString().setTagArg("i2", 0, "status/active");
        }
        delete msg;

    } else if (msg->isSelfMessage()
            && (strcmp(msg->getName(), SEND_MSG_NAME) == 0)) {
        if (_connector) {
            cPacket *payloadPacket = new cPacket(std::to_string(_publisherServiceId).c_str());
            payloadPacket->setTimestamp();
            payloadPacket->setByteLength(
                    static_cast<int64_t>(getPayloadBytes()));
            sendDirect(payloadPacket, _connector->gate("applicationIn"));
            EV_DEBUG << _publisherName << ": Message Published." << endl;

            //schedule next send event
            scheduleNextMessage();
            emit(this->_msgSentSignal,payloadPacket);
        } else {
            throw cRuntimeError("No Publisher Registered for this app.");
        }
        delete msg;


    } else {
        delete msg;
    }

}

void PublisherAppBase::setQoS() {
    _qosPolicies[QoSPolicyNames::LocalPort] = new LocalPortQoSPolicy(_tcpPort);
    _qosPolicies[QoSPolicyNames::LocalPort] = new LocalPortQoSPolicy(_udpPort);
    _qosPolicies[QoSPolicyNames::LocalAddress] = new LocalAddressQoSPolicy(_localAddress);
    _qosPolicies[QoSPolicyNames::StreamID] = new StreamIDQoSPolicy(_streamID);
    _qosPolicies[QoSPolicyNames::SRClass] = new SRClassQoSPolicy(_srClass);
    _qosPolicies[QoSPolicyNames::Framesize] = new FramesizeQoSPolicy(_framesize);
    _qosPolicies[QoSPolicyNames::IntervalFrames] = new IntervalFramesQoSPolicy(_intervalFrames);
}

void PublisherAppBase::printQoS() {
    cout << "printing qos policies: [ ";
    for (auto policy : _qosPolicies){
        cout << policy.first << " ";
    }
    cout << "]" << endl << endl;

    cout << endl;
    cout << "checking values: " << endl;
    cout << "StreamID: " << (dynamic_cast<StreamIDQoSPolicy*>(_qosPolicies[QoSPolicyNames::StreamID]))->getValue() << endl;
    int value = (int)(dynamic_cast<SRClassQoSPolicy*>(_qosPolicies[QoSPolicyNames::SRClass]))->getValue();
    cout << "SRClass: " << value << endl;
    cout << "Framesize: " << (dynamic_cast<FramesizeQoSPolicy*>(_qosPolicies[QoSPolicyNames::Framesize]))->getValue() << endl;
    cout << "IntervalFrames: " << (dynamic_cast<IntervalFramesQoSPolicy*>(_qosPolicies[QoSPolicyNames::IntervalFrames]))->getValue() << endl;

}

} /* end namespace SOQoSMW */
