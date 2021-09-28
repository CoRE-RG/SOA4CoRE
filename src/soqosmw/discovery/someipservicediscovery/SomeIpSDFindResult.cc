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

#include <soqosmw/discovery/someipservicediscovery/SomeIpSDFindResult.h>

namespace SOQoSMW {

SomeIpSDFindResult::SomeIpSDFindResult(int serviceId, int instanceId, inet::L3Address remoteAddress) :
        _serviceId(serviceId), _instanceId(instanceId), _remoteAddress(remoteAddress) {
}

SomeIpSDFindResult::~SomeIpSDFindResult() {
}

int SomeIpSDFindResult::getServiceId() const {
    return _serviceId;
}

int SomeIpSDFindResult::getInstanceId() const {
    return _instanceId;
}

inet::L3Address SomeIpSDFindResult::getRemoteAddress() const {
    return _remoteAddress;
}

IService* SomeIpSDFindResult::getService() const {
    return _service;
}

void SomeIpSDFindResult::setService(IService* service) {
    _service = service;
}



} /* end namespace SOQoSMW */
