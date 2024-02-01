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

#include "soa4core/applications/base/ServiceBase.h"
#include "soa4core/connector/publisher/PublisherConnector.h"
#include "AVBPublisherEndpoint.h"
//STD
#include <string>
//INET
#include <inet/linklayer/ethernet/Ethernet.h>
//CoRE4INET
#include <core4inet/base/CoRE4INET_Defs.h>
#include <core4inet/utilities/ConfigFunctions.h>
#include <core4inet/base/NotifierConsts.h>
//AUTO-GENERATED MESSAGES
#include <core4inet/linklayer/ethernet/avb/AVBFrame_m.h>

using namespace std;
using namespace CoRE4INET;
using namespace inet;

namespace SOA4CoRE {

Define_Module(AVBPublisherEndpoint);

void AVBPublisherEndpoint::initialize() {
    // get owning app
    ServiceBase* app = _publisherConnector->getApplication();
    if(!app){
        throw cRuntimeError("Owning application not found in init of publisher.");
    }

    //find srp table
    _srpTable = check_and_cast<SRPTable *>(
            app->getParentModule()->getSubmodule("srpTable"));
    if(!_srpTable){
        throw cRuntimeError("srpTable module required for stream reservation but not found");
    }

    _avbOutCTC = app->getParentModule()->getSubmodule("avbCTC");
    if(!_avbOutCTC){
        throw cRuntimeError("avbOutCTC module required for stream sending but not found");
    }

    _multicastMAC = inet::MACAddress::generateAutoAddress();
    _multicastMAC.setAddressByte(0, (_multicastMAC.getAddressByte(1) | 0x01));

    RTPublisherEndpointBase::initialize();
}

void AVBPublisherEndpoint::handleParameterChange(const char* parname) {
    RTPublisherEndpointBase::handleParameterChange(parname);

    if (!parname || !strcmp(parname, "srClass"))
    {
        if (strcmp(par("srClass").stringValue(), "A") == 0)
        {
            this->_srClass = SR_CLASS::A;
            if(par("pcp").intValue() == -1){
                this->_pcp = PCP_DEFAULT_SRCLASSA;
            }
        }
        else if (strcmp(par("srClass").stringValue(), "B") == 0)
        {
            this->_srClass = SR_CLASS::B;
            if(par("pcp").intValue() == -1){
                this->_pcp = PCP_DEFAULT_SRCLASSB;
            }
        }
        else
        {
            throw cRuntimeError("Parameter srClass of %s is %s and is only allowed to be A or B", getFullPath().c_str(),
                    par("srClass").stringValue());
        }
    }
    if (!parname || !strcmp(parname, "streamID"))
    {
        this->_streamID = parameterULongCheckRange(par("streamID"), 0, static_cast<unsigned long>(MAX_STREAM_ID));
    }
    if (!parname || !strcmp(parname, "intervalFrames"))
    {
        this->_intervalFrames = static_cast<uint16_t>(parameterULongCheckRange(par("intervalFrames"), 1,
        MAX_INTERVAL_FRAMES));
    }
    if (!parname || !strcmp(parname, "vlan_id"))
    {
        this->_vlanID = static_cast<uint16_t>(parameterULongCheckRange(par("vlan_id"), 0, MAX_VLAN_ID));
    }
    if (!parname || !strcmp(parname, "pcp"))
    {
        if(par("pcp").intValue() != -1){
            this->_pcp = static_cast<uint8_t>(parameterULongCheckRange(par("pcp"), 0, MAX_Q_PRIORITY));
        }
    }
    if (!parname || !strcmp(parname, "payload"))
    {
        auto payload = parameterULongCheckRange(par("payload"), 0,
                MAX_ETHERNET_DATA_BYTES);

        if (payload <= (MIN_ETHERNET_FRAME_BYTES - ETHER_MAC_FRAME_BYTES - ETHER_8021Q_TAG_BYTES))
        {
            _frameSize = MIN_ETHERNET_FRAME_BYTES;
        }
        else
        {
            _frameSize = payload + ETHER_MAC_FRAME_BYTES + ETHER_8021Q_TAG_BYTES;
        }
    }
}

void AVBPublisherEndpoint::receiveSignal(__attribute__((unused))   cComponent *src,
        simsignal_t id, cObject *obj, __attribute__((unused))   cObject *details) {
    auto numListeners = _srpTable->getListenersForStreamId(_streamID, _vlanID).size();
    if (id == NF_AVB_LISTENER_REGISTERED || id == NF_AVB_LISTENER_UPDATED)
    {
        SRPTable::ListenerEntry *lentry = dynamic_cast<SRPTable::ListenerEntry*>(obj);
        //If talker for the desired stream, register Listener
        if (lentry && lentry->streamId == _streamID && lentry->vlan_id == _vlanID)
        {
            EV_INFO << _endpointPath << ": Listener for stream " << lentry->streamId << " registered!" << std::endl;
            emit(_remotesSignal, numListeners);
            _isConnected = true;
        }
    }
    else if (id == NF_AVB_LISTENER_REGISTRATION_TIMEOUT || id == NF_AVB_LISTENER_UNREGISTERED)
    {
        SRPTable::ListenerEntry *lentry = dynamic_cast<SRPTable::ListenerEntry*>(obj);
        //If talker for the desired stream, unregister Listener
        if (lentry && lentry->streamId == _streamID && lentry->vlan_id == _vlanID)
        {
            emit(_remotesSignal, numListeners);
            EV_INFO << _endpointPath << ": Listener Removed at stream " << _streamID << endl;
            //check whether there are listeners left
            if ( numListeners == 0)
            {
                EV_DEBUG << _endpointPath << ": No more Listeners Registered for " << _streamID << "!" << std::endl;
                _isConnected = false;
            }
        }
    }
}

void AVBPublisherEndpoint::initializeTransportConnection() {
    _isConnected = false;
    EV_INFO << "Register Talker in node" << std::endl;
    _srpTable->subscribe(NF_AVB_LISTENER_UPDATED, this);
    _srpTable->subscribe(NF_AVB_LISTENER_REGISTERED, this);
    _srpTable->subscribe(NF_AVB_LISTENER_UNREGISTERED, this);
    _srpTable->subscribe(NF_AVB_LISTENER_REGISTRATION_TIMEOUT, this);
    _srpTable->updateTalkerWithStreamId(_streamID, this,
            _multicastMAC, _srClass, _frameSize, _intervalFrames, _vlanID, _pcp);
    EV_INFO << _endpointPath << ": Registered AVBTalker with streamID " << _streamID << endl;
}

ConnectionSpecificInformation* AVBPublisherEndpoint::getConnectionSpecificInformation() {
    CSI_AVB* connection = new CSI_AVB();
    connection->setStreamID(_streamID);
    connection->setVlanID(_vlanID);

    return connection;
}

void AVBPublisherEndpoint::publish(cPacket* msg) {
    if (_isConnected) {
        std::string name = "Stream ";
        name += std::to_string(_streamID);
        AVBFrame *outFrame = new AVBFrame(name.c_str());
        outFrame->setTimestamp();
        outFrame->setStreamID(_streamID);
        outFrame->setDest(_multicastMAC);
        outFrame->setVID(_vlanID);
        outFrame->setPcp(_pcp);

        outFrame->encapsulate(msg->dup());
        //Padding
        if (outFrame->getByteLength() < MIN_ETHERNET_FRAME_BYTES) {
            outFrame->setByteLength(MIN_ETHERNET_FRAME_BYTES);
        }
        sendDirect(outFrame, _avbOutCTC->gate("in"));
    }
}

} /*end namespace SOA4CoRE*/
