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

#include <soqosmw/service/qosservice/QoSService.h>

namespace SOQoSMW {

QoSService::QoSService(int serviceId, inet::L3Address address, uint16_t instanceId, QoSGroups qos, int port = -1, int streamId = -1,
        CoRE4INET::SR_CLASS srClass = CoRE4INET::SR_CLASS::A, size_t framesize = -1, uint16_t intervalFrames = -1):
        QoSServiceIdentifier(serviceId,instanceId), _address(address), _port(port), _streamId(streamId), _srClass(srClass), _framesize(framesize), _intervalFrames(intervalFrames){
}

QoSService::~QoSService() {
}

int QoSService::getServiceId() const {
    return _serviceIdentifier.getServiceId();
}

inet::L3Address QoSService::getAddress() const {
    return _address;
}

QoSGroups QoSService::getQosGroup() const {
    return _qos;
}

int QoSService::getPort() const {
    return _port;
}

bool QoSService::operator==(const QoSService& qoSService) const {
    return (_serviceIdentifier == qoSService._serviceIdentifier)
            && _address == qoSService.getAddress()
            && _port == qoSService.getPort()
            && _qos == qoSService.getQosGroup()
            && _streamId == qoSService.getStreamId()
            && _srClass == qoSService.getSrClass()
            && _framesize == qoSService.getFramesize()
            && _intervalFrames == qoSService.getIntervalFrames();
}

bool QoSService::operator!=(const QoSService& qoSService) const {
    return !(*this == qoSService);
}

uint16_t QoSService::getInstanceId() const{
    return _serviceIdentifier.getInstanceId();
}

size_t QoSService::getFramesize() const {
    return _framesize;
}

uint16_t QoSService::getIntervalFrames() const {
    return _intervalFrames;
}

CoRE4INET::SR_CLASS QoSService::getSrClass() const {
    return _srClass;
}

int QoSService::getStreamId() const {
    return _streamId;
}

} /* namespace SOQoSMW */
