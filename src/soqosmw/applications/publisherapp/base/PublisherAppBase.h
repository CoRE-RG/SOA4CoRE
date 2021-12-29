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

#ifndef __SOQOSMW_APPLICATIONS_PUBLISHERAPP_BASE_PUBLISHERAPP_H_
#define __SOQOSMW_APPLICATIONS_PUBLISHERAPP_BASE_PUBLISHERAPP_H_

#include "soqosmw/applications/base/SOQoSMWApplicationBase.h"
#include "soqosmw/qospolicy/base/qospolicy.h"
#include "soqosmw/service/qosservice/QoSService.h"
//CoRE4INET
#include "core4inet/base/avb/AVBDefs.h"
//STD
#include <cstdint>
#include <string>
#include <unordered_map>

namespace SOQoSMW {
class PublisherWriter;
} /* namespace SOQoSMW */

using namespace omnetpp;

namespace SOQoSMW {

#define START_MSG_NAME "Start Message"
#define SEND_MSG_NAME "Send Message"

/**
 * @brief Base class for a soqosmw publisher application.
 *
 * @ingroup soqosmw/applications
 *
 * @author Timo Haeckel and Mehmet Cakir for HAW Hamburg
 */
class PublisherAppBase: public virtual SOQoSMWApplicationBase {

public:
    PublisherAppBase();

    virtual ~PublisherAppBase();

    /**
     * Indicated that PublisherApp is enabled
     *
     * @return true when enabled, otherwise false
     */
    bool isEnabled();

    /**
     * Returns the number of bytes of the payload desired
     *
     * @return Size of payload in bytes
     */
    size_t getPayloadBytes();

protected:
    /**
     * Caches enabled parameter
     */
    bool _enabled;

    /**
     * Caches payload parameter
     */
    size_t _payload;

    /**
     * size of the ethernet frame calculated from the payload.
     */
    size_t _framesize;

    /**
     * Signal that is emitted each time then a message is sent.
     */
    simsignal_t _msgSentSignal;

    /**
     * The service representations of this app
     */
    std::vector<QoSService> _qosServices;

    /**
     * Caches the start time parameter
     */
    double _startTime;

    /**
     * Caches the interval length parameter
     */
    double _interval;

    /**
     * Caches the number of Messages per Interval parameter.
     */
    int _intervalFrames;

    /**
     * Caches the AVB SR Class.
     */
    CoRE4INET::SR_CLASS _srClass;

    /**
     * Caches the stream ID.
     */
    unsigned long _streamID;

    /**
     * Caches the QoS types this application can serve
     */
    std::vector<std::string> _qosGroups;

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

    void setQoS();
    void printQoS();
    void createPublisherWithQoS();
    virtual void scheduleNextMessage();

private:
    /**
     * Signal that is emitted each time the payload is used.
     */
    simsignal_t _sigPayload;
};

} /* end namespace SOQoSMW */

#endif
