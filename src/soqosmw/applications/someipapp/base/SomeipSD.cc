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

#include <soqosmw/applications/someipapp/base/SomeipSD.h>
#include "soqosmw/applications/someipapp/dummySD/publisherapp/SomeipPublisher.h"
#include "soqosmw/applications/someipapp/dummySD/subscriberapp/SomeipSubscriber.h"

namespace SOQoSMW {

Define_Module(SomeipSD);

void SomeipSD::initialize(int stage) {
    SomeipAppBase::initialize(stage);
}

void SomeipSD::handleMessageWhenUp(cMessage *msg) {

}

void SomeipSD::discover() {

}

//void SomeipSD::registerPublisher(SomeipPublisher* someipPublisher) {
//
//}

//void SomeipSD::registerSubscriber(SomeipSubscriber* someipSubscriber) {
//
//}

} /* end namespace SOQoSMW */
