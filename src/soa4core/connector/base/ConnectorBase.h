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

#ifndef __SOA4CORE_CONNECTORBASE_H_
#define __SOA4CORE_CONNECTORBASE_H_

//STD
#include <mutex>
//OMNETPP
#include <omnetpp.h>


using namespace omnetpp;

namespace SOA4CoRE {

#define PROCESSINGDELAY_MSG_NAME "Processing Delay"

/**
 * @brief The ConnectorBase provides a common interface for all connector modules.
 * Those modules are created by the Manager module (@see~Manager)
 * during runtime to connect a service application to its protocol specific service endpoints.
 *
 * @ingroup soa4core/connector
 *
 * @author Timo Haeckel and Mehmet Mueller for HAW Hamburg
 */
class ConnectorBase : public cSimpleModule
{
/**
 * Methods
 */
public:
protected:
    /**
     * Initializes the module
     */
    virtual void initialize() override;

    /**
     * Indicates a parameter has changed.
     *
     * @param parname Name of the changed parameter or nullptr if multiple parameter changed.
     */
    virtual void handleParameterChange(const char* parname) override;
private:

/**
 * Member variables
 */
public:
protected:
    /**
     * Caches if connector mapping creation after simulation is enabled.
     */
    bool _createConnectorMappingEnabled;

    /**
     * Signal to emit messages which are dropped
     */
    simsignal_t _messageDroppedSignal;
private:
};

} /*end namespace SOA4CoRE*/

#endif
