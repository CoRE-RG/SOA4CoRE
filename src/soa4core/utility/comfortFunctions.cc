
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

#include "soa4core/utility/comfortFunctions.h"
#include "inet/linklayer/ethernet/Ethernet.h"
#include "core4inet/utilities/HelperFunctions.h"

using namespace inet;
using namespace CoRE4INET;

namespace SOA4CoRE {

uint64_t buildStreamIDForService(uint16_t serviceId,
        inet::MACAddress macAddress) {
    uint64_t macDestAsInt = macAddress.getInt(); // from INET::MACAddress: 6*8=48 bit address, lowest 6 bytes are used, highest 2 bytes are always zero
    uint64_t shiftedServiceId = ((uint64_t) serviceId) << 48;// shift to occupy the two highest 2 bytes
    return macDestAsInt + shiftedServiceId;
}

uint64_t buildStreamIDForService(uint16_t serviceId, uint16_t instanceId,
        inet::IPv4Address iPv4Address) {
    uint64_t ipDestInt = iPv4Address.getInt(); //4*8=32 bit address, lowest 4 bytes will be used
    uint64_t shiftedInstanceId = ((uint64_t) instanceId) << 32;// shift to occupy the two highest bytes 3,4
    uint64_t shiftedServiceId = ((uint64_t) serviceId) << 48;// shift to occupy the two highest 2 bytes
    return ipDestInt + shiftedInstanceId + shiftedServiceId;
}

int normalizeFramesizeForCMI(uint16_t l2Framesize, double intervalMin, SR_CLASS srclass, bool errorOnNegativeResult) {
    double intervalCMIRatio = intervalMin / getIntervalForClass(srclass);
    double correctionForReservation = (intervalCMIRatio-1) * (PREAMBLE_BYTES + SFD_BYTES + INTERFRAME_GAP_BITS/8 + SRP_SAFETYBYTE);
    int framesize = ceil((l2Framesize - correctionForReservation) / intervalCMIRatio);
    if(errorOnNegativeResult && framesize < 0) {
        throw cRuntimeError("Normalized framesize for CMI is negative and cannot be reserved.");
    }
    return framesize;
}

} // end namespace SOA4CoRE
