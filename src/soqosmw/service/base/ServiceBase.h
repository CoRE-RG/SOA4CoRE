/*
 * ServiceBase.h
 *
 *  Created on: Apr 8, 2021
 *      Author: mehkir
 */

#ifndef SOQOSMW_SERVICE_BASE_SERVICEBASE_H_
#define SOQOSMW_SERVICE_BASE_SERVICEBASE_H_

#include <string>
#include "soqosmw/service/base/IService.h"
#include "soqosmw/service/base/ServiceIdentifierBase.h"

namespace SOQoSMW {

class ServiceBase: public IService{
public:
    ServiceBase(std::string name, int id);
    virtual ~ServiceBase();

    std::string getServiceName();
    int getServiceId();

private:
    ServiceIdentifierBase _serviceIdentifierBase;
};

} /* namespace SOQoSMW */

#endif /* SOQOSMW_SERVICE_BASE_SERVICEBASE_H_ */
