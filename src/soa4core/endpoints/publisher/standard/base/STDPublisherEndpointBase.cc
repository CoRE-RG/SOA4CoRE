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

#include "STDPublisherEndpointBase.h"

#include "core4inet/utilities/ConfigFunctions.h"
#include "core4inet/utilities/ModuleAccess.h"
#include "core4inet/buffer/base/BGBuffer.h"

using namespace std;
using namespace inet;
using namespace CoRE4INET;

namespace SOA4CoRE {

bool STDPublisherEndpointBase::has8021QInformation() {
    return _pcp < 0 || _vlanID < 0;
}

void STDPublisherEndpointBase::initialize()
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

void STDPublisherEndpointBase::handleParameterChange(const char* parname)
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
}

void STDPublisherEndpointBase::createAndInstallFilter(inet::IPv4Address destAddr, int srcPort, int destPort)
{
    MACAddress macAddress = resolveDestMacAddress(destAddr);
    TrafficPattern* tp = createTrafficPattern(destAddr, srcPort, destPort);
    IEEE8021QDestinationInfo *destInfo = createDestinationInfo(macAddress);
    IPoREFilter *filter = new IPoREFilter();
    filter->setDestInfo(destInfo);
    filter->setTrafficPattern(tp);
    installFilter(filter);
}

void STDPublisherEndpointBase::installFilter(IPoREFilter* filter)
{
    _networkLayer->addFilter(filter);
}

TrafficPattern* STDPublisherEndpointBase::createTrafficPattern(
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

IEEE8021QDestinationInfo* STDPublisherEndpointBase::createDestinationInfo(MACAddress destMAC)
{
    list<cGate*> destGates;
    // use default gate bg out as in IPv4oIEEE8021Q
    cModule* module = findModuleWhereverInNode(_defaultDestGate.c_str(), this);
    if (!module)
    {
        throw cRuntimeError("Default destModule \"%s\" could not be resolved.", _defaultDestGate.c_str());
    }
    if (BGBuffer *bgBuf = dynamic_cast<BGBuffer*>(module))
    {
        destGates.push_back(bgBuf->gate("in"));
    } else
    {
        throw cRuntimeError("destModule: %s is not a BGBuffer and destGate could not be resolved.", _defaultDestGate.c_str());
    }
    return createDestinationInfo(_vlanID, _pcp, destMAC, destGates);
}

IEEE8021QDestinationInfo* STDPublisherEndpointBase::createDestinationInfo(int vid, int pcp, MACAddress destMAC,
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

MACAddress STDPublisherEndpointBase::resolveDestMacAddress(inet::IPv4Address destAddress)
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

