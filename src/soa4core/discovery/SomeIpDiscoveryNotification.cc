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

#include <soa4core/discovery/SomeIpDiscoveryNotification.h>
//STD
#include <algorithm>

using namespace inet;

namespace SOA4CoRE {

SomeIpDiscoveryNotification::SomeIpDiscoveryNotification(int serviceId, L3Address address, uint16_t instanceId,
                                                         std::set<QoSGroup> qosGroups, QoSGroup qosGroup, int tcpPort, int udpPort,
                                                         L3Address mcastDestAddr, int mcastDestPort) :
                                DiscoveryNotification(serviceId, address), _instanceId(instanceId), _qosGroups(qosGroups), _qosGroup(qosGroup),
                                _tcpPort(tcpPort), _udpPort(udpPort), _mcastDestAddr(mcastDestAddr), _mcastDestPort(mcastDestPort) {
}

SomeIpDiscoveryNotification::SomeIpDiscoveryNotification(int serviceId, inet::L3Address address, uint16_t instanceId,
                                                         std::set<QoSGroup> qosGroups, QoSGroup qosGroup, int tcpPort, int udpPort,
                                                         L3Address mcastDestAddr, int mcastDestPort,
                                                         size_t framesizeMax, double intervalMin, int vlan_id, int pcp, double deadline, bool preventReservation) :
                                DiscoveryNotification(serviceId, address), _instanceId(instanceId), _qosGroups(qosGroups), _qosGroup(qosGroup),
                                _tcpPort(tcpPort), _udpPort(udpPort), _mcastDestAddr(mcastDestAddr), _mcastDestPort(mcastDestPort),
                                _payloadMax(framesizeMax), _intervalMin(intervalMin), _vlan_id(vlan_id), _pcp(pcp), _deadline(deadline), _preventReservation(preventReservation) {
}

SomeIpDiscoveryNotification::SomeIpDiscoveryNotification() :
                                DiscoveryNotification(), _instanceId(0xFFFF), _qosGroup(QoSGroup::UNDEFINED),
                                _tcpPort(-1), _udpPort(-1), _mcastDestPort(-1){
}

SomeIpDiscoveryNotification::~SomeIpDiscoveryNotification() {
}

SomeIpDiscoveryNotification::SomeIpDiscoveryNotification(const SomeIpDiscoveryNotification& someIpDiscoveryNotification) :
    DiscoveryNotification(static_cast<DiscoveryNotification>(someIpDiscoveryNotification)), _instanceId(someIpDiscoveryNotification._instanceId),
    _qosGroups(someIpDiscoveryNotification._qosGroups), _qosGroup(someIpDiscoveryNotification._qosGroup), _tcpPort(someIpDiscoveryNotification._tcpPort),
    _udpPort(someIpDiscoveryNotification._udpPort), _mcastDestAddr(someIpDiscoveryNotification._mcastDestAddr), _mcastDestPort(someIpDiscoveryNotification._mcastDestPort),
    _payloadMax(someIpDiscoveryNotification._payloadMax), _intervalMin(someIpDiscoveryNotification._intervalMin), _vlan_id(someIpDiscoveryNotification._vlan_id),
    _pcp(someIpDiscoveryNotification._pcp), _deadline(someIpDiscoveryNotification._deadline), _preventReservation(someIpDiscoveryNotification._preventReservation){
}

SomeIpDiscoveryNotification& SomeIpDiscoveryNotification::operator=(const SomeIpDiscoveryNotification& other) {
    _serviceId = other._serviceId;
    _address = other._address;
    _instanceId = other._instanceId;
    _qosGroups = other._qosGroups;
    _qosGroup = other._qosGroup;
    _tcpPort = other._tcpPort;
    _udpPort = other._udpPort;
    _mcastDestAddr = other._mcastDestAddr;
    _mcastDestPort = other._mcastDestPort;
    _payloadMax = other._payloadMax;
    _intervalMin = other._intervalMin;
    _vlan_id = other._vlan_id;
    _pcp = other._pcp;
    _deadline = other._deadline;
    _preventReservation = other._preventReservation;
    return *this;
}

SomeIpDiscoveryNotification* SomeIpDiscoveryNotification::dup() const {
    return new SomeIpDiscoveryNotification(*this);
}

uint16_t SomeIpDiscoveryNotification::getInstanceId() const {
    return _instanceId;
}

QoSGroup SomeIpDiscoveryNotification::getQoSGroup() const {
    return _qosGroup;
}

std::set<QoSGroup> SomeIpDiscoveryNotification::getQoSGroups() const {
    return _qosGroups;
}

int SomeIpDiscoveryNotification::getTcpPort() const {
    return _tcpPort;
}

int SomeIpDiscoveryNotification::getUdpPort() const {
    return _udpPort;
}

int SomeIpDiscoveryNotification::getMcastDestPort() const {
    return _mcastDestPort;
}

L3Address SomeIpDiscoveryNotification::getMcastDestAddr() const {
    return _mcastDestAddr;
}

void SomeIpDiscoveryNotification::setInstanceId(uint16_t instanceId) {
    _instanceId = instanceId;
}

void SomeIpDiscoveryNotification::setMcastDestAddr(
        const L3Address& mcastDestAddr) {
    _mcastDestAddr = mcastDestAddr;
}

void SomeIpDiscoveryNotification::setMcastDestPort(
        int mcastDestPort) {
    _mcastDestPort = mcastDestPort;
}

void SomeIpDiscoveryNotification::setQosGroup(QoSGroup qosGroup) {
    _qosGroup = qosGroup;
}

void SomeIpDiscoveryNotification::setTcpPort(int tcpPort) {
    _tcpPort = tcpPort;
}

void SomeIpDiscoveryNotification::setUdpPort(int udpPort) {
    _udpPort = udpPort;
}

bool SomeIpDiscoveryNotification::addQoSGroup(QoSGroup qosGroup) {
    if(std::find(_qosGroups.begin(), _qosGroups.end(), qosGroup) == _qosGroups.end()) {
        _qosGroups.insert(qosGroup);
        return true;
    }
    return false;
}

double SomeIpDiscoveryNotification::getDeadline() const {
    return _deadline;
}

void SomeIpDiscoveryNotification::setDeadline(double deadline) {
    _deadline = deadline;
}

size_t SomeIpDiscoveryNotification::getPayloadMax() const {
    return _payloadMax;
}

void SomeIpDiscoveryNotification::setFramesizeMax(
        size_t framesizeMax) {
    _payloadMax = framesizeMax;
}

double SomeIpDiscoveryNotification::getIntervalMin() const {
    return _intervalMin;
}

void SomeIpDiscoveryNotification::setIntervalMin(double intervalMin) {
    _intervalMin = intervalMin;
}

int SomeIpDiscoveryNotification::getPcp() const {
    return _pcp;
}

void SomeIpDiscoveryNotification::setPcp(int pcp) {
    _pcp = pcp;
}

int SomeIpDiscoveryNotification::getVlanId() const {
    return _vlan_id;
}

void SomeIpDiscoveryNotification::setVlanId(int vlanId) {
    _vlan_id = vlanId;
}

bool SomeIpDiscoveryNotification::getPreventReservation() const {
    return _preventReservation;
}
void SomeIpDiscoveryNotification::setPreventReservation(bool preventReservation) {
    _preventReservation = preventReservation;
}

bool SomeIpDiscoveryNotification::updateFromEndpointOption(
        SomeIpSDOption* option) {
    bool changed = false;
    IPv4EndpointOption* ipv4EndpointOption = dynamic_cast<IPv4EndpointOption*>(option);
    if(!ipv4EndpointOption) {
        throw cRuntimeError("SomeIpSDOption is not type of IPv4EndpointOption");
    }
    bool isMcast = false;
    if(dynamic_cast<IPv4MulticastOption*>(option)) {
        isMcast = true;
    } else {
        if (_address.isUnspecified() || _address != ipv4EndpointOption->getIpv4Address()){
            changed = true;
            this->setAddress(ipv4EndpointOption->getIpv4Address());
        }
    }
    switch(ipv4EndpointOption->getL4Protocol()) {
        case IPProtocolId::IP_PROT_UDP:
            if(isMcast) {
                if(!this->addQoSGroup(QoSGroup::SOMEIP_UDP_MCAST)){
                    throw cRuntimeError("Already parsed another UDP Mcast Option for this offer");
                }
                changed = true;
                this->setMcastDestAddr(ipv4EndpointOption->getIpv4Address());
                this->setMcastDestPort(ipv4EndpointOption->getPort());
            } else {
                if(!this->addQoSGroup(QoSGroup::SOMEIP_UDP)){
                    throw cRuntimeError("Already parsed another UDP Option for this offer");
                }
                changed = true;
                this->setUdpPort(ipv4EndpointOption->getPort());
            }
            break;
        case IPProtocolId::IP_PROT_TCP:
            if(!this->addQoSGroup(QoSGroup::SOMEIP_TCP)){
                throw cRuntimeError("Already parsed another TCP Option for this offer");
            }
            changed = true;
            this->setUdpPort(ipv4EndpointOption->getPort());
            this->setTcpPort(ipv4EndpointOption->getPort());
            break;
        default:
            throw cRuntimeError("Unknown L4 Protocol.");
    }
    return changed;
}

bool SomeIpDiscoveryNotification::operator==(const SomeIpDiscoveryNotification& someIpDiscoveryNotification) const {
    return static_cast<DiscoveryNotification>(*this) == static_cast<DiscoveryNotification>(someIpDiscoveryNotification)
            && _instanceId == someIpDiscoveryNotification._instanceId
            && (_qosGroups == someIpDiscoveryNotification._qosGroups || _qosGroup == someIpDiscoveryNotification._qosGroup)
            && _tcpPort == someIpDiscoveryNotification._tcpPort
            && _udpPort == someIpDiscoveryNotification._udpPort
            && _mcastDestAddr == someIpDiscoveryNotification._mcastDestAddr
            && _mcastDestPort == someIpDiscoveryNotification._mcastDestPort;
}

bool SomeIpDiscoveryNotification::operator!=(const SomeIpDiscoveryNotification& someIpDiscoveryNotification) const {
    return !(*this == someIpDiscoveryNotification);
}

} /* end namespace SOA4CoRE */

