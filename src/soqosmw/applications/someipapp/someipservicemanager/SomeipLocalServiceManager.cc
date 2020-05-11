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

#include <soqosmw/applications/someipapp/someipservicemanager/SomeipLocalServiceManager.h>
#include <soqosmw/applications/someipapp/someipSDTestApp/publisherapp/SomeipPublisher.h>
#include <soqosmw/applications/someipapp/someipSDTestApp/subscriberapp/SomeipSubscriber.h>

namespace SOQoSMW {

Define_Module(SomeipLocalServiceManager);

void SomeipLocalServiceManager::initialize() {
    cModule* module = getParentModule()->getSubmodule("udpApp", SOMEIPSDIDX);
    if(_someipSD = dynamic_cast<SomeipSD*>(module)){
    } else {
        throw cRuntimeError("Submodule at index %d is no Someip SD app."
                "Please place the SomeipServiceDiscovery at index %d", SOMEIPSDIDX);
    }
    module = getParentModule()->getSubmodule("udpApp", SOMEIPLOCALSERVICEREGISTRYIDX);
    if(_someipLSR = dynamic_cast<SomeipLocalServiceRegistry*>(module)){
    } else {
        throw cRuntimeError("Submodule at index %d is no Someip LSR app."
                "Please place the SomeipLocalServiceRegistry at index %d", SOMEIPLOCALSERVICEREGISTRYIDX);
    }
}

void SomeipLocalServiceManager::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
}

void SomeipLocalServiceManager::registerPublisherService(SomeipPublisher *someipPublisher) {


}

void SomeipLocalServiceManager::registerSubscriberService(SomeipSubscriber *someipSubscriber) {

}

void SomeipLocalServiceManager::discoverService(uint16_t serviceID, uint16_t instanceID) {

}

}
