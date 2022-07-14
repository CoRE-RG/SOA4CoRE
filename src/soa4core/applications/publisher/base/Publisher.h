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

#ifndef __SOA4CORE_APPLICATIONS_PUBLISHERA_BASE_PUBLISHER_H_
#define __SOA4CORE_APPLICATIONS_PUBLISHERA_BASE_PUBLISHER_H_

#include "soa4core/applications/base/ServiceBase.h"
#include "soa4core/messages/qosnegotiation/QoSNegotiationProtocol_m.h"
//CORE4INET
#include <core4inet/base/avb/AVBDefs.h>
//STD
#include <cstdint>
#include <string>
#include <unordered_map>

using namespace omnetpp;

namespace SOA4CoRE {

#define START_MSG_NAME "Start Message"
#define SEND_MSG_NAME "Send Message"

/**
 * @brief Base class for a publisher application.
 *
 * @ingroup soa4core/applications
 *
 * @author Timo Haeckel and Mehmet Mueller for HAW Hamburg
 */
class Publisher: public virtual ServiceBase {

/**
 * Methods
 */
public:
    Publisher();

    virtual ~Publisher();

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

    /**
     * Returns the QoS groups
     * @return the QoS groups
     */
    std::set<QoSGroup> getQoSGroups();

    /**
     * Returns the stream ID
     * @return the stream ID
     */
    unsigned long getStreamId();

    /**
     * Returns the AVB SR class
     * @return the AVB SR class
     */
    CoRE4INET::SR_CLASS getSrClass();

    /**
     * Returns the the ethernet frame size
     * @return the ethernet frame size
     */
    size_t getFramesize();

    /**
     * Returns the interval frames
     * @return the interval frames
     */
    int getIntervalFrames();

    /**
     * Returns the vlan id
     * @return the vlan id
     */
    uint16_t getVlanId() const {
        return _vlan_id;
    }

    /**
     * Returns the vlan pcp
     * @return the vlan pcp
     */
    uint8_t getPcp() const {
        return _pcp;
    }

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
     * Prints the QoS groups
     */
    void printQoS();

    /**
     * Creates and registers this Publisher
     */
    void createPublisherWithQoS();

    /**
     * Schedules the next message to be published
     */
    virtual void scheduleNextMessage();

private:

/**
 * Member variables
 */
public:
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
     * Caches the vlan ID.
     */
    uint16_t _vlan_id;

    /**
     * Caches the vlan pcp.
     */
    uint8_t _pcp;

private:
    /**
     * Signal that is emitted each time the payload is used.
     */
    simsignal_t _sigPayload;

    /**
     * Caches the QoS types this application can serve
     */
    std::set<QoSGroup> _qosGroups;
};

} /* end namespace SOA4CoRE */

#endif
