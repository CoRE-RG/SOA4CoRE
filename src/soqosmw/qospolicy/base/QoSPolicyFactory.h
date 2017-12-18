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

#ifndef SOQOSMW_QOSPOLICY_BASE_QOSPOLICYFACTORY_H_
#define SOQOSMW_QOSPOLICY_BASE_QOSPOLICYFACTORY_H_

#include <qospolicy/base/IQoSPolicy.h>
#include <string>
#include <unordered_map>

namespace omnetpp {
class cPar;
} /* namespace omnetpp */

namespace soqosmw {

class QoSPolicyFactory {
public:
    QoSPolicyFactory();
    virtual ~QoSPolicyFactory();

    static std::unordered_map<std::string, IQoSPolicy>& extractPolicyFromPar(omnetpp::cPar& parameter);
};

} /* end namepsace soqosmw */

#endif /* SOQOSMW_QOSPOLICY_BASE_QOSPOLICYFACTORY_H_ */
