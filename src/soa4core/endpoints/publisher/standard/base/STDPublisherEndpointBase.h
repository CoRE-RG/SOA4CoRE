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

#ifndef __SOA4CORE_STDPUBLISHERENDPOINTBASE_H_
#define __SOA4CORE_STDPUBLISHERENDPOINTBASE_H_

#include "soa4core/endpoints/publisher/base/PublisherEndpointBase.h"

#include "core4inet/networklayer/inet/base/IPv4oREBase.h"
#include "core4inet/networklayer/inet/base/IPoREFilter.h"
#include "core4inet/networklayer/inet/IEEE8021Q/IEEE8021QDestinationInfo.h"
#include "core4inet/networklayer/inet/base/TrafficPattern.h"
#include "inet/linklayer/common/MACAddress.h"
#include "inet/networklayer/contract/ipv4/IPv4Address.h"
#include "inet/networklayer/contract/IARP.h"
#include <list>

namespace SOA4CoRE {

/**
 * @brief The STDPublisherEndpointBase is a publisher endpoint module with STD qos (@see~EndpointBase).
 * Publishers are created by the QoSNegotiationProtocol module (@see~QoSNegotiationProtocol)
 * during runtime to connect realise a publishing endpoint for a service.
 * Publishers are connected to a specific transport and a publisher connector module (@see~PublisherConnector)
 * which loosely couples the publisher endpoint with a publisher application (@see~Publisher).
 * Optionally allows to configure the IPv4oRE stack from CoRE4INET to add 802.1Q tags.
 *
 * @ingroup soa4core/endpoints
 *
 * @author Timo Haeckel and Mehmet Mueller for HAW Hamburg
 */
class STDPublisherEndpointBase : public PublisherEndpointBase
{
/**
 * Methods
 */
public:
protected:
    virtual void initialize() override;
    virtual void handleParameterChange(const char* parname) override;
    CoRE4INET::TrafficPattern * createTrafficPattern() {
        return new CoRE4INET::TrafficPattern();
    }

    /**
     * Creates and installs a traffic filter on the IPv4oIEEE8021Q network layer
     * @param destAddr the destination IP
     * @param srcPort the transport src port
     * @param destPort the transport dest port
     */
    void createAndInstallFilter(inet::IPv4Address destAddr, int srcPort, int destPort);

    /**
     * Installs the given filter on the IPv4oIEEE8021Q network layer
     * @param filter the filter to be added
     */
    void installFilter(CoRE4INET::IPoREFilter* filter);

    /**
     * Build a traffic pattern based on the destIP, srcPort and destPort.
     * @param destAddr the destination IP
     * @param srcPort the transport src port
     * @param destPort the transport dest port
     * @return a traffic pattern for streams from this endpoint
     */
    CoRE4INET::TrafficPattern* createTrafficPattern(inet::IPv4Address destAddr, int srcPort, int destPort);

    /**
     * Builds a 802.1Q destination info used in a IPoREFilter to create the QHeader on layer 2
     * Calls the createDestinationInfo with the class attributes and resolves the default gate
     * @param destMAC the destination MAC to be used
     * @return the destination info
     */
    CoRE4INET::IEEE8021QDestinationInfo* createDestinationInfo(inet::MACAddress destMAC);

    /**
     * Builds a 802.1Q destination info used in a IPoREFilter to create the QHeader on layer 2
     * @param vid the vlan id to be set
     * @param pcp the pcp to be set
     * @param destMAC the destination MAC to be used
     * @param destGates the destination gate the layer2 frame shall be forwarded to
     * @return the destination info
     */
    CoRE4INET::IEEE8021QDestinationInfo* createDestinationInfo(uint16_t vid, uint8_t pcp, inet::MACAddress destMAC,
            std::list<cGate*>& destGates);

    /**
     * Tries to resolve the destination MAC for the given IPv4 address
     * @param destAddress the IP address to resolve
     * @return the MAC address
     */
    inet::MACAddress resolveDestMacAddress(inet::IPv4Address destAddress);
private:

/**
 * Member variables
 */
public:
protected:

    /**
     * Local ip address for UDP socket
     */
    std::string _localAddress;

    /**
     * Local port for UDP socket
     */
    int _localPort;

    /**
     * VLAN ID for 802.1Q filters in the IPv4oRE network layer
     */
    uint16_t _vlanID = -1;

    /**
     * PCP for 802.1Q filters in the IPv4oRE network layer
     */
    uint8_t _pcp = -1;

    /**
     * Default destination gate for 802.1Q packets in the IPv4oRE network layer
     */
    std::string _defaultDestGate = "bgOut[0]";

    /**
     * The ipv4 module form IPv4oRE stack
     */
    CoRE4INET::IPv4oREBase* _networkLayer;

    /**
     *
     */
    inet::IARP* _arp = nullptr;
private:
};

} /*end namespace SOA4CoRE*/

#endif
