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

#ifndef SOA4CORE_SERVICE_SUBSCRIBERAPPLICATIONINFORMATION_SUBSCRIBERAPPLICATIONINFORMATION_H_
#define SOA4CORE_SERVICE_SUBSCRIBERAPPLICATIONINFORMATION_SUBSCRIBERAPPLICATIONINFORMATION_H_

#include "soa4core/service/applicationinformation/ApplicationInformation.h"
//AUTO-GENERATED MESSAGES
#include "soa4core/messages/qosnegotiation/QoSNegotiationProtocol_m.h"

namespace SOA4CoRE {

class SubscriberApplicationInformation: public ApplicationInformation {
    /**
     * Methods
     */
public:
    SubscriberApplicationInformation();

    SubscriberApplicationInformation(int serviceId, inet::L3Address address, uint16_t instanceId = INSTANCE_ID, QoSGroup qosGroup = QoSGroup::RT,
            int tcpPort = -1, int udpPort = -1, int streamId = -1, CoRE4INET::SR_CLASS srClass = CoRE4INET::SR_CLASS::A,
            size_t framesize = -1, uint16_t intervalFrames = -1);
    virtual ~SubscriberApplicationInformation();

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
    QoSGroup _qosGroup;
};
} /* end namespace SOA4CoRE */

#endif /* SOA4CORE_SERVICE_SUBSCRIBERAPPLICATIONINFORMATION_SUBSCRIBERAPPLICATIONINFORMATION_H_ */