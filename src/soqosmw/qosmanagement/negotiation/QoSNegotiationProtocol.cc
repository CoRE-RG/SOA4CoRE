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

#include <soqosmw/messages/qosnegotiation/QoSNegotiationProtocol_m.h>
#include "soqosmw/base/EndpointDescription.h"
#include "soqosmw/qosmanagement/negotiation/datatypes/Request.h"
#include "soqosmw/qosmanagement/negotiation/QoSNegotiationProtocol.h"
#include "soqosmw/servicemanager/LocalServiceManager.h"
//AUTO-GENERATED MESSAGES
#include "inet/networklayer/common/L3AddressResolver.h"
//STD
#include <cstring>
#include <iostream>
#include <iterator>

namespace SOQoSMW {
using namespace std;
using namespace inet;

#define MSGKIND_CREATEBROKERREQUEST     1

Define_Module(QoSNegotiationProtocol);

QoSNegotiationProtocol::QoSNegotiationProtocol() {
    _socketBound = false;
}

QoSNegotiationProtocol::~QoSNegotiationProtocol() {
}

void QoSNegotiationProtocol::initialize(int stage) {

    if(stage == INITSTAGE_LOCAL) {
        this->_qosNt = registerSignal("qosNt");
        this->_rxPkSignal = registerSignal("rxPk");
        ProcessingTimeSimulation::initialize();
    }
    if (stage == INITSTAGE_APPLICATION_LAYER) {
        handleParameterChange(nullptr);
        _lsm = dynamic_cast<LocalServiceManager*>(getParentModule()->getSubmodule(
                               par("lsmmoduleName")));
        if (!isSocketBound()) {
            socketSetup();
        }
    }
}

void QoSNegotiationProtocol::processScheduledMessage(cMessage* msg) {

    if (msg->isSelfMessage() && msg->getKind() == MSGKIND_CREATEBROKERREQUEST) {
        // extract request from message
        Request* request = (Request*) msg->getContextPointer();

        //create broker as requested
        QoSBroker* broker = new QoSBroker(&_socket, _lsm, request->getSubscriberEndpointDescription(), request->getPublisherEndpointDescription(), request);

        //tell broker to start the request.
        bool handled = broker->startNegotiation();

        if(handled) {
            //Add broker to list.
            _brokers.push_back(broker);
        }

    } else {

        if (auto as_envelope = dynamic_cast<SOQoSMW::Envelope*>(msg)) {
            emit(_rxPkSignal, as_envelope);
            //is in SOQoSMW::Envelope --> check other types
            if (auto as_negotiation =
                    dynamic_cast<SOQoSMW::QoSNegotiationProtocolMsg*>(as_envelope)) {

                //let the right broker handle the message.
                bool handled = false;
                for (std::vector<QoSBroker*>::iterator broker = _brokers.begin();
                        broker != _brokers.end(); ++broker) {
                    if ((*broker)->isResponsibleFor(as_negotiation->getReceiver(),
                            as_negotiation->getSender())) {
                        handled = (*broker)->handleMessage(as_negotiation);
                        if ((*broker)->isNegotiationFinished()) {
                            // emit duration of negotiation
                            simtime_t startTimestamp = (*broker)->getStartTimestamp();
                            simtime_t finishTimestamp = (*broker)->getFinishTimestamp();
                            if (startTimestamp != -1 && finishTimestamp != -1) {
                                emit(this->_qosNt,finishTimestamp - startTimestamp);
                            }
                            _brokers.erase(broker);
                        }
                        break;
                    }
                }
                //check if message was handled, else we need a new broker.
                if (!handled) {
                    //create new broker
                    QoSBroker* broker = new QoSBroker(&_socket, _lsm,
                            as_negotiation->getReceiver(),
                            as_negotiation->getSender(), nullptr);

                    handled = broker->handleMessage(as_negotiation);
                    if (!handled) {
                        // broker is not needed because request was invalid.
                        delete broker;
                        EV_ERROR << "QoSNegotiationProtocol:"
                                        << " --> message received"
                                        << " --> Message could not be handled by any Broker.";
                    } else {
                        // handled so broker will be needed for ongoing negotiation.
                        _brokers.push_back(broker);
                    }
                }

            } else {
                EV_WARN << "QoSNegotiationProtocol:" << " --> message received"
                               << " --> not of type SOQoSMW::QoSNegotiationProtocolMsg --> ignore it!"
                               << endl;
            }
        } else {
            EV_WARN << "QoSNegotiationProtocol:" << " --> message received"
                           << " --> not in SOQoSMW::Envelope --> ignore it!"
                           << endl;
        }
    }

    delete msg;
}

void QoSNegotiationProtocol::handleParameterChange(const char* parname) {
    ProcessingTimeSimulation::handleParameterChange(parname);

    //read UDP Common Parameters
    if (!parname || !strcmp(parname, "protocolPort")) {
        _protocolPort = par("protocolPort");
    }
    if (!parname || !strcmp(parname, "localAddress")) {
        const char* localAddr = par("localAddress");
        _localAddress = L3AddressResolver().resolve(localAddr);
    }
}

int QoSNegotiationProtocol::getProtocolPort() {
    Enter_Method("QOSNP::getProtocolPort()");
    return _protocolPort;
}

void QoSNegotiationProtocol::socketSetup() {
    _socket.setOutputGate(gate("udpOut"));
    _socket.bind(_localAddress, _protocolPort);
    _socketBound = true;
}

bool QoSNegotiationProtocol::isSocketBound() {
    return _socketBound;
}

void QoSNegotiationProtocol::socketClose() {
    _socket.close();
    _socketBound = false;
}

void QoSNegotiationProtocol::createQoSBroker(Request* request) {
    Enter_Method("QOSNP::createQoSBroker()");

    // create self message
    cMessage* msg = new cMessage("CreateBrokerRequest");
    msg->setKind(MSGKIND_CREATEBROKERREQUEST);
    msg->setContextPointer(request);

    // call handle message to simulate processing time if needed.
    this->handleMessage(msg);
}

}
/* namespace SOQoSMW */
