/*
 * ServiceBase.cc
 *
 *  Created on: Apr 8, 2021
 *      Author: mehkir
 */

#include <soqosmw/service/base/ServiceBase.h>
#include <iostream>

namespace SOQoSMW {

ServiceBase::ServiceBase(std::string serviceName, int serviceId, inet::L3Address address, int port):
        _serviceIdentifier(ServiceIdentifier(serviceId, serviceName)), _address(address), _port(port) {
}

ServiceBase::~ServiceBase() {
    // TODO Auto-generated destructor stub
}

std::string ServiceBase::getServiceName() const {
    return _serviceIdentifier.getServiceName();
}

int ServiceBase::getServiceId() const {
    return _serviceIdentifier.getServiceId();
}

inet::L3Address ServiceBase::getAddress() const {
    return _address;
}

int ServiceBase::getPort() const {
    return _port;
}

bool ServiceBase::operator==(const ServiceBase& serviceBase) const {
    return (_serviceIdentifier == serviceBase._serviceIdentifier)
            && _address == serviceBase.getAddress()
            && _port == serviceBase.getPort();
}

bool ServiceBase::operator!=(const ServiceBase& serviceBase) const {
    return !(*this == serviceBase);
}


} /* namespace SOQoSMW */
