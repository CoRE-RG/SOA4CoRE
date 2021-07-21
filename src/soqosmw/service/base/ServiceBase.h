/*
 * ServiceBase.h
 *
 *  Created on: Apr 8, 2021
 *      Author: mehkir
 */

#ifndef SOQOSMW_SERVICE_BASE_SERVICEBASE_H_
#define SOQOSMW_SERVICE_BASE_SERVICEBASE_H_

#include <soqosmw/service/serviceidentifier/ServiceIdentifier.h>
#include <string>
#include "soqosmw/service/base/IService.h"

namespace SOQoSMW {

class ServiceBase: public virtual IService{
public:
    ServiceBase(int serviceId, inet::L3Address address, int port);
    virtual ~ServiceBase();

    int getServiceId() const;
    inet::L3Address getAddress() const;
    int getPort() const;

    virtual bool operator==(const ServiceBase& serviceBase) const;
    virtual bool operator!=(const ServiceBase& serviceBase) const;

protected:
    ServiceIdentifier _serviceIdentifier;
    inet::L3Address _address;
    int _port;
};

} /* namespace SOQoSMW */

#endif /* SOQOSMW_SERVICE_BASE_SERVICEBASE_H_ */
