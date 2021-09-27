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

#include "soqosmw/service/someipservice/SomeIpService.h"

namespace SOQoSMW {

SomeIpService::SomeIpService(int serviceId, inet::L3Address address, int port, uint16_t instanceId) :
        ServiceBase(serviceId,address,port), _instanceId(instanceId) {
}

SomeIpService::~SomeIpService() {
}

uint16_t SomeIpService::getInstanceId() const{
    return _instanceId;
}
} /* end namespace SOQoSMW */
