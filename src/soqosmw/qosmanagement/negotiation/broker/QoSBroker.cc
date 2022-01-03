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

#include <soqosmw/messages/qosnegotiation/ConnectionSpecificInformation_m.h>
#include <soqosmw/messages/qosnegotiation/QoSNegotiationProtocol_m.h>
#include "soqosmw/qosmanagement/negotiation/broker/QoSBroker.h"

#include "soqosmw/qosmanagement/negotiation/datatypes/Request.h"
#include "soqosmw/servicemanager/LocalServiceManager.h"
#include "soqosmw/connector/base/ConnectorBase.h"
#include "soqosmw/endpoints/publisher/standard/udp/UDPPublisherEndpoint.h"
#include "soqosmw/endpoints/publisher/someip/udp/SOMEIPUDPPublisherEndpoint.h"
//AUTO-GENERATED Messages
#include <inet/networklayer/common/L3Address.h>
#include <inet/transportlayer/contract/udp/UDPSocket.h>
//STD
#include <iostream>
#include <unordered_map>


namespace SOQoSMW {
using namespace inet;
using namespace std;

QoSBroker::QoSBroker(UDPSocket* socket, LocalServiceManager* lsm, EndpointDescription local,
        EndpointDescription remote, Request* request) :
        _socket(socket), _lsm(lsm), _local(local), _remote(remote), _request(request), _startTimestamp(-1), _finishTimestamp(-1) {
    _negotiationFinished = false;
    if (request != nullptr) {
        _state = QoSBrokerStates_t::CLIENT_STARTUP;
    } else {
        _state = QoSBrokerStates_t::SERVER_NO_SESSION;
    }
}

QoSBroker::~QoSBroker() {
    if (_request) {
        delete _request;
    }
}

bool QoSBroker::handleMessage(QoSNegotiationProtocolMsg *msg) {
    bool handled = false;

    if (msg) {
        if (this->isResponsibleFor(msg->getReceiver(), msg->getSender())) {
            EV_DEBUG << "QoSBroker:" << " --> message received"
                            << " --> I am the receiver" << endl;

            switch (msg->getMessageType()) {
            case QoSNegotiationMsgType::QoS_Request:
                handled = handleRequest(
                        dynamic_cast<SOQoSMW::QoSNegotiationRequest*>(msg));
                break;
            case QoSNegotiationMsgType::QoS_Response:
                handled = handleResponse(
                        dynamic_cast<SOQoSMW::QoSNegotiationResponse*>(msg));
                break;
            case QoSNegotiationMsgType::QoS_Establish:
                handled = handleEstablish(
                        dynamic_cast<SOQoSMW::QoSNegotiationEstablish*>(msg));
                break;
            case QoSNegotiationMsgType::QoS_Finalise:
                handled = handleFinalise(
                        dynamic_cast<SOQoSMW::QoSNegotiationFinalise*>(msg));
                break;
            default:{
                EV_ERROR << "QoSBroker:" << " --> message received"
                                << " --> ERROR: Message type not set correctly! --> ignore it!"
                                << endl;
                break;
            } // default
            } // switch
        } else {
            // i am not responsible do nothing...
            EV_ERROR << "QoSBroker:" << " --> message received"
                            << " --> I am not responsible." << endl;
        }
    }
    return handled;
}

bool QoSBroker::startNegotiation() {
    bool handled = false;
    if (_state == QoSBrokerStates_t::CLIENT_STARTUP) {
        this->_startTimestamp = simTime();
        // create QoS Request Message
        QoSNegotiationRequest* request = new QoSNegotiationRequest("QoSNegotiationRequest");
        // fill envelope
        fillEnvelope(request);
        QoSGroup qosGroup =
                _request->getQosGroup();
        request->setQosClass(qosGroup);
        // set request size
        request->setByteLength(getNegotiationMessageSize(request));
        // send QoS Request
        sendMessage(request);
        EV_DEBUG << "QoSBroker: starting negotiation"
                        << " --> send negotiation request.";
        _state = QoSBrokerStates_t::CLIENT_PENDING_REQUEST;

        _request->setStatus(RequestStatus::REQUEST_SEND);

        handled = true;
    } else {
        _request->setStatus(RequestStatus::INVALID);
        finishNegotiation();
        EV_ERROR << "QoSBroker: starting negotiation"
                        << " --> Broker not in correct state to handle start signal. State is: "
                        << getStateAsName() << endl;
    }
    return handled;
}

bool QoSBroker::handleRequest(QoSNegotiationRequest* request) {
    bool handled = false;
    if (request) {
        if (_state == QoSBrokerStates_t::SERVER_NO_SESSION) {

            // request received --> check requirements
            bool requestAcceptables = isRequestAcceptable(request);

            // create response
            QoSNegotiationResponse* response = new QoSNegotiationResponse("QoSNegotiationResponse");
            fillEnvelope(response);
            response->setQosClass(request->getQosClass());

            if (requestAcceptables) {
                // successfull negotiation
                EV_DEBUG << "QoSBroker: received request"
                                << " --> request acceptable"
                                << " --> send response." << endl;

                response->setResponseStatus(QoSNegotiationStatus::Success);

                _state = QoSBrokerStates_t::SERVER_PENDING_ACCEPT;
            } else {
                // negotiation failed. QUIT
                EV_DEBUG << "QoSBroker: received request"
                                << " --> request unacceptable"
                                << " --> send response." << endl;
                finishNegotiation();

                //create accept payload
                response->setResponseStatus(QoSNegotiationStatus::Failure);
            }
            // set response size
            response->setByteLength(getNegotiationMessageSize(response));
            // send response
            sendMessage(response);
            _state = QoSBrokerStates_t::SERVER_PENDING_ACCEPT;
            handled = true;
        } else {
            EV_ERROR
                            << "QoSBroker: not in correct state to handle new request. State is: "
                            << getStateAsName() << endl;
        }
    } else {
        EV_ERROR << "QoSBroker: received invalid request." << endl;
    }
    return handled;
}

bool QoSBroker::handleResponse(QoSNegotiationResponse* response) {
    bool handled = false;
    if (response) {
        if (_state == QoSBrokerStates_t::CLIENT_PENDING_REQUEST) {
            // response received --> check contract
            EV_DEBUG << "QoSBroker: received response";

            if (response->getResponseStatus()
                    == QoSNegotiationStatus::Success) {
                // successfull negotiation
                EV_DEBUG << " --> negotiation successful";

                // create Connection request
                QoSNegotiationEstablish* establish =
                        new QoSNegotiationEstablish("QoSNegotiationEstablish");
                fillEnvelope(establish);
                establish->setQosClass(response->getQosClass());

                ConnectionSpecificInformation* connectionlessCSI = nullptr;
                switch (response->getQosClass()) {
                case QoSGroup::STD_UDP:
                    connectionlessCSI = new CSI_UDP();
                case QoSGroup::SOMEIP_UDP: {
                    connectionlessCSI = connectionlessCSI ? connectionlessCSI : new CSI_SOMEIP_UDP();
                    // create or find the subscriber
                    SubscriberEndpointBase* sub = _lsm->createOrFindSubscriberFor(_remote.getServiceId(),connectionlessCSI);

                    delete connectionlessCSI;

                    if(sub){
                        ConnectionSpecificInformation* info = sub->getConnectionSpecificInformation();
                        if(info){
                            establish->encapsulate(info);
                        } else {
                            throw cRuntimeError("Did not receive a connection info...");
                        }
                    } else {
                        throw cRuntimeError("No subscriber was created...");
                    }
                    break;
                }
                default:
                    break;
                }
                // set establish size
                establish->setByteLength(getNegotiationMessageSize(establish));
                // send connection establish
                sendMessage(establish);
                EV_DEBUG << " --> send establish request" << endl;
                _request->setStatus(RequestStatus::ESTABLISH_SEND);
                _state = QoSBrokerStates_t::CLIENT_PENDING_CONNECTION;
            } else {
                // negotiation failed. QUIT
                EV_DEBUG << " --> negotiation Failed";
                _request->setStatus(RequestStatus::ACKKNOWLEDGED_FAILURE);
                finishNegotiation();
            }
            handled = true;
        } else {
            EV_ERROR
                            << "QoSBroker: not in correct state to handle new request. State is: "
                            << getStateAsName() << endl;
        }
    } else {
        EV_ERROR << "QoSBroker: received invalid request." << endl;
    }
    return handled;
}

bool QoSBroker::handleEstablish(QoSNegotiationEstablish* establish) {
    bool handled = false;
    if (establish) {
        if (_state == QoSBrokerStates_t::SERVER_PENDING_ACCEPT) {
            EV_DEBUG << "QoSBroker: received establish";
            // create response
            QoSNegotiationFinalise* finalise = new QoSNegotiationFinalise("QoSNegotiationFinalise");
            fillEnvelope(finalise);
            finalise->setQosClass(establish->getQosClass());

            if (isEstablishAcceptable(establish)) {

                // create or find the publisher
                PublisherEndpointBase* pub = _lsm->createOrFindPublisherFor(_local.getServiceId(), QoSGroup(establish->getQosClass()));

                if(pub){
                    // encapsulate the CSI into the packet.
                    ConnectionSpecificInformation* info = pub->getConnectionSpecificInformation();
                    if(info){
                        finalise->encapsulate(info);

                        if (CSI_UDP* csiUdp = dynamic_cast<CSI_UDP*>(info)) {
                            if(ConnectionSpecificInformation* subConnection = dynamic_cast<ConnectionSpecificInformation*>( establish->decapsulate())){
                                if (UDPPublisherEndpoint* udpPublisher = dynamic_cast<UDPPublisherEndpoint*> (pub)) {
                                    switch (info->getConnectionType()) {
                                    case ConnectionType::ct_udp:
                                        udpPublisher->addRemote(subConnection);
                                        break;
                                    case ConnectionType::ct_someip_udp:
                                        // SOME/IP SD
                                        udpPublisher->addRemote(subConnection);
                                        break;
                                    }
                                }
                                delete subConnection;
                            }
                        }
                        // successful negotiation
                        finalise->setFinalStatus(QoSNegotiationStatus::Success);
                        EV_DEBUG << " --> finalise success" << endl;
                        _state = QoSBrokerStates_t::SERVER_SESSION_ESTABLISHED;
                    } else {
                        // negotiation failed. QUIT
                        finalise->setFinalStatus(QoSNegotiationStatus::Failure);
                        EV_DEBUG << " --> finalise failed" << endl;
                        _state = QoSBrokerStates_t::SERVER_FAILURE;
                        throw cRuntimeError("Did not receive a connection info...");
                    }
                } else {
                    // negotiation failed. QUIT
                    finalise->setFinalStatus(QoSNegotiationStatus::Failure);
                    EV_DEBUG << " --> finalise failed" << endl;
                    _state = QoSBrokerStates_t::SERVER_FAILURE;
                    throw cRuntimeError("No publisher was created...");
                }

            } else {
                // negotiation failed. QUIT
                finalise->setFinalStatus(QoSNegotiationStatus::Failure);
                EV_DEBUG << " --> finalise failed" << endl;
                _state = QoSBrokerStates_t::SERVER_FAILURE;
            }
            finishNegotiation();
            // set finalise size
            finalise->setByteLength(getNegotiationMessageSize(finalise));
            sendMessage(finalise);
            handled = true;
        } else {
            EV_ERROR
                            << "QoSBroker: not in correct state to handle new request. State is: "
                            << getStateAsName() << endl;
        }
    } else {
        EV_ERROR << "QoSBroker: received invalid request." << endl;
    }
    return handled;
}

bool QoSBroker::handleFinalise(QoSNegotiationFinalise* finalise) {
    bool handled = false;
    if (finalise) {
        if (_state == QoSBrokerStates_t::CLIENT_PENDING_CONNECTION) {
//            cout << "QoSBroker: full cycle successfull" << endl;

            if(finalise->getFinalStatus() == QoSNegotiationStatus::Success){
                _state = QoSBrokerStates_t::CLIENT_SUCCESS;
                _request->setStatus(RequestStatus::FINALISED_SUCCESS);

                // get connection specific information
                ConnectionSpecificInformation* info = dynamic_cast<SOQoSMW::ConnectionSpecificInformation*>(finalise->decapsulate());
                if(info){

                    // do not create another connectionless subscriber!
                    switch (info->getConnectionType()) {
                    case ConnectionType::ct_udp:
                    case ConnectionType::ct_someip_udp:
                        break;
                    default:
                        // create or find the subscriber
                        SubscriberEndpointBase* sub = _lsm->createOrFindSubscriberFor(_remote.getServiceId(),info);

                        if(!sub){
                            throw cRuntimeError("No subscriber was created...");
                        }
                        break;
                    }

                    delete info;
                }


            }else{
                _state = QoSBrokerStates_t::CLIENT_FAILURE;
                _request->setStatus(RequestStatus::FINALISED_FAILURE);
            }
            finishNegotiation();
            handled = true;
            this->_finishTimestamp = simTime();
        } else {
            EV_ERROR
                            << "QoSBroker: not in correct state to handle new request. State is: "
                            << getStateAsName() << endl;
        }
    } else {
        EV_ERROR << "QoSBroker: received invalid request." << endl;
    }
    return handled;
}

bool QoSBroker::isRequestAcceptable(QoSNegotiationRequest* request) {
    // get payload

    // check if service exists on this node and get a reference

    // ask service if it can handle the request

    return true;
}

bool QoSBroker::isEstablishAcceptable(QoSNegotiationEstablish* establish) {
    return true;
}

void QoSBroker::fillEnvelope(SOQoSMW::Envelope* envelope) {
    // set receiver
    EndpointDescription receiver(_remote);
    envelope->setReceiver(receiver);
    // set sender
    EndpointDescription sender(_local);
    envelope->setSender(sender);
}

void QoSBroker::sendMessage(QoSNegotiationProtocolMsg* payload_packet) {
    // payload_packet->setByteLength(sizeof(*payload_packet));
    _socket->sendTo(payload_packet, _remote.getNetworkAddr(),
            _remote.getNetworkPort());
}

string QoSBroker::getStateAsName() {
    switch (_state) {
    case SERVER_NO_SESSION:
        return "SERVER_NO_SESSION";
    case SERVER_PENDING_ACCEPT:
        return "SERVER_PENDING_ACCEPT";
    case SERVER_SESSION_ESTABLISHED:
        return "SERVER_SESSION_ESTABLISHED";
    case SERVER_FAILURE:
        return "SERVER_FAILURE";

    case CLIENT_STARTUP:
        return "CLIENT_STARTUP";
    case CLIENT_PENDING_REQUEST:
        return "CLIENT_PENDING_REQUEST";
    case CLIENT_PENDING_CONNECTION:
        return "CLIENT_PENDING_CONNECTION";
    case CLIENT_FAILURE:
        return "CLIENT_FAILURE";
    case CLIENT_SUCCESS:
        return "SUCCESS";
    default:
        return "-.-";
    }
}

bool QoSBroker::isResponsibleFor(EndpointDescription& local,
        EndpointDescription& remote) {
    return local == _local && remote == _remote;
}

void QoSBroker::finishNegotiation() {
    _negotiationFinished = true;
}

simtime_t QoSBroker::getStartTimestamp() {
    return this->_startTimestamp;
}

simtime_t QoSBroker::getFinishTimestamp() {
    return this->_finishTimestamp;
}

size_t QoSBroker::getNegotiationMessageSize(QoSNegotiationProtocolMsg* msg) {
    /*                     QoSNPMsg      QoSNPMsg      Envelope        Envelope      Envelope
     *                    messageType    qosClass      protocol     EndpointSender EndpointReceiv
     *                        |             |             |               |             |
     *                        v             v             v               v             v
     **/
    size_t minimumSize = sizeof(int) + sizeof(int) + sizeof(int) + 2*sizeof(int) + 2*sizeof(int);
    size_t pathsSize = sizeof(msg->getSender().getServiceId()) + sizeof(msg->getReceiver().getServiceId());
    size_t result = 0;
    result = minimumSize + pathsSize;
    switch (msg->getMessageType()) {
        case QoSNegotiationMsgType::QoS_Response:
            result += sizeof(int); // responseStatus
            break;
        case QoSNegotiationMsgType::QoS_Finalise:
            result += sizeof(int); // finalStatus
            break;
        default: break;
        }
    return result;
}

bool QoSBroker::isQoSGroup(QoSGroup qosGroup) {
    return _request->getQosGroup() == qosGroup;
}

} /* namespace SOQoSMW */
