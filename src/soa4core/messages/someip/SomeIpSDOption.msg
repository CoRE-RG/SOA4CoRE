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

import inet.networklayer.common.IPProtocolId;
import inet.networklayer.contract.ipv4.IPv4Address;
import inet.networklayer.contract.ipv6.IPv6Address;

//
// @brief The SOME/IP SD option for the SOME/IP SD protocol. (see [PRS_SOMEIPSD_00273], [PRS_SOMEIPSD_00274])
//
// @ingroup soa4core/messages
// 
// @author Mehmet Mueller, and Timo Haeckel for HAW Hamburg
//


namespace SOA4CoRE;

//enum SomeIpSDOptionType;

//SOME/IP SD option type
enum SomeIpSDOptionType {
    CONFIGURATION  = 0x01;
    LOADBALANCING  = 0x02;
    IPV4ENDPOINT   = 0x04;
    IPV6ENDPOINT   = 0x06;
    IPV4MULTICAST  = 0x14;
    IPV6MULTICAST  = 0x16;
    IPV4SDENDPOINT = 0x24;
    IPV6SDENDPOINT = 0x26;
}

//SOME/IP SD option (see [PRS_SOMEIPSD_00273], [PRS_SOMEIPSD_00274])
packet SomeIpSDOption {
    byteLength = 4;
    uint16_t length;
    uint8_t type @enum(SomeIpSDOptionType);
    uint8_t reserved = 0;
}

//SOME/IP SD configuration option (see [PRS_SOMEIPSD_00276])
packet ConfigurationOption extends SomeIpSDOption {
    type = CONFIGURATION;
//    string configurationString;
}

//SOME/IP SD load balancing option (see [PRS_SOMEIPSD_00544])
packet LoadBalancingOption extends SomeIpSDOption {
    byteLength = 8;
    length = 0x0005;
    type = LOADBALANCING;
    uint16_t priority;
    uint16_t weight;
}

//SOME/IP SD IPv4 endpoint option (see [PRS_SOMEIPSD_00307])
packet IPv4EndpointOption extends SomeIpSDOption {
    byteLength = 12;
    length = 0x0009;
    type = IPV4ENDPOINT;
    reserved = 0;
    inet::IPv4Address ipv4Address;
    uint8_t l4Protocol @enum(inet::IPProtocolId);
    uint16_t port;
}

//SOME/IP SD IPv6 endpoint option (see [PRS_SOMEIPSD_00315])
packet IPv6EndpointOption extends SomeIpSDOption {
    byteLength = 24;
    length = 0x0015;
    type = IPV6ENDPOINT;
    reserved = 0;
    inet::IPv6Address ipv6Address;
    uint8_t l4Protocol @enum(inet::IPProtocolId);
    uint16_t port;
}

//SOME/IP SD IPv4 multicast option (see [PRS_SOMEIPSD_00326])
packet IPv4MulticastOption extends IPv4EndpointOption {
    type = IPV4MULTICAST;
}

//SOME/IP SD IPv6 multicast option (see [[PRS_SOMEIPSD_00333])
packet IPv6MulticastOption extends IPv6EndpointOption {
    type = IPV6MULTICAST;
}

//SOME/IP SD IPv4 SD endpoint option (see [PRS_SOMEIPSD_00552])
packet IPv4SDEndpointOption extends IPv4EndpointOption {
    type = IPV4SDENDPOINT;
}

//SOME/IP SD IPv4 SD endpoint option (see [PRS_SOMEIPSD_00559])
packet IPv6SDEndpointOption extends IPv6EndpointOption {
    type = IPV6SDENDPOINT;
}

////////////////////////////////////////////////////////////////////////////////
// custom experimental extensions from studies conducted with this framework: //
// extends SOME/IP SD configuration option (see [PRS_SOMEIPSD_00276])         //
////////////////////////////////////////////////////////////////////////////////

// Information for 802.1Q traffic priorities and vlans
// to be attached to offer or subscribe messages
packet IEEE8021QConfigurationOption extends ConfigurationOption {
    byteLength = 8; // 4 from base base + padding to 32 bit.
    uint8_t pcp;
    // unit8_t padding;
    uint16_t vlan_id;
}

// Information for required ressources of the stream 
// to be attached in offer messages
packet RessourceConfigurationOption extends ConfigurationOption {
    byteLength = 12; // 4 from base base + padding to 32 bit.
    uint16_t maxPayload; // the maximum payload. The layer 1 frame size depends on the used endpoint protocol stack
    double minInterval; // the minimum interval between two frames of this streams
    //NOTE: for simplicity we use a double here based on seconds in a real protocol this will probably be represented by a uint32_t in us, for example.
}

// Information for required ressources of the stream 
// to be attached in offer or subscribe messages
packet RealTimeConfigurationOption extends ConfigurationOption {
    byteLength = 8; // 4 from base base + padding to 32 bit.
    double deadline; // the maximum tolerated latency for packets of this service 
    //NOTE: for simplicity we use a double here based on seconds in a real protocol this will probably be represented by a uint32_t in us, for example.
}
