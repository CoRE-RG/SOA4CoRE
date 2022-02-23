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

#ifndef SOA4CORE_DISCOVERY_SOMEIP_EXTRACTEDQOSOPTIONS_H_
#define SOA4CORE_DISCOVERY_SOMEIP_EXTRACTEDQOSOPTIONS_H_

#include "soa4core/messages/qosnegotiation/QoSNegotiationProtocol_m.h"

namespace SOA4CoRE {

/**
 * @brief Container class for QoS dependent parameters
 *
 * @ingroup soa4core/discovery
 *
 * @author Mehmet Mueller for HAW Hamburg
 */
class ExtractedQoSOptions {
/**
 * Methods
 */
public:
    /**
     * Constructor
     * @param qosGroup the QoS group
     * @param udpPort the UDP port of this QoS group
     * @param tcpPort the TCP port of this QoS group
     */
    ExtractedQoSOptions(QoSGroup qosGroup, int udpPort, int tcpPort);
    virtual ~ExtractedQoSOptions();

    /**
     * Returns the QoS group
     * @return the QoS group
     */
    QoSGroup getQosGroup() const;

    /**
     * Returns the TCP Port
     * @return the TCP Port
     */
    int getTcpPort() const;

    /**
     * Returns the UDP port
     * @return the UDP port
     */
    int getUdpPort() const;

protected:
private:

/**
 * Member variables
 */
public:
protected:
private:
    /**
     * The QoS group
     */
    QoSGroup _qosGroup;

    /**
     * The UDP port
     */
    int _udpPort;

    /**
     * The TCP port
     */
    int _tcpPort;
};

} /* namespace SOA4CoRE */

#endif /* SOA4CORE_DISCOVERY_SOMEIP_EXTRACTEDQOSOPTIONS_H_ */
