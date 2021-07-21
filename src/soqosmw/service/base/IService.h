/*
 * IService.h
 *
 *  Created on: Apr 1, 2021
 *      Author: mehkir
 */

#ifndef SOQOSMW_SERVICE_BASE_ISERVICE_H_
#define SOQOSMW_SERVICE_BASE_ISERVICE_H_

#include "soqosmw/service/base/IServiceIdentifier.h"
#include "inet/networklayer/common/L3Address.h"
#include <string>

namespace SOQoSMW {

class IService : public cObject {
  public:
    virtual ~IService();
    virtual int getServiceId() const = 0;
    virtual inet::L3Address getAddress() const = 0;
    virtual int getPort() const = 0;
};

} /* namespace SOQoSMW */

#endif /* SOQOSMW_SERVICE_BASE_ISERVICE_H_ */
