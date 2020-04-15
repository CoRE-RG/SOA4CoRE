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

namespace SOQoSMW;

enum EntryType{
    FIND = 0x00;
    OFFER = 0x01;
}

packet Entry {
    byteLength = 12;
    uint8_t type @enum(EntryType);
    uint8_t index1stOptions;
    uint8_t index2nsOptions;
    uint8_t num1stAnd2ndOptions;
    uint16_t serviceID;
    uint16_t instanceID;
    uint32_t majorVersion_TTL;
}

packet ServiceEntry extends Entry {
    byteLength = 16;
    uint32_t minorVersion;
}

packet EventgroupEntry extends Entry{
    byteLength = 16;
    uint8_t reserved = 0;
    uint8_t initialDataRequest_Counter;
    uint16_t eventgroupID;
}