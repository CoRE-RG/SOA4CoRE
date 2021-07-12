/*
 * ServiceIdentifier.cc
 *
 *  Created on: Apr 8, 2021
 *      Author: mehkir
 */

#include <soqosmw/service/serviceidentifier/ServiceIdentifier.h>

namespace SOQoSMW {

ServiceIdentifier::ServiceIdentifier(int serviceId, std::string serviceName):
        _serviceId(serviceId), _serviceName(serviceName) {
}

ServiceIdentifier::~ServiceIdentifier() {
}

std::string ServiceIdentifier::getServiceName() const {
    return _serviceName;
}

int ServiceIdentifier::getServiceId() const {
    return _serviceId;
}

bool ServiceIdentifier::operator==(const ServiceIdentifier& serviceIdentifier) const {
    return this->_serviceName == serviceIdentifier.getServiceName()
            && this->_serviceId ==  serviceIdentifier.getServiceId();
}

bool ServiceIdentifier::operator!=(const ServiceIdentifier& serviceIdentifier) const {
    return !(*this == serviceIdentifier);
}



} /* namespace SOQoSMW */
