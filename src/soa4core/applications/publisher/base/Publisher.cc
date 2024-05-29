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

#include "soa4core/applications/publisher/base/Publisher.h"
#include "soa4core/connector/publisher/PublisherConnector.h"
//CoRE4INET
#include <core4inet/base/CoRE4INET_Defs.h>
#include <core4inet/utilities/ConfigFunctions.h>
//INET
#include <inet/networklayer/ipv4/IPv4Datagram_m.h>
#include <inet/linklayer/ethernet/Ethernet.h>
#include <inet/networklayer/common/L3AddressResolver.h>
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
    this->_sigPayload = 0;
}

Publisher::~Publisher() {
}

size_t Publisher::getPayloadBytes() {
    handleParameterChange("payload");
    emit(this->_sigPayload, static_cast<unsigned long>(this->_payload));
    return this->_payload;
}

std::set<QoSGroup> Publisher::getQoSGroups() {
    return this->_qosGroups;
}

unsigned long Publisher::getStreamId() {
    return this->_streamID;
}

CoRE4INET::SR_CLASS Publisher::getSrClass() {
    return this->_srClass;
}

size_t Publisher::getPayloadMax() {
    return _payloadMax;
}

int Publisher::getIntervalFrames() {
    return this->_intervalFrames;
}

double Publisher::getIntervalMin() {
    return _intervalMin;
}

void Publisher::initialize() {
    ServiceBase::initialize();

    this->_msgSentSignal = registerSignal("msgSent");
    this->_sigPayload = registerSignal("payloadSignal");
}

void Publisher::handleParameterChange(const char* parname) {
    ServiceBase::handleParameterChange(parname);

    if (!parname || !strcmp(parname, "payload")) {
        this->_payload = par("payload").intValue();
    }
    if (!parname || !strcmp(parname, "intervalFrames")) {
        this->_intervalFrames = par("intervalFrames");
    }
    if (!parname || !strcmp(parname, "intervalMin")) {
        _intervalMin = CoRE4INET::parameterDoubleCheckRange(par("intervalMin"), 0, SIMTIME_MAX.dbl());
    }
    if (!parname || !strcmp(parname, "payloadMax")) {
        _payloadMax = par("payloadMax");
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
    if (!parname || !strcmp(parname, "vlan_id"))
    {
        if(par("vlan_id").intValue() >= 0){
            this->_vlan_id = static_cast<int>(parameterULongCheckRange(par("vlan_id"), 0, MAX_VLAN_ID));
        }
    }
    if (!parname || !strcmp(parname, "pcp"))
    {
        if(par("pcp").intValue() >= 0){
            this->_pcp = static_cast<int>(parameterULongCheckRange(par("pcp"), 0, MAX_Q_PRIORITY));
        }
    }
    if (!parname || !strcmp(parname, "mcastDestAddress")) {
        _mcastDestAddr = par("mcastDestAddress").stringValue();
    }
    if (!parname || !strcmp(parname, "mcastDestPort")) {
        _mcastDestPort = par("mcastDestPort");
    }
    if (!parname || !strcmp(parname, "preventReservation")) {
        _preventReservation = par("preventReservation");
    }

    if (!parname || !strcmp(parname, "qosGroups")) {
        std::set<std::string> qosGroups;
        cStringTokenizer stringTokenizer(par("qosGroups").stringValue()," ");
        while (stringTokenizer.hasMoreTokens()) {
            const char *token = stringTokenizer.nextToken();
            qosGroups.insert(std::string(token));
        }
        for (std::string qosGroup : qosGroups) {
            if (qosGroup == "IP_TCP" || qosGroup == "STD_TCP") {
                this->_qosGroups.insert(QoSGroup::IP_TCP);
            } else if (qosGroup == "IP_UDP" || qosGroup == "STD_UDP") {
                this->_qosGroups.insert(QoSGroup::IP_UDP);
            } else if (qosGroup == "IP_UDP_MCAST" || qosGroup == "STD_UDP_MCAST") {
                this->_qosGroups.insert(QoSGroup::IP_UDP_MCAST);
            } else if (qosGroup == "SOMEIP_TCP") {
                this->_qosGroups.insert(QoSGroup::SOMEIP_TCP);
            } else if (qosGroup == "SOMEIP_UDP") {
                this->_qosGroups.insert(QoSGroup::SOMEIP_UDP);
            } else if (qosGroup == "SOMEIP_UDP_MCAST") {
                this->_qosGroups.insert(QoSGroup::SOMEIP_UDP_MCAST);
            } else if (qosGroup == "RT") {
                this->_qosGroups.insert(QoSGroup::RT);
            } else if (qosGroup == "WEB") {
                throw cRuntimeError("WEB QoS is not implemented yet");
            } else {
                throw cRuntimeError("Unknown QoS");
            }
        }
    }
}

void Publisher::handleStart() {
    //printQoS();

    //register this as new publisher app!
    _connector = dynamic_cast<ConnectorBase*>(_localServiceManager->registerPublisherService(this));
    if (!_connector) {
        throw cRuntimeError("No publisher connector created.");
    }

    //schedule next send event
    scheduleNextMessage();
}

void Publisher::sendMessage() {
    if (_connector) {
        cPacket *payloadPacket = new cPacket();
        if(!_serviceName.empty())
        {
            payloadPacket->setName(_serviceName.c_str());
        }
        payloadPacket->setTimestamp();
        payloadPacket->setByteLength(
                static_cast<int64_t>(getPayloadBytes()));
        sendDirect(payloadPacket, _connector->gate("applicationIn"));
        EV_DEBUG << _serviceName << ": Message Published." << endl;

        emit(this->_msgSentSignal,payloadPacket);
    } else {
        throw cRuntimeError("No Publisher Registered for this app.");
    }
}

void Publisher::scheduleNextMessage() {
    //schedule next send event
    double interval = CoRE4INET::parameterDoubleCheckRange(par("interval"), 0, SIMTIME_MAX.dbl());
    scheduleAt(simTime() + (interval / this->_intervalFrames),
            new cMessage(SEND_MSG_NAME));
}

void Publisher::handleMessage(cMessage *msg) {

    if (msg->isSelfMessage()
            && (strcmp(msg->getName(), SEND_MSG_NAME) == 0)) {
        sendMessage();
        //schedule next send event
        scheduleNextMessage();
        delete msg;
    } else {
        ServiceBase::handleMessage(msg);
    }
}

void Publisher::printQoS() {
    cout << "printing offered qos services:" << endl;
    cout << "Service ID: " << _serviceId <<endl;
    cout << "StreamID: " << _streamID << endl;
    cout << "SRClass: " << CoRE4INET::SR_CLASStoString[_srClass] << endl;
    cout << "Framesize: " << _payloadMax << endl;
    cout << "IntervalFrames: " << _intervalFrames << endl;

}

} /* end namespace SOA4CoRE */
