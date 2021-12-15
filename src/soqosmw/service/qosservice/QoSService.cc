/*
 * QoSService.cc
 *
 *  Created on: Apr 10, 2021
 *      Author: mehkir
 */

#include <soqosmw/service/qosservice/QoSService.h>

namespace SOQoSMW {

QoSService::QoSService(int serviceId, inet::L3Address address, int port, uint16_t instanceId, QoSPolicyMap qosPolicyMap):
        ServiceBase(serviceId,address,port), _instanceId(instanceId), _qosPolicyMap(qosPolicyMap) {
}

QoSService::~QoSService() {
}

QoSPolicyMap QoSService::getQoSPolicyMap() {
    return _qosPolicyMap;
}

uint16_t QoSService::getInstanceId() const{
    return _instanceId;
}

} /* namespace SOQoSMW */
