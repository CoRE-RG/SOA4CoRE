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

#include <qospolicy/base/IQoSPolicy.h>
#include <string>
#include <vector>

namespace soqosmw {
class SOQoSMWApplicationBase;
} /* namespace soqosmw */


namespace soqosmw {

/**
 * @brief Endpoint interface. Baseclass of all soqosmw communication endpoints.
 *
 * @ingroup soqosmw/endpoints
 *
 * @author Timo Haeckel
 */
class IEndpoint {
public:
    /**
     * Constructor
     * @param endpointPath The path/name of the endpoint
     * @param qosPolicies The QoS Policies describing the communication behavior of the endpoint.
     * @param owner The executing application.
     */
    IEndpoint(std::string endpointPath, std::vector<IQoSPolicy> qosPolicies, SOQoSMWApplicationBase* owner);
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

    /**
     * Check if an application is the executer of this endpoint
     * @param application The application pointer.
     * @return True if the application is the executer.
     */
    bool isExecutedBy (SOQoSMWApplicationBase* application);

protected:
    /**
     * Get the owning application.
     * @return The executing application.
     */
    const SOQoSMWApplicationBase* getOwner() const {
        return _owner;
    }

private:
    /**
     * The name of the endpoint as a path for better specifying (e.g. "Reifendruck/links").
     */
    std::string _endpointPath;

    /**
     * QoS Policies set for this Endpoint.
     */
    std::vector<IQoSPolicy> _qos;

    /**
     * Owner Module of this Endpoint.
     */
    SOQoSMWApplicationBase* _owner;

};

} /*end namespace soqosmw*/

#endif /* SOQOSMW_ENDPOINTS_BASE_IENDPOINT_H_ */
