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
    /**
     * Methods
     */
public:
    QoSService(int serviceId, inet::L3Address address, int port, uint16_t instanceId, QoSPolicyMap qosPolicyMap = QoSPolicyMap());
    virtual ~QoSService();

    /**
     * Returns the QoS Policy Map
     * @return the QoS Policy Map
     */
    QoSPolicyMap getQoSPolicyMap();

    /**
     * Returns the instance ID
     * @return the instance ID
     */
    uint16_t getInstanceId() const;
protected:
private:

    /**
     * Member variables
     */
public:
protected:
    /**
     * The QoS Policy Map
     */
    QoSPolicyMap _qosPolicyMap;
private:
    uint16_t _instanceId;
};

} /* namespace SOQoSMW */

#endif /* SOQOSMW_SERVICE_QOSSERVICE_QOSSERVICE_H_ */
