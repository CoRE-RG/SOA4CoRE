/*
 * ServiceIdentifierBase.h
 *
 *  Created on: Apr 8, 2021
 *      Author: mehkir
 */

#ifndef SOQOSMW_SERVICE_BASE_SERVICEIDENTIFIERBASE_H_
#define SOQOSMW_SERVICE_BASE_SERVICEIDENTIFIERBASE_H_

#include "soqosmw/service/base/IServiceIdentifier.h"

namespace SOQoSMW {

class ServiceBase;

class ServiceIdentifierBase: public IServiceIdentifier {
public:
    ServiceIdentifierBase(int serviceId, std::string serviceName);
    virtual ~ServiceIdentifierBase();

protected:
    friend ServiceBase;

    std::string getServiceName();
    int getServiceId();

private:
    int _serviceId;
    std::string _serviceName;
};

} /* namespace SOQoSMW */

#endif /* SOQOSMW_SERVICE_BASE_SERVICEIDENTIFIERBASE_H_ */
