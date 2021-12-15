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

#ifndef SOQOSMW_SERVICE_QOSSERVICEIDENTIFIER_QOSSERVICEIDENTIFIER_H_
#define SOQOSMW_SERVICE_QOSSERVICEIDENTIFIER_QOSSERVICEIDENTIFIER_H_

#include "soqosmw/service/serviceidentifier/ServiceIdentifier.h"

namespace SOQoSMW {

class QoSServiceIdentifier: public ServiceIdentifier {
public:
    QoSServiceIdentifier(int serviceId, int instanceId);
    virtual ~QoSServiceIdentifier();
    virtual bool operator==(const QoSServiceIdentifier& qosServiceIdentifier) const;
    virtual bool operator!=(const QoSServiceIdentifier& qosServiceIdentifier) const;
    int getInstanceId() const;

protected:
    int _instanceId;
};
} /* namespace SOQoSMW */

#endif /* SOQOSMW_SERVICE_QOSSERVICEIDENTIFIER_QOSSERVICEIDENTIFIER_H_ */
