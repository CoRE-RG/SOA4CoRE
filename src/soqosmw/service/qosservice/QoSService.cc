/*
 * QoSService.cc
 *
 *  Created on: Apr 10, 2021
 *      Author: mehkir
 */

#include <soqosmw/service/qosservice/QoSService.h>

namespace SOQoSMW {

QoSService::QoSService(int serviceId, inet::L3Address address, int port, QoSPolicyMap qosPolicyMap):
        ServiceBase(serviceId,address,port), _qosPolicyMap(qosPolicyMap) {
    // TODO Auto-generated constructor stub

}

QoSService::~QoSService() {
    // TODO Auto-generated destructor stub
}

QoSPolicyMap QoSService::getQoSPolicyMap() {
    return _qosPolicyMap;
}

} /* namespace SOQoSMW */
