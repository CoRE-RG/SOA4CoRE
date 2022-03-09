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

#include "soa4core/applicationinformation/publisher/PublisherApplicationInformation.h"
//STD
#include <algorithm>

namespace SOA4CoRE {

PublisherApplicationInformation::PublisherApplicationInformation() : ApplicationInformation::ApplicationInformation() {
}

PublisherApplicationInformation::PublisherApplicationInformation(int serviceId, inet::L3Address address, uint16_t instanceId,
        std::set<QoSGroup> qosGroups, int tcpPort, int udpPort, int streamId, CoRE4INET::SR_CLASS srClass, size_t framesize,
        uint16_t intervalFrames): ApplicationInformation::ApplicationInformation(serviceId, address, instanceId, tcpPort, udpPort,
                streamId, srClass,framesize, intervalFrames), _qosGroups(qosGroups) {

}

PublisherApplicationInformation::~PublisherApplicationInformation() {
}

std::set<QoSGroup> PublisherApplicationInformation::getQosGroups() const {
    return _qosGroups;
}

bool PublisherApplicationInformation::containsQoSGroup(QoSGroup qosGroup) {
    return std::find(_qosGroups.begin(), _qosGroups.end(), qosGroup) != _qosGroups.end();
}

bool PublisherApplicationInformation::operator==(const PublisherApplicationInformation& publisherApplicationInformation) const {
    return static_cast<ApplicationInformation>(*this) == static_cast<ApplicationInformation>(publisherApplicationInformation)
            && _qosGroups == publisherApplicationInformation.getQosGroups();
}

bool PublisherApplicationInformation::operator!=(const PublisherApplicationInformation& publisherApplicationInformation) const {
    return !(*this == publisherApplicationInformation);
}

} /* end namespace SOA4CoRE */
