/*
 * ServiceIdentifier.cc
 *
 *  Created on: Apr 8, 2021
 *      Author: mehkir
 */

#include <soqosmw/service/serviceidentifier/ServiceIdentifier.h>

namespace SOQoSMW {

ServiceIdentifier::ServiceIdentifier(int serviceId):
        _serviceId(serviceId) {
}

ServiceIdentifier::~ServiceIdentifier() {
}

int ServiceIdentifier::getServiceId() const {
    return _serviceId;
}

bool ServiceIdentifier::operator==(const ServiceIdentifier& serviceIdentifier) const {
    return this->_serviceId ==  serviceIdentifier.getServiceId();
}

bool ServiceIdentifier::operator!=(const ServiceIdentifier& serviceIdentifier) const {
    return !(*this == serviceIdentifier);
}



} /* namespace SOQoSMW */
