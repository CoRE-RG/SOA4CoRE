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

#ifndef __SOA4CORE_APPLICATIONS_PUBLISHER_GATEWAY_GATEWAYPUBLISHER_H_
#define __SOA4CORE_APPLICATIONS_PUBLISHER_GATEWAY_GATEWAYPUBLISHER_H_

#include <soa4core/applications/publisher/base/Publisher.h>

#include "inet/common/InitStages.h"
//STD
#include <vector>

using namespace omnetpp;

namespace SOA4CoRE {
/**
 * @brief A middleware gateway publisher application.
 *
 * @ingroup soa4core/applications
 *
 * @author Timo Haeckel and Mehmet Mueller for HAW Hamburg
 */
class GatewayPublisher: public virtual Publisher {
/**
 * Methods
 */
public:
    GatewayPublisher();
    virtual ~GatewayPublisher();
protected:
    /**
     * Initialization of the module. Sends activator message
     */
    virtual void initialize(int stage) override;

    /**
     * Returns the maximum number of initialization stages
     * @return the maximum number of initialization stages
     */
    virtual int numInitStages() const override {
        return inet::NUM_INIT_STAGES;
    }

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
     */
    virtual void handleParameterChange(const char* parname) override;

    /**
     * Normally it schedules the next message to be published
     * but not as a gateway
     */
    virtual void scheduleNextMessage() override;
private:

/**
 * Member variables
 */
public:
protected:
private:
    /**
     * Caches the canIDs handled in this gateway app
     */
    std::vector<int> _canIds;
};

} /* end namespace SOA4CoRE */

#endif
