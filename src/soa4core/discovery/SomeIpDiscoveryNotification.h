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

#ifndef SOA4CORE_DISCOVERY_SOMEIPDISCOVERYNOTIFICATION_H_
#define SOA4CORE_DISCOVERY_SOMEIPDISCOVERYNOTIFICATION_H_

#include "soa4core/discovery/DiscoveryNotification.h"
//AUTO-GENERATED MESSAGES
#include "soa4core/messages/qosnegotiation/QoSNegotiationProtocol_m.h"
#include "soa4core/messages/someip/SomeIpSDOption_m.h"
//INET
#include <inet/networklayer/common/L3Address.h>

namespace SOA4CoRE {

/**
 * @brief The SomeIpDiscoveryNotification is used as a notification class for a
 * discovered SOME/IP publisher service or a requesting subscriber.
 *
 * @ingroup soa4core/discovery
 *
 * @author Mehmet Mueller
 */

class SomeIpDiscoveryNotification : public DiscoveryNotification {
    /**
     * Methods
     */
public:
    /**
     * Constructor
     * @param serviceId the service id of the application
     * @param address the IP address of the node where the application resides
     * @param instanceId the instance id of the service instance
     * @param qosGroups the QoS groups offered in case of a SOME/IP publisher
     * @param qosGroup the QoS group requested in case of a SOME/IP subscriber
     * @param tcpPort the TCP port of this service
     * @param udpPort the UDP port of this service
     * @param mcastDestAddr the IP multicast destination address of this service
     * @param mcastDestPort the multicast destination port of this service
     */
    SomeIpDiscoveryNotification(int serviceId, inet::L3Address address, uint16_t instanceId, std::set<QoSGroup> qosGroups,
                                QoSGroup qosGroup, int tcpPort, int udpPort, inet::L3Address mcastDestAddr = inet::L3Address(), int _mcastDestPort = -1);
    /**
     * Default Constructor. Set all values manually!
     */
    SomeIpDiscoveryNotification();
    virtual ~SomeIpDiscoveryNotification();

    /**
     * Copy constructor
     * @param someIpDiscoveryNotification the other SOME/IP discovery notification
     */
    SomeIpDiscoveryNotification(const SomeIpDiscoveryNotification& someIpDiscoveryNotification);

    /**
     * Assignment operator
     * @param other the SOME/IP discovery notification which is assigned to this SOME/IP discovery notification
     * @return this SOME/IP discovery notification
     */
    SomeIpDiscoveryNotification& operator=(const SomeIpDiscoveryNotification& other);

    /**
     * Returns a duplicate of this SOME/IP discovery notification
     * @return a duplicate of this SOME/IP discovery notification
     */
    virtual SomeIpDiscoveryNotification* dup() const override;

    /**
     * Returns the instance ID
     * @return the instance ID
     */
    uint16_t getInstanceId() const;

    /**
     * Returns the QoS group
     * @return the QoS group
     */
    QoSGroup getQoSGroup() const;

    /**
     * Returns the QoS groups
     * @return the QoS groups
     */
    std::set<QoSGroup> getQoSGroups() const;

    /**
     * Returns the TCP port
     * @return the TCP port
     */
    int getTcpPort() const;

    /**
     * Returns the UDP port
     * @return the UDP port
     */
    int getUdpPort() const;

    /**
     * Returns the multicast destination port
     * @return the multicast destination port
     */
    int getMcastDestPort() const;

    /**
     * Returns the multicast destination address
     * @return the multicast destination address
     */
    inet::L3Address getMcastDestAddr() const;

    virtual bool operator==(const SomeIpDiscoveryNotification& someIpDiscoveryNotification) const;
    virtual bool operator!=(const SomeIpDiscoveryNotification& someIpDiscoveryNotification) const;

    void setMcastDestAddr(const inet::L3Address& mcastDestAddr);
    void setMcastDestPort(int mcastDestPort);
    void setQosGroup(QoSGroup qosGroup);
    void setTcpPort(int tcpPort);
    void setUdpPort(int udpPort);
    void setInstanceId(uint16_t instanceId);

    /**
     * Inserts the QoS Group if not already in the set.
     * @return true if the QoS group has been inserted
     */
    bool addQoSGroup(QoSGroup qosGroup);

    /**
     * Updated information from a given endpoint option.
     * Only updated if qos group not already exists.
     * @param option the endpoint option to pars
     * @return true if updated
     */
    bool updateFromEndpointOption(SomeIpSDOption* option);

protected:
private:

    /**
     * Member variables
     */
public:
protected:
private:
    /**
     * Instance Id of the service
     */
    uint16_t _instanceId;

    /**
     * Caches the QoS types the publisher application can serve
     */
    std::set<QoSGroup> _qosGroups;

    /**
     * Caches the required QoS type by the subscriber
     */
    QoSGroup _qosGroup;

    /**
     * TCP port used for TCP endpoints
     */
    int _tcpPort;

    /**
     * UDP port used for UDP endpoints
     */
    int _udpPort;

    /**
     * IP multicast destination address used for IPv4MulticastEndpoints
     */
    inet::L3Address _mcastDestAddr;

    /**
     * UDP multicast destination port used for IPv4MulticastEndpoints
     */
    int _mcastDestPort;

};
} /* end namespace SOA4CoRE */
#endif /* SOA4CORE_DISCOVERY_SOMEIPDISCOVERYNOTIFICATION_H_ */
