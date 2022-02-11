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

#include <soa4core/applications/publisher/base/Publisher.h>
#include "soa4core/servicemanager/LocalServiceManager.h"
#include "soa4core/connector/base/ConnectorBase.h"
//CoRE4INET
#include "core4inet/base/CoRE4INET_Defs.h"
#include "core4inet/utilities/ConfigFunctions.h"
//INET
#include "inet/linklayer/ethernet/Ethernet.h"
#include "inet/networklayer/common/L3AddressResolver.h"
//STD
#include <cstring>
#include <iostream>
#include <utility>


using namespace inet;
using namespace CoRE4INET;
using namespace std;

namespace SOA4CoRE {

Define_Module(Publisher);

Publisher::Publisher() {
    this->_enabled = true;
    this->_sigPayload = 0;
}

Publisher::~Publisher() {
}

bool Publisher::isEnabled() {
    return this->_enabled;
}

size_t Publisher::getPayloadBytes() {
    handleParameterChange("payload");
    emit(this->_sigPayload, static_cast<unsigned long>(this->_payload));
    return this->_payload;
}

void Publisher::initialize() {
    ServiceBase::initialize();
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

void Publisher::handleParameterChange(const char* parname) {
    ServiceBase::handleParameterChange(parname);

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

    if (!parname || !strcmp(parname, "qosGroups")) {
        cStringTokenizer stringTokenizer(par("qosGroups").stringValue()," ");
        while (stringTokenizer.hasMoreTokens()) {
            const char *token = stringTokenizer.nextToken();
            this->_qosGroups.insert(std::string(token));
        }
    }
}

void Publisher::createPublisherWithQoS() {
    setQoS();
    //printQoS();

    //register this as new publisher app!
    LocalServiceManager* localServiceManager = nullptr;
    if (!(localServiceManager = dynamic_cast<LocalServiceManager*>(_localServiceManager))){
        throw cRuntimeError("No LocalServiceManager found.");
    }
    localServiceManager->registerPublisherService(_publisherApplicationInformation, this);
    _connector = localServiceManager->getPublisherConnectorForServiceId(_publisherServiceId);
}

void Publisher::scheduleNextMessage() {
    //schedule next send event
    scheduleAt(simTime() + (this->_interval / this->_intervalFrames),
            new cMessage(SEND_MSG_NAME));
}

void Publisher::handleMessage(cMessage *msg) {

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

void Publisher::setQoS() {
    std::set<QoSGroup> qosGroups;
    for (std::string qosGroup : _qosGroups) {
        if (qosGroup == "STD_TCP") {
            qosGroups.insert(QoSGroup::STD_TCP);
        } else if (qosGroup == "STD_UDP") {
            qosGroups.insert(QoSGroup::STD_UDP);
        } else if (qosGroup == "SOMEIP_TCP") {
            qosGroups.insert(QoSGroup::SOMEIP_TCP);
        } else if (qosGroup == "SOMEIP_UDP") {
            qosGroups.insert(QoSGroup::SOMEIP_UDP);
        } else if (qosGroup == "RT") {
            qosGroups.insert(QoSGroup::RT);
        } else if (qosGroup == "WEB") {
            throw cRuntimeError("WEB QoS is not implemented yet");
        } else {
            throw cRuntimeError("Unknown QoS");
        }
    }

    _publisherApplicationInformation = PublisherApplicationInformation(_publisherServiceId,
                             inet::L3AddressResolver().resolve(_localAddress.c_str()),
                             _instanceId, qosGroups, _tcpPort, _udpPort, _streamID, _srClass, _framesize, _intervalFrames);
}

void Publisher::printQoS() {
    cout << "printing offered qos services:" << endl;
    cout << "Service ID: " << _publisherApplicationInformation.getServiceId() <<endl;
    cout << "StreamID: " << _publisherApplicationInformation.getStreamId() << endl;
    cout << "SRClass: " << CoRE4INET::SR_CLASStoString[_publisherApplicationInformation.getSrClass()] << endl;
    cout << "Framesize: " << _publisherApplicationInformation.getFramesize() << endl;
    cout << "IntervalFrames: " << _publisherApplicationInformation.getIntervalFrames() << endl;

}

} /* end namespace SOA4CoRE */
