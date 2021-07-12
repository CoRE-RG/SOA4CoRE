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
    ServiceIdentifier(int serviceId, std::string serviceName);
    virtual ~ServiceIdentifier();
    virtual bool operator==(const ServiceIdentifier& serviceIdentifier) const;
    virtual bool operator!=(const ServiceIdentifier& serviceIdentifier) const;
    std::string getServiceName() const;
    int getServiceId() const;

protected:
    int _serviceId;
    std::string _serviceName;
};

} /* namespace SOQoSMW */

namespace std {
    template<>
    struct hash<SOQoSMW::ServiceIdentifier>
    {
        size_t operator()(const SOQoSMW::ServiceIdentifier &serviceIdentifier) const {
            std::hash<int> hashVal;
            return hashVal(serviceIdentifier.getServiceId());
        }
    };
}

#endif /* SOQOSMW_SERVICE_SERVICEIDENTIFIER_SERVICEIDENTIFIER_H_ */
