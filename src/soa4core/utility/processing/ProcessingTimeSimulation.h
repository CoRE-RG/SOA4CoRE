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

#ifndef __SDN4CORE_PROCESSINGTIMESIMULATION_H_
#define __SDN4CORE_PROCESSINGTIMESIMULATION_H_

//OMNETPP
#include <omnetpp.h>

using namespace omnetpp;

namespace SOA4CoRE {

/**
 * @brief Schedules Processing for incoming messages two for a later time slot.
 * Only allows the processing of one message at the time.
 * Other incoming messages wait in a queue.
 *
 * @ingroup soa4core/utility
 *
 * @author Timo Haeckel for HAW Hamburg
 */
class ProcessingTimeSimulation: public cSimpleModule {
public:
    virtual ~ProcessingTimeSimulation();

protected:
    /**
     * Initializes the module
     */
    virtual void initialize() override;

    /**
     * Handles the given message
     * @param msg the given message
     */
    virtual void handleMessage(cMessage *msg) override;

    /**
     * Indicates a parameter has changed.
     *
     * @param parname Name of the changed parameter or nullptr if multiple parameter changed.
     */
    virtual void handleParameterChange(const char *parname) override;

    /**
     * Schedules the message at a later time according to _processingTime parameter.
     * After the processing time the message will be forwarded to processMessage().
     * @param msg   The msg to schedule.
     */
    virtual void simulateProcessingTime(cMessage* msg);

    /**
     * Triggers the next processing time simulation. Resets the busy status if msgList
     * is empty, otherwise schedules next message in processing time.
     */
    virtual void triggerNextProcessingTime();

    /**
     * Processes the scheduled message. Needs to delete the message after handling it.
     * @param msg  the incoming message
     */
    virtual void processScheduledMessage(cMessage *msg) = 0;

    /**
     * Cached parameter processing time.
     */
    double _processingTime;

    /**
     * Caches the allowParallelProcessing parameter.
     * allow parallel processing so new incoming
     * messages will be scheduled in processing time.
     */
    bool _allowParallelProcessing;

    /**
     * Incoming message list to simulate processing time.
     */
    std::list<cMessage*> _msgList;

    /**
     * Indicates if a message is currently scheduled.
     */
    bool _busy;

    //stats
    simsignal_t _queueSize;
    simsignal_t _waitingTime;
};

} /*end namespace SOA4CoRE*/

#endif
