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
#include "core4inet/base/avb/AVBDefs.h"
#include "inet/networklayer/common/L3Address.h"
#include "soqosmw/messages/qosnegotiation/QoSNegotiationProtocol_m.h"

using namespace CoRE4INET;
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
    QoSService();

    QoSService(int serviceId, inet::L3Address address, uint16_t instanceId, std::set<QoSGroups> qosGroups, int tcpPort = -1, int udpPort = -1, int streamId = -1,
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
     * Returns the TCP port
     * @return the TCP port
     */
    int getTCPPort() const;

    /**
     * Returns the UDP port
     * @return the UDP port
     */
    int getUDPPort() const;

    /**
     * Returns the QoS groups this service provides
     * @return the QoS groups
     */
    std::set<QoSGroups> getQosGroups() const;

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
    CoRE4INET::SR_CLASS getSrClass() const;

    /**
     * Returns the stream ID of the AVB traffic
     * @return the stream ID
     */
    int getStreamId() const;

    /**
     * Returns a common QoS group if present
     * @param qoSService
     * @return A common QoS group if present, else nullptr
     */
    QoSGroups* getCommonQoSGroup(QoSService qosService);

    /**
     * Returns true if this service contains the given QoS group
     * @param qosGroup
     * @return true if this service contains given qosGroup, else false
     */
    bool containsQoSGroup(QoSGroups qosGroup);

    /**
     * Returns true if the service is instantiated by the default constructor.
     * @return true if the service is instantiated by the default constructor, else false
     */
    bool isDefaultConstructed();

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
     * The TCP port
     */
    int _tcpPort;

    /**
     * The UDP port
     */
    int _udpPort;

    /**
     * The QoS groups this service provides
     */
    std::set<QoSGroups> _qosGroups;

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

    /**
     * Indicates if this service is instantiated by the default constructor,
     * since the default constructor does not specify a useful QoSService.
     */
    bool _defaultQoSService;

};

} /* namespace SOQoSMW */

#endif /* SOQOSMW_SERVICE_QOSSERVICE_QOSSERVICE_H_ */
