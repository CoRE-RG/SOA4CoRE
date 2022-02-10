//
// c Timo Haeckel for HAW Hamburg
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

#ifndef SOA4CORE_BASE_ENDPOINTDESCRIPTION_H_
#define SOA4CORE_BASE_ENDPOINTDESCRIPTION_H_

#include <string>

#include <inet/networklayer/common/L3Address.h>

namespace SOA4CoRE {

/**
 * @brief This class describes an Endpoint.
 *
 * @ingroup soa4core/base
 *
 * @author Timo Haeckel and Mehmet Mueller
 */
class EndpointDescription {
public:
    EndpointDescription(int serviceId, inet::L3Address networkAddr, int networkPort) : _serviceId(serviceId), _networkAddr (networkAddr), _networkPort(networkPort){

    }

    EndpointDescription() {
    }

    virtual ~EndpointDescription() {
    }

    /**
     * comparing operators
     */
    inline bool operator==(EndpointDescription& other) {
        bool value = false;
        if (_networkAddr == other.getNetworkAddr()) {
            if (_networkPort == other.getNetworkPort()) {
                if (_serviceId == other.getServiceId()) {
                    value = true;
                }
            }
        }
        return value;
    }

    inline bool operator!= (EndpointDescription& other) {
        return !(*this == other);
    }

    inet::L3Address& getNetworkAddr() {
        return _networkAddr;
    }

    void setNetworkAddr(inet::L3Address& networkAddr) {
        _networkAddr = networkAddr;
    }

    int getNetworkPort() {
        return _networkPort;
    }

    void setNetworkPort(int networkPort) {
        _networkPort = networkPort;
    }

    int getServiceId() {
        return _serviceId;
    }

    void setServiceId(int serviceId) {
        _serviceId = serviceId;
    }

    std::string toString(){
        std::string ret = "EndpointDescription[";
        ret.append(" serviceId:").append(std::to_string(_serviceId));
        ret.append(" networkAddr:").append(_networkAddr.str());
        ret.append(" networkPort").append(std::to_string(_networkPort));
        return ret.append(" ]");
    }

private:
    /**
     * The service id this Endpoint provides.
     */
    int _serviceId;

    /**
     * The network address of the node running the Endpoint.
     */
    inet::L3Address _networkAddr;

    /**
     * The network port of the Endpoint.
     */
    int _networkPort;
};

} /* namespace SOA4CoRE */

#endif /* SOA4CORE_BASE_ENDPOINTDESCRIPTION_H_ */
