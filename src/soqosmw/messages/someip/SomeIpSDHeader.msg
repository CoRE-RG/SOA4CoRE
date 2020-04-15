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
#include "soqosmw/messages/someip/Entry_m.h"
#include "soqosmw/messages/someip/Option_m.h"

#define MIN_SOMEIPSDHEADERBYTESIZE 12
}}

class noncobject Entry;
class noncobject Option; 

namespace SOQoSMW;

packet SomeIpSD {
    uint32_t flags;
    uint32_t entriesLength;
    Entry entriesArray[];
    uint32_t optionsLength;
    Option optionsArray[];
}
