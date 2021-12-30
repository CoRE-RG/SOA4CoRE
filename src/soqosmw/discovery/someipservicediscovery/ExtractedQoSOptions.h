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

#ifndef SOQOSMW_DISCOVERY_SOMEIPSERVICEDISCOVERY_EXTRACTEDQOSOPTIONS_H_
#define SOQOSMW_DISCOVERY_SOMEIPSERVICEDISCOVERY_EXTRACTEDQOSOPTIONS_H_

#include "soqosmw/messages/qosnegotiation/QoSNegotiationProtocol_m.h"

namespace SOQoSMW {

/**
 * @brief Container class for QoS dependent parameters
 *
 * @ingroup soqosmw/discovery
 *
 * @author Mehmet Cakir
 */
class ExtractedQoSOptions {
/**
 * Methods
 */
public:
    ExtractedQoSOptions(QoSGroups qosGroup, int udpPort, int tcpPort);
    virtual ~ExtractedQoSOptions();

    /**
     * Returns the QoS group
     * @return the QoS group
     */
    QoSGroups getQosGroup() const;

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
    QoSGroups _qosGroup;

    /**
     * The UDP port
     */
    int _udpPort;

    /**
     * The TCP port
     */
    int _tcpPort;
};

} /* namespace SOQoSMW */

#endif /* SOQOSMW_DISCOVERY_SOMEIPSERVICEDISCOVERY_EXTRACTEDQOSOPTIONS_H_ */
