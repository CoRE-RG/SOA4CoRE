/*
 * ServiceIdentifierBase.cc
 *
 *  Created on: Apr 8, 2021
 *      Author: mehkir
 */

#include <soqosmw/service/base/ServiceIdentifierBase.h>

namespace SOQoSMW {

ServiceIdentifierBase::ServiceIdentifierBase(int serviceId, std::string serviceName): _serviceId(serviceId), _serviceName(serviceName) {
}

ServiceIdentifierBase::~ServiceIdentifierBase() {
}

std::string ServiceIdentifierBase::getServiceName() {
    return _serviceName;
}

int ServiceIdentifierBase::getServiceId() {
    return _serviceId;
}

} /* namespace SOQoSMW */
