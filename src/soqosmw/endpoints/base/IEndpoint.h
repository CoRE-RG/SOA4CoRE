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

#include <string>

namespace omnetpp {
class cMessage;
} /* namespace omnetpp */

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
     * @param executingApplication The executing application.
     */
    IEndpoint(std::string endpointPath);
    virtual ~IEndpoint();

    const std::string& getEndpointPath() const {
        return _endpointPath;
    }

    void setEndpointPath(const std::string& path) {
        _endpointPath = path;
    }

    virtual void notify(omnetpp::cMessage* notification){
    }

protected:
    /**
     * The name of the endpoint as a path for better specifying (e.g. "Reifendruck/links").
     */
    std::string _endpointPath;

};

} /*end namespace soqosmw*/

#endif /* SOQOSMW_ENDPOINTS_BASE_IENDPOINT_H_ */