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

#ifndef __SOA4CORE_APPLICATIONS_SUBSCRIBERAPP_GATEWAY_GWSINKAPPBASE_H_
#define __SOA4CORE_APPLICATIONS_SUBSCRIBERAPP_GATEWAY_GWSINKAPPBASE_H_

#include "soa4core/applications/subscriberapp/base/SubscriberAppBase.h"

using namespace omnetpp;

namespace SOA4CoRE {

/**
 * @brief Base class for a middleware publisher application.
 *
 * @ingroup soa4core/applications
 *
 * @author Timo Haeckel for HAW Hamburg
 */
class GatewaySubscriberApp: public virtual SubscriberAppBase {
private:

    cGate* _toGateway;

protected:
    /**
     * Initialization of the module. Sends activator message
     */
    virtual void initialize() override;

    /**
     * This method should be called from subclasses unless the module
     * resets the bag on its own.
     *
     * @param msg Parameter must be forwarded from subclass
     */
    virtual void handleMessage(cMessage *msg) override;

public:
    GatewaySubscriberApp();
    virtual ~GatewaySubscriberApp();

private:
};

}/* end namespace SOA4CoRE */

#endif
