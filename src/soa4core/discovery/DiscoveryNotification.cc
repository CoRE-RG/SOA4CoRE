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

#include <soa4core/discovery/DiscoveryNotification.h>

namespace SOA4CoRE {

DiscoveryNotification::DiscoveryNotification(int serviceId, inet::L3Address address): _serviceId(serviceId), _address(address)  {
}

DiscoveryNotification::~DiscoveryNotification() {
}

int DiscoveryNotification::getServiceId() const {
    return _serviceId;
}


inet::L3Address DiscoveryNotification::getAddress() const {
       return _address;
}

} /* end namespace SOA4CoRE */

