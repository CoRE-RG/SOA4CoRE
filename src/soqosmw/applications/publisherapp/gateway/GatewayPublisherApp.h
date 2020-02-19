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

#ifndef __SOQOSMW_APPLICATIONS_PUBLISHERAPP_GATEWAY_GWSOURCEAPPBASE_H_
#define __SOQOSMW_APPLICATIONS_PUBLISHERAPP_GATEWAY_GWSOURCEAPPBASE_H_

#include "soqosmw/applications/publisherapp/base/PublisherAppBase.h"
#include "inet/common/InitStages.h"
//STD
#include <vector>

using namespace omnetpp;

namespace SOQoSMW {
/**
 * @brief A soqosmw gateway publisher application.
 *
 * @ingroup soqosmw/applications
 *
 * @author Timo Haeckel for HAW Hamburg
 */
class GatewayPublisherApp: public virtual PublisherAppBase {
private:

    /**
     * Caches the canIDs handled in this gateway app
     */
    std::vector<int> _canIds;

public:
    GatewayPublisherApp();

    virtual ~GatewayPublisherApp();

protected:

    /**
     * Initialization of the module. Sends activator message
     */
    virtual void initialize(int stage) override;
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

    virtual void scheduleNextMessage() override;

private:
};

} /* end namespace SOQoSMW */

#endif
