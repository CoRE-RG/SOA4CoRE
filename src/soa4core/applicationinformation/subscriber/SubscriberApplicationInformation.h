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

#ifndef SOA4CORE_APPLICATIONINFORMATION_SUBSCRIBER_SUBSCRIBERAPPLICATIONINFORMATION_H_
#define SOA4CORE_APPLICATIONINFORMATION_SUBSCRIBER_SUBSCRIBERAPPLICATIONINFORMATION_H_

#include <soa4core/applicationinformation/base/ApplicationInformation.h>
#include "soa4core/messages/qosnegotiation/QoSNegotiationProtocol_m.h"

namespace SOA4CoRE {

/**
 * @brief The SubscriberApplicationInformation is a container for information about a subscriber application.
 *
 * @ingroup soa4core/applicationinformation
 *
 * @author Mehmet Mueller
 */

class SubscriberApplicationInformation: public ApplicationInformation {
    /**
     * Methods
     */
public:
    SubscriberApplicationInformation();

    /**
     * Constructor
     * @param serviceId the service id of the application
     * @param address the IP address of the node where the application resides
     * @param instanceId the instance id of the service instance
     * @param qosGroup the QoS group requested/subscribed by this subscriber
     * @param tcpPort the TCP port of this service
     * @param udpPort the UDP port of this service
     * @param streamId the AVB stream ID
     * @param srClass the stream reservation class of the AVB stream
     * @param framesize the AVB frame size
     * @param intervalFrames the AVB interval frames
     */
    SubscriberApplicationInformation(int serviceId, inet::L3Address address, uint16_t instanceId = INSTANCE_ID, QoSGroup qosGroup = QoSGroup::RT,
            int tcpPort = -1, int udpPort = -1, int streamId = -1, CoRE4INET::SR_CLASS srClass = CoRE4INET::SR_CLASS::A,
            size_t framesize = -1, uint16_t intervalFrames = -1);
    virtual ~SubscriberApplicationInformation();

    /**
     * Returns the QoS group
     * @return the QoS group
     */
    QoSGroup getQoSGroup() const;

    virtual bool operator==(const SubscriberApplicationInformation& subscriberApplicationInformation) const;
    virtual bool operator!=(const SubscriberApplicationInformation& subscriberApplicationInformation) const;
protected:
private:
    /**
     * Member variables
     */
public:
protected:
private:

    /**
     * The QoS group requested/subscribed by this subscriber
     */
    QoSGroup _qosGroup;
};
} /* end namespace SOA4CoRE */

#endif /* SOA4CORE_APPLICATIONINFORMATION_SUBSCRIBER_SUBSCRIBERAPPLICATIONINFORMATION_H_ */
