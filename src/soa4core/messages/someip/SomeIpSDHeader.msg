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

cplusplus {{
#include "soa4core/messages/someip/SomeIpHeader_m.h"
}}

//
// @brief The SOME/IP SD entry for the SOME/IP protocol. (see (RS_SOMEIPSD_00001))
//
// @ingroup soa4core/messages
// 
// @author Mehmet Mueller for HAW Hamburg
//

namespace SOA4CoRE;

class SomeIpHeader;

//SOME/IP header
packet SomeIpSDHeader extends SomeIpHeader {
    @customize(true);
    byteLength = 28;
    messageID = 0xFFFF8100;
    requestID = 0x00000001;
    protocolVersion = PV_1;
    interfaceVersion = IV_1;
    messageType = NOTIFICATION;
    returnCode = E_OK;
    uint32_t flags;
    uint32_t entriesLength = 0;
    //std::list<SomeIpSDEntry*> entryList; will be created in SomeIpSDHeader.h
    uint32_t optionsLength = 0;
    //std::list<SomeIpSDOption*> optionList; will be created in SomeIpSDHeader.h
}
