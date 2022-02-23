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

#ifndef __SOA4CORE_APPLICATIONS_SUBSCRIBER_BASE_SUBSCRIBER_H_
#define __SOA4CORE_APPLICATIONS_SUBSCRIBER_BASE_SUBSCRIBER_H_

#include <soa4core/applicationinformation/subscriber/SubscriberApplicationInformation.h>
#include "soa4core/applications/base/ServiceBase.h"
#include "soa4core/messages/qosnegotiation/QoSNegotiationProtocol_m.h"


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
 * @author Timo Haeckel and Mehmet Mueller for HAW Hamburg
 */
class Subscriber: public virtual ServiceBase {
/**
 * Methods
 */
public:
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

    /**
     * Indicates a parameter has changed.
     *
     * @param parname Name of the changed parameter or nullptr if multiple parameter changed.
     */
    virtual void handleParameterChange(const char* parname) override;

    /**
     * Sets the QoS group this subscriber requests/subscribes
     */
    void setQoS();
private:

/**
 * Member variables
 */
public:
    Subscriber();
    virtual ~Subscriber();
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
     * Signal that is emitted every time a frame was sent.
     */
     simsignal_t _rxPkSignal;

private:
    /**
     * Caches the start time parameter
     */
    double _startTime;
};

}/* end namespace SOA4CoRE */

#endif
