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

#include <soqosmw/service/applicationinformation/ApplicationInformation.h>

namespace SOQoSMW {

ApplicationInformation::ApplicationInformation() {

}

ApplicationInformation::ApplicationInformation(int serviceId, inet::L3Address address, uint16_t instanceId,
        int tcpPort, int udpPort, int streamId, CoRE4INET::SR_CLASS srClass, size_t framesize, uint16_t intervalFrames):
        _serviceId(serviceId), _address(address), _instanceId(instanceId), _tcpPort(tcpPort),
        _udpPort(udpPort), _streamId(streamId), _srClass(srClass), _framesize(framesize), _intervalFrames(intervalFrames) {
}

/*
ApplicationInformation::ApplicationInformation(const ApplicationInformation& applicationInformation) {
    _serviceId = applicationInformation._serviceId;
    _address = applicationInformation._address;
    _instanceId = applicationInformation._instanceId;
    _tcpPort = applicationInformation._tcpPort;
    _udpPort = applicationInformation._udpPort;
    _streamId = applicationInformation._streamId;
    _srClass = applicationInformation._srClass;
    _framesize = applicationInformation._framesize;
    _intervalFrames = applicationInformation._intervalFrames;

}

ApplicationInformation& ApplicationInformation::operator=(const ApplicationInformation& applicationInformation) {
    if(this == &applicationInformation)
        return *this;

    _serviceId = applicationInformation._serviceId;
    _address = applicationInformation._address;
    _instanceId = applicationInformation._instanceId;
    _tcpPort = applicationInformation._tcpPort;
    _udpPort = applicationInformation._udpPort;
    _streamId = applicationInformation._streamId;
    _srClass = applicationInformation._srClass;
    _framesize = applicationInformation._framesize;
    _intervalFrames = applicationInformation._intervalFrames;
    return *this;
}

*/

ApplicationInformation::~ApplicationInformation() {
}

int ApplicationInformation::getServiceId() const {
    return _serviceId;
}

inet::L3Address ApplicationInformation::getAddress() const {
    return _address;
}

int ApplicationInformation::getTCPPort() const {
    return _tcpPort;
}

int ApplicationInformation::getUDPPort() const {
    return _udpPort;
}

bool ApplicationInformation::operator==(const ApplicationInformation& applicationInformation) const {
    return _serviceId == applicationInformation.getServiceId()
            && _address == applicationInformation.getAddress()
            && _instanceId == applicationInformation.getInstanceId()
            && _tcpPort == applicationInformation.getTCPPort()
            && _udpPort == applicationInformation.getUDPPort()
            && _streamId == applicationInformation.getStreamId()
            && _srClass == applicationInformation.getSrClass()
            && _framesize == applicationInformation.getFramesize()
            && _intervalFrames == applicationInformation.getIntervalFrames();
}

bool ApplicationInformation::operator!=(const ApplicationInformation& applicationInformation) const {
    return !(*this == applicationInformation);
}

uint16_t ApplicationInformation::getInstanceId() const{
    return _instanceId;
}

size_t ApplicationInformation::getFramesize() const {
    return _framesize;
}

uint16_t ApplicationInformation::getIntervalFrames() const {
    return _intervalFrames;
}

CoRE4INET::SR_CLASS ApplicationInformation::getSrClass() const {
    return _srClass;
}

int ApplicationInformation::getStreamId() const {
    return _streamId;
}



} /* namespace SOQoSMW */
