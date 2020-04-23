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
#include "soqosmw/messages/someip/SomeIpHeader_m.h"
#include "soqosmw/messages/someip/Entry_m.h"
#include "soqosmw/messages/someip/Option_m.h"
#include <vector>
}}

namespace SOQoSMW;

cplusplus {{
typedef std::vector<Entry> EntryList;
typedef std::vector<Option> OptionList;

#define MIN_SOMEIPSDHEADERBYTESIZE 12
}}

class SomeIpHeader;
class noncobject EntryList;
class noncobject OptionList;

packet SomeIpSDHeader extends SomeIpHeader {
    messageID = 0xFFFF8100;
    requestID = 0x00000001;
    protocolVersion = PV_1;
    interfaceVersion = IV_1;
    messageType = NOTIFICATION;
    returnCode = E_OK;
    uint32_t flags;
    uint32_t entriesLength = 0;
    EntryList entries;
    uint32_t optionsLength = 0;
    OptionList options;
}
