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

#ifndef __SOA4CORE_APPLICATIONS_SUBSCRIBERAPP_BASE_SUBSCRIBERAPPBASE_H_
#define __SOA4CORE_APPLICATIONS_SUBSCRIBERAPP_BASE_SUBSCRIBERAPPBASE_H_

#include "soa4core/applications/base/MiddlewareApplicationBase.h"
#include "soa4core/messages/qosnegotiation/QoSNegotiationProtocol_m.h"
#include "soa4core/service/subscriberapplicationinformation/SubscriberApplicationInformation.h"

//STD
#include <string>
#include <unordered_map>

using namespace omnetpp;

namespace SOA4CoRE {

/**
 * @brief Base class for a middleware subscriber application
 *
 * @ingroup soa4core/applications
 *
 * @author Timo Haeckel and Mehmet Cakir for HAW Hamburg
 */
class SubscriberAppBase: public virtual MiddlewareApplicationBase {
private:


    /**
     * Caches the start time parameter
     */
    double _startTime;

protected:

    /**
     * The application informations of this app
     */
    SubscriberApplicationInformation _subscriberApplicationInformation;

    /**
     * Caches the required QoS type
     */
    QoSGroup _qosGroup;

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

    /**
     * Indicates a parameter has changed.
     *
     * @param parname Name of the changed parameter or nullptr if multiple parameter changed.
     */
    virtual void handleParameterChange(const char* parname) override;

    /**
     * Signal that is emitted every time a frame was sent.
     */
     simsignal_t _rxPkSignal;

     void setQoS();

public:
    SubscriberAppBase();
    virtual ~SubscriberAppBase();

private:

};

}/* end namespace SOA4CoRE */

#endif
