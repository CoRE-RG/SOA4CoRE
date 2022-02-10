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

#include "AVBSubscriberEndpoint.h"

#include "soa4core/applications/base/MiddlewareApplicationBase.h"
#include "soa4core/connector/pubsub/reader/SubscriberConnector.h"

//CoRE4INET
#include "core4inet/base/NotifierConsts.h"
#include "core4inet/utilities/ConfigFunctions.h"
//AUTO-GENERATED MESSAGES
#include "core4inet/linklayer/ethernet/avb/AVBFrame_m.h"

using namespace std;
using namespace CoRE4INET;

namespace SOA4CoRE {

Define_Module(AVBSubscriberEndpoint);

ConnectionSpecificInformation* AVBSubscriberEndpoint::getConnectionSpecificInformation() {
    CSI_AVB* connection = new CSI_AVB();
    connection->setStreamID(_streamID);
    connection->setVlanID(_vlanID);

    return connection;
}

void AVBSubscriberEndpoint::initialize() {
    _updateMessage = nullptr;
    // get owning app
    MiddlewareApplicationBase* app = _subscriberConnector->getApplications()[0];
    if (!app) {
        throw cRuntimeError(
                "Owning application not found in init of publisher.");
    }

    //find srp table
    _srpTable = check_and_cast<SRPTable *>(
            app->getParentModule()->getSubmodule("srpTable"));
    if (!_srpTable) {
        throw cRuntimeError(
                "srpTable module required for stream reservation but not found");
    }

    //init base class
    RTSubscriberEndpointBase::initialize();
}

void AVBSubscriberEndpoint::initializeTransportConnection() {
    // register for srp updates
    _srpTable->subscribe(NF_AVB_TALKER_REGISTERED, this);
    _srpTable->subscribe(NF_AVB_LISTENER_REGISTRATION_TIMEOUT, this);

    // register listener
    if (_srpTable->getTalkerEntryForStreamId(_streamID, _vlanID)) {
        _srpTable->updateListenerWithStreamId(_streamID, this, _vlanID);
        _isConnected = true;
    }

    if (_updateInterval > 0) {
        scheduleUpdateMessage(simTime() + _updateInterval);
    }
}

void AVBSubscriberEndpoint::receiveSignal(cComponent* src, simsignal_t id,
        cObject* obj, cObject* details) {
    Enter_Method_Silent
    ();
    if (id == NF_AVB_TALKER_REGISTERED) {
        SRPTable::TalkerEntry *tentry = check_and_cast<SRPTable::TalkerEntry*>(
                obj);

        //If talker for the desired stream, register Listener
        if (tentry->streamId == _streamID && tentry->vlan_id == _vlanID) {
            SRPTable *signal_srpTable = check_and_cast<SRPTable *>(src);

            signal_srpTable->updateListenerWithStreamId(_streamID, this,
                    _vlanID);
            _isConnected = true;

            EV_INFO << _endpointPath << ": Registered AVBListener for streamID "
                           << _streamID << endl;
            if (_updateInterval > 0) {
                scheduleUpdateMessage(simTime() + _updateInterval);
            }
        }
    } else if (id == NF_AVB_LISTENER_REGISTRATION_TIMEOUT) {
        _isConnected = false;
        SRPTable::ListenerEntry *lentry = check_and_cast<
                SRPTable::ListenerEntry*>(obj);
        if (lentry->streamId == _streamID && lentry->vlan_id == _vlanID) {
            if (lentry->module == this) {

                EV_ERROR << _endpointPath
                                << ": Registration failed for AVBListener streamID "
                                << _streamID << endl;
                if (_retryInterval > 0) {
                    scheduleUpdateMessage(simTime() + _retryInterval);
                }
            }
        }
    }
}

void AVBSubscriberEndpoint::handleMessage(cMessage *msg) {
    if (msg->isSelfMessage()
            && (strcmp(msg->getName(), "updateSubscription") == 0)) {
        // update the subscription!
        if (_srpTable->getTalkerEntryForStreamId(_streamID, _vlanID)) {
            _srpTable->updateListenerWithStreamId(_streamID, this, _vlanID);
            _isConnected = true;
        } else {
            _isConnected = false;
        }
        if (_updateInterval > 0) {
            scheduleUpdateMessage(simTime() + _updateInterval);
        }
    } else {
        RTSubscriberEndpointBase::handleMessage(msg);
    }
}

void AVBSubscriberEndpoint::processScheduledMessage(cMessage* msg) {
    if (msg->arrivedOn("AVBin")) {
        if (AVBFrame* frame = dynamic_cast<AVBFrame*>(msg)) {
            handleTransportIn(frame->decapsulate());
        }
        delete msg;
    } else {
        RTSubscriberEndpointBase::handleMessage(msg);
    }
}

void AVBSubscriberEndpoint::handleParameterChange(const char* parname) {
    RTSubscriberEndpointBase::handleParameterChange(parname);

    if (!parname || !strcmp(parname, "updateInterval")) {
        this->_updateInterval = parameterDoubleCheckRange(par("updateInterval"),
                0, DBL_MAX);
    }
    if (!parname || !strcmp(parname, "retryInterval")) {
        this->_retryInterval = parameterDoubleCheckRange(par("retryInterval"),
                0, DBL_MAX);
    }
    if (!parname || !strcmp(parname, "streamID")) {
        this->_streamID = parameterULongCheckRange(par("streamID"), 0,
                static_cast<unsigned long>(MAX_STREAM_ID));
    }
    if (!parname || !strcmp(parname, "vlan_id")) {
        this->_vlanID = static_cast<unsigned short>(parameterULongCheckRange(
                par("vlan_id"), 0, MAX_VLAN_ID));
    }
}

void AVBSubscriberEndpoint::scheduleUpdateMessage(SimTime at) {
    // create message if it doesnt exist
    if (_updateMessage) {
        // cancel previously scheduled updates
        if (_updateMessage->isScheduled()) {
            cancelEvent(_updateMessage);
        }
    } else {
        _updateMessage = new cMessage("updateSubscription");
    }
    // schedule the message
    scheduleAt(at, _updateMessage);
}

} /*end namespace SOA4CoRE*/
