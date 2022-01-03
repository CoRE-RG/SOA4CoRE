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

#ifndef SOQOSMW_DISCOVERY_SOMEIPSERVICEDISCOVERY_SOMEIPSDACKNOWLEDGESUBSCRIPTION_H_
#define SOQOSMW_DISCOVERY_SOMEIPSERVICEDISCOVERY_SOMEIPSDACKNOWLEDGESUBSCRIPTION_H_

#include "soqosmw/service/publisherapplicationinformation/PublisherApplicationInformation.h"

namespace SOQoSMW {

class SomeIpSDAcknowledgeSubscription : public cObject{
    /**
     * Methods
     */
public:
    SomeIpSDAcknowledgeSubscription(inet::L3Address remoteAddress, PublisherApplicationInformation publisherApplicationInformation, QoSGroup qosGroup);
    virtual ~SomeIpSDAcknowledgeSubscription();

    /**
     * Returns the remote address of the subscriber
     * @return the remote address of the subscriber
     */
    const inet::L3Address& getRemoteAddress() const;

    /**
     * Returns the publisher application information
     * @return the publisher application information
     */
    const PublisherApplicationInformation& getPublisherApplicationInformation() const;

    /**
     * Returns the QoS group
     * @return the QoS group
     */
    QoSGroup getQosGroup() const;

protected:
private:

    /**
     * Member variables
     */
public:
protected:
private:
    /**
     * The remote address of the subscriber
     */
    inet::L3Address _remoteAddress;

    /**
     * The subscriber application information
     */
    PublisherApplicationInformation _publisherApplicationInformation;

    /**
     * The QoS group
     */
    QoSGroup _qosGroup;
};

} /* namespace SOQoSMW */
#endif /* SOQOSMW_DISCOVERY_SOMEIPSERVICEDISCOVERY_SOMEIPSDACKNOWLEDGESUBSCRIPTION_H_ */
