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

enum OptionType {
    CONFIGURATION  = 0x01;
    LOADBALANCING  = 0x02;
    IPV4ENDPOINT   = 0x04;
    IPV6ENDPOINT   = 0x06;
    IPV4MULTICAST  = 0x14;
    IPV6MULTICAST  = 0x16;
    IPV4SDENDPOINT = 0x24;
    IPV6SDENDPOINT = 0x26;
}

enum L4Protocol {
    TCP = 0;
    UDP = 1;
}

packet Option {
    byteLength = 4;
    uint16_t length;
    uint8_t type @enum(OptionType);
    uint8_t reserved = 0;
}

packet ConfigurationOption extends Option {
    type = CONFIGURATION;
    string configurationString;
}

packet LoadBalancingOption extends Option {
    byteLength = 8;
    length = 0x0005;
    type = LOADBALANCING;
    uint16_t priority;
    uint16_t weight;
}

packet IPv4EndpointOption extends Option {
    byteLength = 12;
    length = 0x0009;
    type = IPV4ENDPOINT;
    uint32_t ipv4Address;
    uint8_t reserved = 0;
    uint8_t l4Protocol @enum(L4Protocol);
    uint16_t port;
}

packet IPv6EndpointOption extends Option {
    byteLength = 24;
    length = 0x0015;
    type = IPV6ENDPOINT;
    char ipv6Address[16];
    uint8_t reserved = 0;
    uint8_t l4Protocol @enum(L4Protocol);
    uint16_t port;
}

packet IPv4MulticastOption extends Option {
    byteLength = 12;
    length = 0x0009;
    type = IPV4MULTICAST;
    uint32_t ipv4Address;
    uint8_t reserved = 0;
    uint8_t l4Protocol @enum(L4Protocol);
    uint16_t port;
}

packet IPv6MulticastOption extends Option {
    byteLength = 24;
    length = 0x0015;
    type = IPV6MULTICAST;
    char ipv6Address[16];
    uint8_t reserved = 0;
    uint8_t l4Protocol @enum(L4Protocol);
    uint16_t port;
}

packet IPv4SDEndpointOption extends Option {
    byteLength = 12;
    length = 0x0009;
    type = IPV4SDENDPOINT;
    uint32_t ipv4Address;
    uint8_t reserved = 0;
    uint8_t l4Protocol @enum(L4Protocol);
    uint16_t port;
}

packet IPv6SDEndpointOption extends Option {
    byteLength = 24;
    length = 0x0015;
    type = IPV6SDENDPOINT;
    char ipv6Address[16];
    uint8_t reserved = 0;
    uint8_t l4Protocol @enum(L4Protocol);
    uint16_t port;
}