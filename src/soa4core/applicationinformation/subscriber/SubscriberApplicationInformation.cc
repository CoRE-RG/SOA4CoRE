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

#include "soa4core/applicationinformation/subscriber/SubscriberApplicationInformation.h"

namespace SOA4CoRE {

SubscriberApplicationInformation::SubscriberApplicationInformation() : ApplicationInformation::ApplicationInformation() {

}

SubscriberApplicationInformation::SubscriberApplicationInformation(int serviceId, inet::L3Address address, uint16_t instanceId,
        QoSGroup qosGroup, int tcpPort, int udpPort, int streamId, CoRE4INET::SR_CLASS srClass,
        size_t framesize, uint16_t intervalFrames) : ApplicationInformation(serviceId, address, instanceId, tcpPort, udpPort,
                streamId, srClass, framesize, intervalFrames), _qosGroup(qosGroup){
}

SubscriberApplicationInformation::~SubscriberApplicationInformation() {

}

QoSGroup SubscriberApplicationInformation::getQoSGroup() const{
    return _qosGroup;
}

bool SubscriberApplicationInformation::operator==(const SubscriberApplicationInformation& subscriberApplicationInformation) const {
    return static_cast<ApplicationInformation>(*this) == static_cast<ApplicationInformation>(subscriberApplicationInformation)
            && _qosGroup == subscriberApplicationInformation.getQoSGroup();
}

bool SubscriberApplicationInformation::operator!=(const SubscriberApplicationInformation& subscriberApplicationInformation) const {
    return !(*this == subscriberApplicationInformation);
}

} /* end namespace SOA4CoRE */
