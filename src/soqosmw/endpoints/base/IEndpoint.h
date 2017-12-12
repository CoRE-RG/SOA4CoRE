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

#ifndef SOQOSMW_ENDPOINTS_BASE_IENDPOINT_H_
#define SOQOSMW_ENDPOINTS_BASE_IENDPOINT_H_

//STD
#include <string>
#include <vector>

//SOQOSMW
#include <soqosmw/qospolicy/base/IQoSPolicy.h>

namespace soqosmw {

class IEndpoint {
public:
    IEndpoint(std::string endpointPath, std::vector<IQoSPolicy> qosPolicies);
    virtual ~IEndpoint();

    const std::string& getEndpointPath() const {
        return _endpointPath;
    }

    void setEndpointPath(const std::string& path) {
        _endpointPath = path;
    }

    const std::vector<IQoSPolicy>& getQos() const {
        return _qos;
    }

    void setQos(const std::vector<IQoSPolicy>& qos) {
        _qos = qos;
    }

//    virtual bool mathes(std::string& path, std::vector<IQoSPolicy>& qos);

private:
    std::string _endpointPath;
    std::vector<IQoSPolicy> _qos;
};

} /*end namespace soqosmw*/

#endif /* SOQOSMW_ENDPOINTS_BASE_IENDPOINT_H_ */
