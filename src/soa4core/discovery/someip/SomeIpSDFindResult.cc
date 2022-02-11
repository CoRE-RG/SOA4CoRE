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

#include <soa4core/discovery/someip/SomeIpSDFindResult.h>

namespace SOA4CoRE {

SomeIpSDFindResult::SomeIpSDFindResult(L3Address remoteAddress,
        PublisherApplicationInformation publisherApplicationInformation) :
        _remoteAddress(remoteAddress), _publisherApplicationInformation(publisherApplicationInformation) {
}

SomeIpSDFindResult::~SomeIpSDFindResult() {
}

inet::L3Address SomeIpSDFindResult::getRemoteAddress() const {
    return _remoteAddress;
}

PublisherApplicationInformation SomeIpSDFindResult::getPublisherApplicationInformation() const {
    return _publisherApplicationInformation;
}

} /* end namespace SOA4CoRE */
