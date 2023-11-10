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

#include <soa4core/endpoints/publisher/ip/base/IPPublisherEndpointBase.h>
#include "soa4core/applications/publisher/base/Publisher.h"
#include "soa4core/connector/publisher/PublisherConnector.h"

#include "core4inet/utilities/ConfigFunctions.h"
#include "core4inet/utilities/HelperFunctions.h"
#include "core4inet/utilities/ModuleAccess.h"
#include "core4inet/buffer/base/BGBuffer.h"
#include "core4inet/services/avb/SRP/SRPTable.h"

using namespace std;
using namespace inet;
using namespace CoRE4INET;

namespace SOA4CoRE {

bool IPPublisherEndpointBase::has8021QInformation() {
    return _pcp >= 0 && _vlanID >= 0;
}

void IPPublisherEndpointBase::initialize()
{
    PublisherEndpointBase::initialize();

    cModule* networkLayer = findModuleWhereverInNode("networkLayer", this);
    if(!networkLayer)
    {
        throw cRuntimeError("Network Layer could not be resolved during initialization.");
    }
    cModule* ipModule = networkLayer->getSubmodule("ip");
    if(!ipModule)
    {
        throw cRuntimeError("Ip module could not be resolved during initialization.");
    }
    if(!(this->_networkLayer = dynamic_cast<IPv4oREBase*>(ipModule)))
    {
        throw cRuntimeError("Ip module is not of type IPv4oREBase.");
    }
}

void IPPublisherEndpointBase::handleParameterChange(const char* parname)
{
    PublisherEndpointBase::handleParameterChange(parname);

    if (!parname || !strcmp(parname, "localAddress"))
    {
        _localAddress = par("localAddress").stdstringValue();
    }
    if (!parname || !strcmp(parname, "localPort"))
    {
        _localPort = par("localPort");
    }
    if (!parname || !strcmp(parname, "vlan_id"))
    {
        if(par("vlan_id").intValue() != -1)
        {
            this->_vlanID = static_cast<int>(parameterULongCheckRange(par("vlan_id"), 0, MAX_VLAN_ID));
        }
    }
    if (!parname || !strcmp(parname, "pcp"))
    {
        if(par("pcp").intValue() != -1)
        {
            this->_pcp = static_cast<int>(parameterULongCheckRange(par("pcp"), 0, MAX_Q_PRIORITY));
        }
    }
    if (!parname || !strcmp(parname, "localAddress"))
    {
        _localAddress = par("localAddress").stdstringValue();
    }
    if (!parname || !strcmp(parname, "registerStream"))
    {
        _registerStream = par("registerStream").boolValue();
    }
    if (!parname || !strcmp(parname, "advertiseStreamRegistration"))
    {
        _advertiseStreamRegistration = par("advertiseStreamRegistration").boolValue();
    }
}

void IPPublisherEndpointBase::registerTalker(IPv4Address& destAddress)
{
    //find srp table
    SRPTable* srpTable = dynamic_cast<SRPTable*>(findModuleWhereverInNode("srpTable", getParentModule()));
    if(!srpTable){
        throw cRuntimeError("srpTable module required for stream reservation but not found");
    }
    MACAddress macAddress = resolveDestMacAddress(destAddress);
    //calculate framesize used per class measurement interval.
    Publisher* app = dynamic_cast<Publisher*>(_publisherConnector->getApplication());
    if(!app) {
        throw cRuntimeError("Publisher could not be resolved.");
    }
    double interval_cmi_ratio = app->getIntervalMin() / getIntervalForClass(SR_CLASS::A);
    uint16_t frameSize = app->getPayloadMax() / interval_cmi_ratio; // TODO Update to get max frame size instead of payload size. Problem: we do not know all headers.
    uint64 streamId = app->getStreamId();
    srpTable->updateTalkerWithStreamId( streamId, this, macAddress, 
                                        SR_CLASS::A, frameSize, 1, _vlanID, 
                                        _pcp, !_advertiseStreamRegistration);
    if(srpTable->getListenersForStreamId(streamId, _vlanID).empty()){
        //add a listener
        cModule* listener = app->getParentModule()->getSubmodule("eth", 0);
        if(!listener) {
            listener = app->getParentModule()->getSubmodule("phy", 0);
            if(!listener) {
                throw cRuntimeError("Could not resolve phy port to register dummy listener module.");
            }
        }
        srpTable->updateListenerWithStreamId(streamId, listener, _vlanID, !_advertiseStreamRegistration);
    }
}

void IPPublisherEndpointBase::createAndInstallFilter(inet::IPv4Address destAddr, int srcPort, int destPort)
{
    MACAddress macAddress = resolveDestMacAddress(destAddr);
    TrafficPattern* tp = createTrafficPattern(destAddr, srcPort, destPort);
    IEEE8021QDestinationInfo *destInfo = createDestinationInfo(macAddress);
    IPoREFilter *filter = new IPoREFilter();
    filter->setDestInfo(destInfo);
    filter->setTrafficPattern(tp);
    installFilter(filter);
}

void IPPublisherEndpointBase::installFilter(IPoREFilter* filter)
{
    _networkLayer->addFilter(filter);
}

TrafficPattern* IPPublisherEndpointBase::createTrafficPattern(
        inet::IPv4Address destAddr, int srcPort, int destPort)
{
    TrafficPattern* tp = new TrafficPattern();
    tp->setDestAddr(destAddr);
    tp->setDestPrefixLength(32);
    tp->setSrcPortMin(srcPort);
    tp->setSrcPortMax(srcPort);
    tp->setDestPortMin(destPort);
    tp->setDestPortMax(destPort);
    return tp;
}

IEEE8021QDestinationInfo* IPPublisherEndpointBase::createDestinationInfo(MACAddress destMAC)
{
    list<cGate*> destGates;
    cModule* module;
    if(!_defaultDestGate.empty())
    {
        if(!(module = findModuleWhereverInNode(_defaultDestGate.c_str(), this))) {
            throw cRuntimeError("Default destModule \"%s\" set but could not be resolved.", _defaultDestGate.c_str());
        }
    } else if (!(module = findModuleWhereverInNode("bgOut[0]", this)))
    {
        if(!(module = findModuleWhereverInNode("bgOut", this)))
        {
            throw cRuntimeError("BG buffers could not be resolved.");
        }
    }
    if (module->hasGate("in"))
    {
        destGates.push_back(module->gate("in"));
        // todo add a gate parameter
    } else
    {
        throw cRuntimeError("destModule: %s does not have an in gate.", _defaultDestGate.c_str());
    }
    return createDestinationInfo(_vlanID, _pcp, destMAC, destGates);
}

IEEE8021QDestinationInfo* IPPublisherEndpointBase::createDestinationInfo(int vid, int pcp, MACAddress destMAC,
        std::list<cGate*>& destGates)
{
    IEEE8021QDestinationInfo *destInfo = new IEEE8021QDestinationInfo();
    destInfo->setDestType(DestinationType_8021Q);
    destInfo->setVID(static_cast<uint16_t>(vid));
    destInfo->setPCP(static_cast<uint8_t>(pcp));
    destInfo->setDestMac(new MACAddress(destMAC));
    destInfo->setDestGates(destGates);
    return destInfo;
}

MACAddress IPPublisherEndpointBase::resolveDestMacAddress(inet::IPv4Address destAddress)
{
    MACAddress destMac;
    if (destAddress.isLimitedBroadcastAddress())
    {
        destMac = MACAddress::BROADCAST_ADDRESS;
    }
    else if (destAddress.isMulticast())
    {
        destMac = MACAddress::makeMulticastAddress(destAddress);
    }
    else
    {
        if(_arp == nullptr) {
            if(!(_arp = dynamic_cast<IARP*>(findModuleWhereverInNode("arp", this)))) {
                throw cRuntimeError("MAC Address could not be resolved because arp module was'nt found.");
            }
        }
        destMac = _arp->resolveL3Address(destAddress, nullptr);
        if(destMac.isUnspecified()) {
            throw cRuntimeError("Could not resolve MAC address with ARP directly. RT IP stack does not support dynamic discovery yet, maybe try GlobalARP.");
        }
    }
    return destMac;
}

} /*end namespace SOA4CoRE*/

