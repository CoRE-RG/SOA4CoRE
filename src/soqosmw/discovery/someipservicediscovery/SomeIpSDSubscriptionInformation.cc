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

#include <soqosmw/discovery/someipservicediscovery/SomeIpSDSubscriptionInformation.h>

namespace SOQoSMW {

SomeIpSDSubscriptionInformation::SomeIpSDSubscriptionInformation(uint16_t serviceId, uint16_t instanceId, inet::L3Address localAddress, uint16_t localPort, inet::L3Address remoteAddress) :
_serviceId(serviceId), _instanceId(instanceId), _localAddress(localAddress), _localPort(localPort), _remoteAddress(remoteAddress){

}

SomeIpSDSubscriptionInformation::~SomeIpSDSubscriptionInformation() {
}

uint16_t SomeIpSDSubscriptionInformation::getInstanceId() const {
    return _instanceId;
}

const inet::L3Address& SomeIpSDSubscriptionInformation::getLocalAddress() const {
    return _localAddress;
}

uint16_t SomeIpSDSubscriptionInformation::getLocalPort() const {
    return _localPort;
}

const inet::L3Address& SomeIpSDSubscriptionInformation::getRemoteAddress() const {
    return _remoteAddress;
}

uint16_t SomeIpSDSubscriptionInformation::getServiceId() const {
    return _serviceId;
}

} /* end namespace SOQoSMW */
