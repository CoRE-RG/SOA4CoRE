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

//
// @brief Message class for connection specific information.
//
// @ingroup soqosmw/messages
// 
// @author Timo Haeckel and Mehmet Cakir for HAW Hamburg
//
 
namespace SOQoSMW;

//Type of ConenctionSpecificInformation
enum ConnectionType{
    ct_udp			= 0;
    ct_tcp			= 1;
    ct_http			= 2;
    ct_avb			= 3;
}

packet ConnectionSpecificInformation {
    int connectionType @enum(ConnectionType);
}

packet CSI_TCP extends ConnectionSpecificInformation {
    connectionType = ConnectionType::ct_tcp;
    string address;
    int port;
}

packet CSI_UDP extends ConnectionSpecificInformation {
    connectionType = ConnectionType::ct_udp;
    string address;
    int port;
}

packet CSI_AVB extends ConnectionSpecificInformation {
    connectionType = ConnectionType::ct_avb;
    long streamID;
    int vlanID;
}
