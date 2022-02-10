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

#include "soa4core/discovery/someipservicediscovery/SomeIpSDSubscriptionInformation.h"

namespace SOA4CoRE {

SomeIpSDSubscriptionInformation::SomeIpSDSubscriptionInformation(inet::L3Address remoteAddress, SubscriberApplicationInformation subscriberApplicationInformation) :
        _remoteAddress(remoteAddress), _subscriberApplicationInformation(subscriberApplicationInformation){

}

SomeIpSDSubscriptionInformation::~SomeIpSDSubscriptionInformation() {
}

const inet::L3Address& SomeIpSDSubscriptionInformation::getRemoteAddress() const {
    return _remoteAddress;
}

const SubscriberApplicationInformation& SomeIpSDSubscriptionInformation::getSubscriberApplicationInformation() const {
    return _subscriberApplicationInformation;
}

} /* end namespace SOA4CoRE */
