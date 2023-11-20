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
#include "core4inet/base/avb/AVBDefs.h"

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

/**
 * Normalizes the framesize for the class measurement interval.
 * Calculates the interval to CMI ratio and devides the framesize by it.
 * Also corrects for multiples of l1 header, IFG and SRP that would be added
 * in SRTable bandwidth calculation.
 * @param l2Framesize the layer 2 framesize
 * @param intervalMin the minimum interval of the stream
 * @param srclass the SR class to normalize for
 * @param errorOnNegativeResult true, if we should throw an error for a negative framesize result
 * @return the normalized framesize for the CMI that is ceiled to an int,
 *         CAUTION: can be negative for very large interval and small frames
 */
int normalizeFramesizeForCMI(uint16_t l2Framesize, double intervalMin, CoRE4INET::SR_CLASS srclass, bool errorOnNegativeResult = true);

}

#endif /* SOA4CORE_UTILITY_COMFORTFUNCTIONS_H_ */
