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

#include "LocalServiceRegistry.h"

namespace SOQoSMW {

Define_Module(LocalServiceRegistry);

void LocalServiceRegistry::initialize()
{
    // TODO - Generated method body
}

void LocalServiceRegistry::handleMessage(cMessage *msg)
{
    // Do nothing
}

/**
 * Adds a published service
 */
void LocalServiceRegistry::addPublisherService(IService service) {
    //TODO to implement
}

/**
 * Returns a service for given serice identifier
 * @return IService
 */
IService LocalServiceRegistry::getService(IServiceIdentifier serviceIdentifier) {
    //TODO to implement
}

} /* end namespace SOQoSMW */
