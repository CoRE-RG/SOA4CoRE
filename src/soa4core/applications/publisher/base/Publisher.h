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
     * Returns the maximum payload to calculate framesizes
     * @return maximum payload bytes
     */
    size_t getPayloadMax();

    /**
     * Returns the interval frames
     * @return the interval frames
     */
    int getIntervalFrames();

    /**
     * Returns the minimum interval between two consecutive frames
     * @return the interval in seconds
     */
    virtual double getIntervalMin();

    /**
     * Returns the vlan id
     * @return the vlan id
     */
    int getVlanId() const {
        return _vlan_id;
    }

    /**
     * Returns the vlan pcp
     * @return the vlan pcp
     */
    int getPcp() const {
        return _pcp;
    }

    /**
     * Returns the multicast destination address
     * @return the multicast destination address
     */
    const std::string getMcastDestAddr() const {
        return _mcastDestAddr;
    }

    /**
     * Returns the multicast destination port
     * @return the multicast destination port
     */
    int getMcastDestPort() const {
        return _mcastDestPort;
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
     * Interface method to handle service start up of publisher/subscriber
     * Creates and registers this Publisher
     */
    virtual void handleStart() override;

    /**
     * Create a payload packet and send it to the connector.
     */
    virtual void sendMessage();

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
     * Caches the current payload parameter.
     */
    size_t _payload;

    /**
     * Maximum size of the payload.
     */
    size_t _payloadMax;

    /**
     * The minimum interval between two frames.
     * @note The actual send interval is determined by the volatile interval parameter.
     *       This is used to be forwarded to endpoints or service discovery for optional
     *       ressource reservation mechanisms
     */
    double _intervalMin;

    /**
     * Signal that is emitted each time then a message is sent.
     */
    simsignal_t _msgSentSignal;

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
    int _vlan_id = -1;

    /**
     * Caches the vlan pcp.
     */
    int _pcp = -1;

    /**
     * Caches the multicast IP destination address.
     */
    std::string _mcastDestAddr;

    /**
     * Caches the multicast destination UDP port.
     */
    int _mcastDestPort;

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
