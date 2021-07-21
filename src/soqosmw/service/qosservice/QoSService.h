/*
 * QoSService.h
 *
 *  Created on: Apr 10, 2021
 *      Author: mehkir
 */

#ifndef SOQOSMW_SERVICE_QOSSERVICE_QOSSERVICE_H_
#define SOQOSMW_SERVICE_QOSSERVICE_QOSSERVICE_H_

#include "soqosmw/service/base/ServiceBase.h"
#include "soqosmw/qospolicy/base/qospolicy.h"

namespace SOQoSMW {

class QoSService: public virtual ServiceBase {
public:
    QoSService(int serviceId, inet::L3Address address, int port, QoSPolicyMap qosPolicyMap);
    virtual ~QoSService();

    QoSPolicyMap getQoSPolicyMap();

protected:
    QoSPolicyMap _qosPolicyMap;
};

} /* namespace SOQoSMW */

#endif /* SOQOSMW_SERVICE_QOSSERVICE_QOSSERVICE_H_ */
