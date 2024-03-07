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

#ifndef __SOA4CORE_APPLICATIONS_BASE_SERVICEBASE_H_
#define __SOA4CORE_APPLICATIONS_BASE_SERVICEBASE_H_

#include "soa4core/manager/base/IManager.h"
//STD
#include <string>
//INET
#include <inet/networklayer/common/L3Address.h>
//OMNETPP
#include <omnetpp.h>

/**
 * forward declarations
 */
namespace SOA4CoRE {
class ConnectorBase;
} /* namespace SOA4CoRE */

using namespace omnetpp;

namespace SOA4CoRE {

/**
 * @brief Base class for a Middleware-Application.
 *
 * @ingroup soa4core/applications
 *
 * @author Timo Haeckel and Mehmet Mueller for HAW Hamburg
 */
class ServiceBase: public virtual cSimpleModule {
/**
 * Methods
 */
public:
    virtual ~ServiceBase();

    /**
     * Indicated that a Service App is enabled
     *
     * @return true when enabled, otherwise false
     */
    bool isEnabled();

    /**
     * Returns the service name
     * @return the service name
     */
    std::string getServiceName();

    /**
     * Returns the service ID
     * @return the service ID
     */
    int getServiceId() const;

    /**
     * Returns the IP address
     * @return the IP address
     */
    inet::L3Address getAddress();

    /**
     * Returns the instance ID
     * @return the instance ID
     */
    uint16_t getInstanceId();

    /**
     * Returns the TCP port
     * @return the TCP port
     */
    int getTcpPort();

    /**
     * Returns the UDP port
     * @return the UDP port
     */
    int getUdpPort();

    /**
     * Returns the deadline
     * @return the deadline
     */
    double getDeadline();

protected:
    /**
     * Initialization of the module.
     */
    virtual void initialize() override;

    /**
     * This method should be called from subclasses unless the module
     * resets the bag on its own.
     *
     * @param msg Parameter must be forwarded from subclass
     */
    virtual void handleMessage(cMessage *msg) override;

    /**
     * Indicates a parameter has changed.
     *
     * @param parname Name of the changed parameter or nullptr if multiple parameter changed.
     *
     * @throws cRuntimeError When needed parameters could not be found.
     */
    virtual void handleParameterChange(const char* parname) override;

    /**
     * Interface method to handle service start up of publisher/subscriber
     */
    virtual void handleStart() {};

private:

    /**
     * Member variables
     */
public:
protected:

    /**
     * Reference to the Manager.
     */
    IManager* _localServiceManager;

    /**
     * Reference to the Connector module to the endpoints of this service.
     */
    ConnectorBase* _connector;

    /**
     * TCP port used for TCP endpoints
     */
    int _tcpPort;

    /**
     * UDP port used for UDP endpoints
     */
    int _udpPort;

    /**
     * The local address.
     */
    std::string _localAddress;

    /**
     * Name of the service.
     */
    std::string _serviceName;

    /**
      * Id of the publishing service to subscribe to.
      */
    int _serviceId;

    /**
     * Instance Id of the service
     */
    uint16_t _instanceId;

    /**
     * Caches enabled parameter
     */
    bool _enabled;

    /**
     * Caches the start time parameter
     */
    double _startTime;

    /**
     * Caches the deadline parameter
     */
    double _deadline;

private:
};

} /* end namespace SOA4CoRE */

#endif
