//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef SOQOSMW_SERVICE_SOMEIPSERVICEIDENTIFIER_SOMEIPSERVICEIDENTIFIER_H_
#define SOQOSMW_SERVICE_SOMEIPSERVICEIDENTIFIER_SOMEIPSERVICEIDENTIFIER_H_

#include "soqosmw/service/serviceidentifier/ServiceIdentifier.h"

namespace SOQoSMW {

class SomeIpServiceIdentifier: public ServiceIdentifier {
public:
    SomeIpServiceIdentifier(int serviceId, int instanceId);
    virtual ~SomeIpServiceIdentifier();
    virtual bool operator==(const SomeIpServiceIdentifier& someIpServiceIdentifier) const;
    virtual bool operator!=(const SomeIpServiceIdentifier& someIpServiceIdentifier) const;
    int getInstanceId() const;

protected:
    int _instanceId;
};
} /* namespace SOQoSMW */

#endif /* SOQOSMW_SERVICE_SOMEIPSERVICEIDENTIFIER_SOMEIPSERVICEIDENTIFIER_H_ */
