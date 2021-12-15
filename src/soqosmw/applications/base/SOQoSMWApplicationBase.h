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

#ifndef __SOQOSMW_APPLICATIONS_BASE_SOQOSMWAPPLICATIONBASE_H_
#define __SOQOSMW_APPLICATIONS_BASE_SOQOSMWAPPLICATIONBASE_H_

#include <omnetpp.h>
#include <string>
#include"soqosmw/servicemanager/base/ILocalServiceManager.h"

/**
 * forward declarations
 */
namespace SOQoSMW {
class ConnectorBase;
} /* namespace SOQoSMW */

using namespace omnetpp;

namespace SOQoSMW {

/**
 * @brief Base class for a SOQoSMW-Application.
 *
 * contains the module connection to the LocalServiceManager
 *
 * @ingroup soqosmw/applications
 *
 * @author Timo Haeckel and Mehmet Cakir for HAW Hamburg
 */
class SOQoSMWApplicationBase: public virtual cSimpleModule {
/**
 * Methods
 */
public:
    virtual ~SOQoSMWApplicationBase();

    std::string getServiceName();

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

private:

    /**
     * Member variables
     */
public:
protected:

    /**
     * Reference to the LocalServiceManager.
     */
    ILocalServiceManager* _localServiceManager;

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
     * Name of the subscriber service.
     */
    std::string _subscriberName;

    /**
     * Name of the service to publish.
     */
    std::string _publisherName;

    /**
      * Id of the publishing service to subscribe to.
      */
    int _publisherServiceId;

    /**
     * Instance Id of the service
     */
    uint16_t _instanceId;

private:
};

} /* end namespace SOQoSMW */

#endif
