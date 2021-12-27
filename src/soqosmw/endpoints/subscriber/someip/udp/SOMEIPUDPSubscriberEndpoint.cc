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

#include "SOMEIPUDPSubscriberEndpoint.h"

namespace SOQoSMW {

Define_Module(SOMEIPUDPSubscriberEndpoint);

ConnectionSpecificInformation* SOMEIPUDPSubscriberEndpoint::getConnectionSpecificInformation() {
    CSI_SOMEIP_UDP* connection = new CSI_SOMEIP_UDP();
    connection->setAddress(_localAddress.c_str());
    connection->setPort(_localPort);
    return connection;
}

} /* end namespace SOQoSMW */
