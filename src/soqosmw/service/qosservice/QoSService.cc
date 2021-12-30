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
#include <algorithm>

namespace SOQoSMW {

QoSService::QoSService() : _serviceIdentifier(QoSServiceIdentifier(-1,-1)), _defaultQoSService(true){

}

QoSService::QoSService(int serviceId, inet::L3Address address, uint16_t instanceId, std::set<QoSGroups> qosGroups,
        int tcpPort, int udpPort, int streamId, CoRE4INET::SR_CLASS srClass, size_t framesize, uint16_t intervalFrames):
        _serviceIdentifier(QoSServiceIdentifier(serviceId,instanceId)), _address(address), _qosGroups(qosGroups),
        _tcpPort(tcpPort), _udpPort(udpPort), _streamId(streamId), _srClass(srClass), _framesize(framesize),
        _intervalFrames(intervalFrames), _defaultQoSService(false){
}

QoSService::~QoSService() {
}

int QoSService::getServiceId() const {
    return _serviceIdentifier.getServiceId();
}

inet::L3Address QoSService::getAddress() const {
    return _address;
}

std::set<QoSGroups> QoSService::getQosGroups() const {
    return _qosGroups;
}

int QoSService::getTCPPort() const {
    return _tcpPort;
}

int QoSService::getUDPPort() const {
    return _udpPort;
}

bool QoSService::operator==(const QoSService& qoSService) const {
    return (_serviceIdentifier == qoSService._serviceIdentifier)
            && _address == qoSService.getAddress()
            && _tcpPort == qoSService.getTCPPort()
            && _udpPort == qoSService.getUDPPort()
            && _streamId == qoSService.getStreamId()
            && _srClass == qoSService.getSrClass()
            && _framesize == qoSService.getFramesize()
            && _intervalFrames == qoSService.getIntervalFrames()
            && _qosGroups == qoSService.getQosGroups();
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

QoSGroups* QoSService::getCommonQoSGroup(QoSService qosService) {
    QoSGroups* commonQoSGroup = nullptr;
    std::set<QoSGroups> result;
    std::set_intersection(_qosGroups.begin(), _qosGroups.end(), qosService.getQosGroups().begin(), qosService.getQosGroups().end(), std::inserter(result, result.begin()));
    if (result.size() > 1) {
        throw cRuntimeError("The subscriber set must not contain more than one QoSGroup");
    }
    if (result.size()) {
        commonQoSGroup = new QoSGroups(*std::next(result.begin(), 1));
    }
    return commonQoSGroup;
}

bool QoSService::containsQoSGroup(QoSGroups qosGroup) {
    return std::find(_qosGroups.begin(), _qosGroups.end(), qosGroup) != _qosGroups.end();
}

} /* namespace SOQoSMW */
