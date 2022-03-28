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

#include <soa4core/registry/base/Registry.h>

namespace SOA4CoRE {

Define_Module(Registry);

Registry::Registry() {

}

Registry::~Registry() {
}

void Registry::initialize()
{
}

void Registry::handleMessage(cMessage *msg)
{
}

void Registry::addPublisherService(PublisherApplicationInformation publisherApplicationInformation) {
    _serviceRegistry[publisherApplicationInformation.getServiceId()] = publisherApplicationInformation;
}

PublisherApplicationInformation Registry::getService(ServiceIdentifier serviceIdentifier) {
    PublisherApplicationInformation service;
    if (_serviceRegistry.count(serviceIdentifier.getServiceId())) {
        service = _serviceRegistry[serviceIdentifier.getServiceId()];
    }
    return service;
}

bool Registry::containsService(ServiceIdentifier serviceIdentifier) {
    return _serviceRegistry.count(serviceIdentifier.getServiceId());
}

std::unordered_map<Registry::ServiceId,PublisherApplicationInformation> Registry::getRegistry() {
    return _serviceRegistry;
}

} /* end namespace SOA4CoRE */
