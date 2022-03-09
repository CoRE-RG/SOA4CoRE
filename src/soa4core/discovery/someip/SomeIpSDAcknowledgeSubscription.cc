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

#include "soa4core/discovery/someip/SomeIpSDAcknowledgeSubscription.h"

namespace SOA4CoRE {

SomeIpSDAcknowledgeSubscription::SomeIpSDAcknowledgeSubscription(inet::L3Address remoteAddress, PublisherApplicationInformation publisherApplicationInformation, QoSGroup qosGroup) :
        _remoteAddress(remoteAddress), _publisherApplicationInformation(publisherApplicationInformation), _qosGroup(qosGroup) {
}

SomeIpSDAcknowledgeSubscription::~SomeIpSDAcknowledgeSubscription() {
}

const inet::L3Address& SomeIpSDAcknowledgeSubscription::getRemoteAddress() const {
    return _remoteAddress;
}

const PublisherApplicationInformation& SomeIpSDAcknowledgeSubscription::getPublisherApplicationInformation() const {
    return _publisherApplicationInformation;
}

QoSGroup SomeIpSDAcknowledgeSubscription::getQosGroup() const {
    return _qosGroup;
}

} /* namespace SOA4CoRE */
