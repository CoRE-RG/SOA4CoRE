//
// c Timo Haeckel for HAW Hamburg
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

#include "StaticServiceDiscoveryTest.h"
#include "soqosmw/serviceregistry/localserviceregistry/LocalServiceRegistry.h"
#include <string>

namespace SOQoSMW {

Define_Module(StaticServiceDiscoveryTest);

void StaticServiceDiscoveryTest::initialize() {
    cMessage* startSignal = new cMessage("sdTest");

    scheduleAt(simTime() + 1, startSignal);

    _sr = dynamic_cast<LocalServiceRegistry*>(getParentModule()->getSubmodule(
            par("lsr")));

}

void StaticServiceDiscoveryTest::handleMessage(cMessage *msg) {
    if (msg->isSelfMessage() && msg->isName("sdTest")) {
        std::cout << "Printing Registry from Test Module:" << endl;
        for (auto& service : dynamic_cast<LocalServiceRegistry*>(_sr)->getRegistry()) {
            std::cout << service.first.getServiceId() << ": " << service.second->getServiceName() << std::endl;
        }
    }
}

} /*end namespace SOQoSMW*/
