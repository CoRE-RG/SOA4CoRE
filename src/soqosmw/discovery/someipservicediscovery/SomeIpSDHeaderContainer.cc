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

#include <soqosmw/discovery/someipservicediscovery/SomeIpSDHeaderContainer.h>

namespace SOQoSMW {

SomeIpSDHeaderContainer::SomeIpSDHeaderContainer(SomeIpSDEntry& someIpSDEntry, SomeIpSDHeader& someIpSDHeader) :
        _someIpSDEntry(someIpSDEntry), _someIpSDHeader(someIpSDHeader) {
    // TODO Auto-generated constructor stub

}

SomeIpSDHeaderContainer::~SomeIpSDHeaderContainer() {
    // TODO Auto-generated destructor stub
}

SomeIpSDEntry& SomeIpSDHeaderContainer::getSomeIpSdEntry() const {
    return _someIpSDEntry;
}

SomeIpSDHeader& SomeIpSDHeaderContainer::getSomeIpSdHeader() const {
    return _someIpSDHeader;
}

IService* SomeIpSDHeaderContainer::getService() const {
    return _service;
}

void SomeIpSDHeaderContainer::setService(IService* service) {
    _service = service;
}

} /* end namespace SOQoSMW */
