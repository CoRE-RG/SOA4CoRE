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

#ifndef SOA4CORE_UTILITY_COMFORTFUNCTIONS_H_
#define SOA4CORE_UTILITY_COMFORTFUNCTIONS_H_

#include "inet/linklayer/common/MACAddress.h"
#include "inet/networklayer/contract/ipv4/IPv4Address.h"

namespace SOA4CoRE {


/**
 * We need a unique stream ID per flow for stream reservation which is not trivial.
 * Using the service id is not enough when one service has multiple !UNICAST! destination nodes.
 * Using the destination MAC is not enough when multiple services have the same destination addresses.
 * The combination of the two, however, is unique
 * (except when there are multiple instances of a service @see buildStreamIDFrom(uint16_t serviceId, inet::MACAddress macAddress))
 * @param serviceId
 * @param macAddress
 * @return a 64 bit value with 2 highest bytes occupied by serviceId and the lower 6 by the MAC address
 */
uint64_t buildStreamIDForService(uint16_t serviceId, inet::MACAddress macAddress);

/**
 * We need a unique stream ID per flow for stream reservation which is not trivial.
 * Service ID and MACAddress are not unique if multiple instances of a service exist and are subscribed by the same destination.
 * Using the destination IP is shorter and contains the same information as the destination MAC in the case of SOME/IP services.
 * Combining instance and service ID uniquely identifies the service endpoint.
 * @param serviceId
 * @param instanceId
 * @param ipv4Address
 * @return concatinate uint64 with (highest to lowest byte) 2 bytes serviceId, 2 bytes instanceId and 4 bytes IP
 */
uint64_t buildStreamIDForService(uint16_t serviceId, uint16_t instanceId, inet::IPv4Address ipv4Address);

}

#endif /* SOA4CORE_UTILITY_COMFORTFUNCTIONS_H_ */
