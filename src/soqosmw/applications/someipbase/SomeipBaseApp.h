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

#ifndef SOQOSMW_APPLICATIONS_SOMEIPBASE_SOMEIPBASEAPP_H_
#define SOQOSMW_APPLICATIONS_SOMEIPBASE_SOMEIPBASEAPP_H_

#include "inet/applications/udpapp/UDPBasicApp.h"

namespace SOQoSMW {

/**
 * @brief Base class for a soqosmw publisher application.
 *
 * @ingroup soqosmw/applications
 *
 * @author Mehmet Cakir for HAW Hamburg
 */
class SomeipBaseApp: public inet::UDPBasicApp {
public:
    SomeipBaseApp();
    virtual ~SomeipBaseApp();
protected:
    /**
     * Initializes parameters and schedules a selfmsg
     *
     * @param stage indicates the initialization stage
     */
    virtual void initialize(int stage) override;

    /**
     * Method how incoming messages will be handled
     *
     * @param msg is the incoming message
     */
    virtual void handleMessage(omnetpp::cMessage *msg) override;

    //TODO Unclear when this method will be called
    /**
     *
     * @param msg
     */
    virtual void handleMessageWhenUp(cMessage *msg) override;

    /**
     * Will be called after the simulation finishes
     */
    virtual void finish() override;

    /**
     * Will be called while the graphical simulation is running.
     * Additional visualization updates can be implemented.
     */
    virtual void refreshDisplay() const override;

    /**
     * Sends a SOME/IP packet
     */
    virtual void sendPacket() override;

    //TODO Unclear when this method will be called
    /**
     *
     * @param msg
     */
    virtual void processPacket(cPacket *msg) override;

    /**
     * Setups network connection
     */
    virtual void processStart() override;

    //TODO Unclear when this method will be called
    /**
     *
     */
    virtual void processSend() override;

    //TODO Unclear when this method will be called
    /**
     *
     * @param doneCallback
     * @return
     */
    virtual bool handleNodeStart(inet::IDoneCallback *doneCallback) override;
private:
    /**
     * Schedules a self message
     * @param scheduleTime the time a self message will be scheduled for
     */
    void scheduleSelfMsg(omnetpp::simtime_t scheduleTime);

    /**
     * Caches if the node sends the initial message
     */
    bool initialMsg;
};
}
#endif /* SOQOSMW_APPLICATIONS_SOMEIPBASE_SOMEIPBASEAPP_H_ */
