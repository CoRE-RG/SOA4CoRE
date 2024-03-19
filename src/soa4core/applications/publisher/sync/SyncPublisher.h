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

#ifndef __SOA4CORE_APPLICATIONS_PUBLISHER_SYNC_SYNCPUBLISHER_H_
#define __SOA4CORE_APPLICATIONS_PUBLISHER_SYNC_SYNCPUBLISHER_H_

#include "soa4core/applications/publisher/base/Publisher.h"

//CoRE4INET
#include "core4inet/utilities/classes/Scheduled.h"

using namespace omnetpp;

namespace SOA4CoRE {

/**
 * @brief Synchronous source for a publisher application.
 *
 * @ingroup soa4core/applications
 *
 * @author Timo Haeckel for HAW Hamburg
 */
class SyncPublisher:
        public virtual SOA4CoRE::Publisher,
        public virtual CoRE4INET::Scheduled,
        public virtual cListener
{
/**
 * Methods
 */
public:
    /**
     * Overrides the getter for intervalMin
     * @return the periodLength * modulo in seconds
     */
    virtual double getIntervalMin() override;
protected:
    virtual void initialize() override;
    /**
     * Schedules the next message to be published
     */
    virtual void scheduleNextMessage() override;
    /**
     * Indicates a parameter has changed.
     *
     * @param parname Name of the changed parameter or nullptr if multiple parameter changed.
     */
    virtual void handleParameterChange(const char* parname) override;
    /**
     * @brief handle self messages to send frames
     *
     * @param msg incoming self messages
     */
    virtual void handleMessage(cMessage *msg) override;
    /**
     * Receives signal from sync module
     *
     * @param source signal emitting  component
     * @param signalID ID of the signal
     * @param obj representation of the signal
     */
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj, cObject *details) override;
private:

/**
 * Member variables
 */
public:
protected:
private:
    /**
     * Caches action_time parameter
     */
    double actionTime;
    /**
     * Caches modulo parameter
     */
    unsigned int modulo;
    /**
     * True when node is synchronized (Frames will be only sent when node runs synchronous)
     */
    bool synchronized = false;
    /**
     * Next scheduled cycle to send a message
     * prevents multiple messages being send in the same cycle, e.g. through clock sync
     * uses the sendInterval to skip cycles if needed.
     */
    uint32_t nextCycle = 0;
    /**
     * Cached scheduler event to be reused
     */
    CoRE4INET::SchedulerActionTimeEvent *event;
};

} /* end namespace SOA4CoRE */

#endif // __SOA4CORE_APPLICATIONS_PUBLISHER_SYNC_SYNCPUBLISHER_H_
