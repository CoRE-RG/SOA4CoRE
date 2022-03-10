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
// @brief The SOME/IP SD entry for the SOME/IP protocol. (see [PRS_SOMEIPSD_00266])
//
// @ingroup soa4core/messages
// 
// @author Mehmet Mueller for HAW Hamburg
//

namespace SOA4CoRE;

//SOME/IP SD entry type
enum SomeIpSDEntryType{
    FIND                  = 0x00; // A Service SomeIpSDEntry
    OFFER                 = 0x01; // A Service SomeIpSDEntry
    SUBSCRIBEVENTGROUP    = 0x06; // A Eventgroup SomeIpSDEntry
    SUBSCRIBEVENTGROUPACK = 0x07; // A Eventgroup SomeIpSDEntry
    
}

//SOME/IP SD entry
packet SomeIpSDEntry {
    byteLength = 12;
    uint8_t type @enum(SomeIpSDEntryType);
    // Two option fields are used to support two different types of options:
    // Options common between multiple SOME/IP-SD entries and options different for each SOME/IP-SD entry.
    // This way redundant information can be avoided. see [PRS_SOMEIPSD_00271]
    uint8_t index1stOptions;
    uint8_t index2ndOptions;
    uint8_t num1stOptions; // actually 4Bit
    uint8_t num2ndOptions; // actually 4Bit
    uint16_t serviceID;
    uint16_t instanceID;
    uint8_t majorVersion;
    uint32_t TTL; // actually 24Bit
}

//SOME/IP SD service entry
packet ServiceEntry extends SomeIpSDEntry {
    byteLength = 16;
    uint32_t minorVersion;
}

//SOME/IP SD eventgroup entry 
packet EventgroupEntry extends SomeIpSDEntry{
    byteLength = 16;
    uint8_t reserved = 0;
    uint8_t initialDataRequest_Counter;
    uint16_t eventgroupID;
}