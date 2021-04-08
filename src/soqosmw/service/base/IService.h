/*
 * IService.h
 *
 *  Created on: Apr 1, 2021
 *      Author: mehkir
 */

#ifndef SOQOSMW_SERVICE_BASE_ISERVICE_H_
#define SOQOSMW_SERVICE_BASE_ISERVICE_H_

#include "soqosmw/service/base/IServiceIdentifier.h"
#include <string>

namespace SOQoSMW {

class IService {
  public:
    virtual std::string getServiceName() = 0;
    virtual int getServiceId() = 0;
};

} /* namespace SOQoSMW */

#endif /* SOQOSMW_SERVICE_BASE_ISERVICE_H_ */
