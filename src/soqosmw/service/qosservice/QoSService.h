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

#ifndef SOQOSMW_SERVICE_QOSSERVICE_QOSSERVICE_H_
#define SOQOSMW_SERVICE_QOSSERVICE_QOSSERVICE_H_

#include "soqosmw/service/qosserviceidentifier/QoSServiceIdentifier.h"

namespace SOQoSMW {

/**
 * @brief The service representation for apps
 *
 * @ingroup soqosmw/service
 *
 * @author Mehmet Mueller
 */

class QoSService {
    /**
     * Methods
     */
public:
    QoSService(int serviceId, inet::L3Address address, uint16_t instanceId, QoSGroups qos, int port = -1, int streamId = -1,
            CoRE4INET::SR_CLASS srClass = CoRE4INET::SR_CLASS::A, size_t framesize = -1, uint16_t intervalFrames = -1);
    virtual ~QoSService();

    virtual bool operator==(const QoSService& qoSService) const;
    virtual bool operator!=(const QoSService& qoSService) const;

    /**
     * Returns the service id
     * @return the service id
     */
    int getServiceId() const;

    /**
     * Returns the IP address
     * @return the IP address
     */
    inet::L3Address getAddress() const;

    /**
     * Returns the port
     * @return the port
     */
    int getPort() const;

    /**
     * Returns the QoS group this service provides
     * @return the QoS group
     */
    QoSGroups getQosGroup() const;

    /**
     * Returns the instance ID
     * @return the instance ID
     */
    uint16_t getInstanceId() const;

    /**
     * Returns the frame size used for AVB
     * @return the frame size
     */
    size_t getFramesize() const;

    /**
     * Returns the interval frames used for AVB
     * @return the interval frames
     */
    uint16_t getIntervalFrames() const;

    /**
     * Returns the stream reservation class for AVB
     * @return the stream reservation class
     */
    CoRE4INET::SR_CLASS getSrClass();

    /**
     * Returns the stream ID of the AVB traffic
     * @return the stream ID
     */
    int getStreamId() const;

protected:
private:

    /**
     * Member variables
     */
public:
protected:
private:
    /**
     * The service identifier
     */
    QoSServiceIdentifier _serviceIdentifier;

    /**
     * The IP address
     */
    inet::L3Address _address;

    /**
     * The port
     */
    int _port;

    /**
     * The QoS this service provides
     */
    QoSGroups _qos;

    /**
     * The stream ID for AVB traffic
     */
    int _streamId;

    /**
     * The stream reservation class for AVB traffic
     */
    CoRE4INET::SR_CLASS _srClass;

    /**
     * The frame size for AVB traffic
     */
    size_t _framesize;

    /**
     * The interval frames for AVB traffic
     */
    uint16_t _intervalFrames;

};

} /* namespace SOQoSMW */

#endif /* SOQOSMW_SERVICE_QOSSERVICE_QOSSERVICE_H_ */
