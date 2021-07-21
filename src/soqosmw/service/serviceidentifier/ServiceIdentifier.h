/*
 * ServiceIdentifierBase.h
 *
 *  Created on: Apr 8, 2021
 *      Author: mehkir
 */

#ifndef SOQOSMW_SERVICE_SERVICEIDENTIFIER_SERVICEIDENTIFIER_H_
#define SOQOSMW_SERVICE_SERVICEIDENTIFIER_SERVICEIDENTIFIER_H_

#include "soqosmw/service/base/IServiceIdentifier.h"

namespace SOQoSMW {

class ServiceBase;

class ServiceIdentifier: public virtual IServiceIdentifier {
public:
    ServiceIdentifier(int serviceId);
    virtual ~ServiceIdentifier();
    virtual bool operator==(const ServiceIdentifier& serviceIdentifier) const;
    virtual bool operator!=(const ServiceIdentifier& serviceIdentifier) const;
    int getServiceId() const;

protected:
    int _serviceId;
};

} /* namespace SOQoSMW */

#endif /* SOQOSMW_SERVICE_SERVICEIDENTIFIER_SERVICEIDENTIFIER_H_ */
