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

namespace SOA4CoRE {

SomeIpDiscoveryNotification::SomeIpDiscoveryNotification(int serviceId, inet::L3Address address, uint16_t instanceId,
                                                         std::set<QoSGroup> qosGroups, QoSGroup qosGroup, int tcpPort, int udpPort,
                                                         inet::L3Address mcastDestAddr, int _mcastDestPort) :
                             DiscoveryNotification(serviceId, address), _instanceId(instanceId), _qosGroups(qosGroups), _qosGroup(qosGroup),
                             _tcpPort(tcpPort), _udpPort(udpPort), _mcastDestAddr(mcastDestAddr), _mcastDestPort(_mcastDestPort) {
}

SomeIpDiscoveryNotification::~SomeIpDiscoveryNotification() {
}

SomeIpDiscoveryNotification::SomeIpDiscoveryNotification(const SomeIpDiscoveryNotification& someIpDiscoveryNotification) :
    DiscoveryNotification(static_cast<DiscoveryNotification>(someIpDiscoveryNotification)), _instanceId(someIpDiscoveryNotification._instanceId),
    _qosGroups(someIpDiscoveryNotification._qosGroups), _qosGroup(someIpDiscoveryNotification._qosGroup), _tcpPort(someIpDiscoveryNotification._tcpPort),
    _udpPort(someIpDiscoveryNotification._udpPort){
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

inet::L3Address SomeIpDiscoveryNotification::getMcastDestAddr() const {
    return _mcastDestAddr;
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

