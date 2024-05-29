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
#include "soa4core/utility/comfortFunctions.h"

#include "core4inet/utilities/ConfigFunctions.h"
#include "core4inet/utilities/HelperFunctions.h"
#include "core4inet/utilities/ModuleAccess.h"
#include "core4inet/buffer/base/BGBuffer.h"
#include "core4inet/services/avb/SRP/SRPTable.h"
#include "inet/linklayer/ethernet/Ethernet.h"
#include "inet/networklayer/ipv4/IPv4Datagram_m.h"
#include <core4inet/base/CoRE4INET_Defs.h>

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
    if (!parname || !strcmp(parname, "streamIntervalAsCMI"))
    {
        _streamIntervalAsCMI = this->par("streamIntervalAsCMI");
    }
    if (!parname || !strcmp(parname, "pcpCMI"))
    {
        if (!_streamIntervalAsCMI)
        {
            cValueMap* pcpCMI = dynamic_cast<cValueMap*>(this->par("pcpCMI").objectValue());
            if(!pcpCMI) {
                throw cRuntimeError("CMIs for PCPs have not been specified.");
            }
            for (auto it : pcpCMI->getFields())
            {
                int pcp = atoi(it.first.c_str());
                double cmi = it.second.doubleValueInUnit("s");
                if (cmi > 0)
                {
                    pcpCMIs[pcp] = cmi;
                }
            }
        }
    }
}

bool IPPublisherEndpointBase::requiresReservation()
{
    Publisher* app = dynamic_cast<Publisher*>(_publisherConnector->getApplication());
    return has8021QInformation()
            && _registerStream && !app->getPreventReservation()
            && (_streamIntervalAsCMI || (pcpCMIs.find(_pcp) != pcpCMIs.end()));
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
    // -- not unique if multiple instances of a service exist and are subscribed by the same unicast destination
    // uint64_t streamId = buildStreamIDForService(serviceId, mac_dest)
    uint64_t streamId = createStreamId(destAddress);
    int numFrames = 1;
    size_t payload = app->getPayloadMax();
    size_t maxPayload = getMaxPhysicalFrameSize() - getHeaderBytes();
    size_t avgPayload = payload;
    if(payload > maxPayload)
    {
        numFrames = ceil(payload / (maxPayload));
        avgPayload = payload / numFrames;
        // TODO add parameter to control whether avg or max payload should be used
        payload = maxPayload;
    }
    int fullL2FrameSize = calculateL2Framesize(avgPayload);
    double interval = -1;
    SR_CLASS srclass = SR_CLASS::A;
    int normalizedFramesize = fullL2FrameSize;
    if (_streamIntervalAsCMI)
    {
        // TODO check whether we should use the avg payload and interval or the max payload and min interval
        // interval = (app->getIntervalMin() + app->getIntervalMax()) / 2.0;
        interval = app->getIntervalMin();
    }
    else {
        auto cmiIt = pcpCMIs.find(_pcp);
        if(cmiIt == pcpCMIs.end())
        {
            throw cRuntimeError("CMI for PCP %d unknown", app->getPcp());
        }
        normalizedFramesize = normalizeFramesizeForCMI(fullL2FrameSize, cmiIt->second, srclass, false);
        if(normalizedFramesize < 0) {
            srclass = SR_CLASS::B;
            normalizedFramesize = normalizeFramesizeForCMI(fullL2FrameSize, cmiIt->second, srclass, true);
        }
    }
    srpTable->updateTalkerWithStreamId( streamId, this, macAddress,
                                        srclass, (uint16_t) normalizedFramesize, numFrames, _vlanID,
                                        _pcp, !_advertiseStreamRegistration, interval);
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

uint16_t IPPublisherEndpointBase::calculateL2Framesize(uint16_t payload) 
{
    return getHeaderBytes() + payload;
}

size_t IPPublisherEndpointBase::getHeaderBytes() 
{
    uint16_t addQTagBytes = ETHER_8021Q_TAG_BYTES;
    if(!has8021QInformation()) {
        addQTagBytes = 0;
    }
    return ETHER_MAC_FRAME_BYTES + addQTagBytes + IP_HEADER_BYTES;
}

size_t IPPublisherEndpointBase::getMaxPhysicalFrameSize() 
{
    if(has8021QInformation()) {
        return MAX_ETHERNET_FRAME_BYTES;
    }
    return MAX_ETHERNET_FRAME_BYTES - ETHER_8021Q_TAG_BYTES;
}

uint64_t IPPublisherEndpointBase::createStreamId(
        inet::IPv4Address destAddress) {
    Publisher* app = dynamic_cast<Publisher*>(_publisherConnector->getApplication());
    if(!app) {
        throw cRuntimeError("Publisher could not be resolved.");
    }
    return app->getStreamId();
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
