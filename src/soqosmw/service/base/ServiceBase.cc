/*
 * ServiceBase.cc
 *
 *  Created on: Apr 8, 2021
 *      Author: mehkir
 */

#include <soqosmw/service/base/ServiceBase.h>
#include <iostream>

namespace SOQoSMW {

ServiceBase::ServiceBase(std::string name, int id): _serviceIdentifierBase(ServiceIdentifierBase(id, name)){
}

ServiceBase::~ServiceBase() {
    // TODO Auto-generated destructor stub
}

std::string ServiceBase::getServiceName() {
    return _serviceIdentifierBase.getServiceName();
}

int ServiceBase::getServiceId() {
    return _serviceIdentifierBase.getServiceId();
}

} /* namespace SOQoSMW */
