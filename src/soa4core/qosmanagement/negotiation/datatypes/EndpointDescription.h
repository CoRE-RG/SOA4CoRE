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

#ifndef SOA4CORE_QOSMANAGEMENT_NEGOTIATION_DATATYPES_ENDPOINTDESCRIPTION_H_
#define SOA4CORE_QOSMANAGEMENT_NEGOTIATION_DATATYPES_ENDPOINTDESCRIPTION_H_

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
    /**
     * Constructor
     * @param serviceId The service id this endpoints wants to publish/subscribe
     * @param networkAddr The network address of the endpoint
     * @param networkPort The network port of the endpoint
     */
    EndpointDescription(int serviceId, inet::L3Address networkAddr, int networkPort) : _serviceId(serviceId), _networkAddr (networkAddr), _networkPort(networkPort){

    }

    /**
     * Default constructor
     */
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

    /**
     * Returns the network address
     * @return the network address
     */
    inet::L3Address& getNetworkAddr() {
        return _networkAddr;
    }

    /**
     * Returns the network port
     * @return the network port
     */
    int getNetworkPort() {
        return _networkPort;
    }

    /**
     * Returns the service id
     * @return the service id
     */
    int getServiceId() {
        return _serviceId;
    }

    /**
     * Returns the string representation of this endpoint
     * @return the string representation of this endpoint
     */
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

#endif /* SOA4CORE_QOSMANAGEMENT_NEGOTIATION_DATATYPES_ENDPOINTDESCRIPTION_H_ */
