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

#include <soa4core/registry/base/Registry.h>
#include "StaticServiceDiscoveryTest.h"
//STD
#include <string>

namespace SOA4CoRE {

Define_Module(StaticServiceDiscoveryTest);

void StaticServiceDiscoveryTest::initialize() {
    cMessage* startSignal = new cMessage("sdTest");

    scheduleAt(simTime() + 1, startSignal);

    _sr = dynamic_cast<Registry*>(getParentModule()->getSubmodule(
            par("sr")));

}

void StaticServiceDiscoveryTest::handleMessage(cMessage *msg) {
    if (msg->isSelfMessage() && msg->isName("sdTest")) {
        std::cout << "Printing Registry from Test Module:" << endl;
        for (auto& service : dynamic_cast<Registry*>(_sr)->getRegistry()) {
            std::cout << service.first << ": " << service.second.getAddress() << std::endl;
        }
    }
}

} /*end namespace SOA4CoRE*/
