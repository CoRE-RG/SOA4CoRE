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

#ifndef SOQOSMW_APPLICATIONS_SOMEIPAPP_BASE_SOMEIPAPPBASE_H_
#define SOQOSMW_APPLICATIONS_SOMEIPAPP_BASE_SOMEIPAPPBASE_H_

#include "inet/applications/udpapp/UDPBasicApp.h"
#include "soqosmw/messages/someip/SomeIpHeader_m.h"

namespace SOQoSMW {

/**
 * @brief Base class for a SOME/IP application.
 *
 * @ingroup soqosmw/applications
 *
 * @author Mehmet Cakir
 */
class SomeipAppBase: public virtual inet::UDPBasicApp {
public:
    SomeipAppBase();
    virtual ~SomeipAppBase();
protected:
    /**
     * Initializes parameters
     *
     * @param stage indicates the initialization stage
     */
    virtual void initialize(int stage) override;

    /**
     * Handles incoming message as soon as node is up
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
    virtual void sendPacket(uint16_t serviceID, uint16_t methodID, uint32_t length, uint16_t clientID, uint16_t sessionID,
            uint8_t protocolVersion, uint8_t interfaceVersion, uint8_t messageType, uint8_t returnCode);

    /**
     * Processes a packet
     *
     * @param msg
     */
    virtual void processPacket(cPacket *msg) override;

    /**
     * Setups network connection
     */
    virtual void processStart() override;

    /**
     * Processes the sending of a packet
     */
    virtual void processSend() override;

    /**
     * Will be called after initialize method for additional preparations
     * before the node starts.
     *
     * @param doneCallback
     * @return
     */
    virtual bool handleNodeStart(inet::IDoneCallback *doneCallback) override;
protected:
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
} /* end namespace SOQoSMW */
#endif /* SOQOSMW_APPLICATIONS_SOMEIPAPP_BASE_SOMEIPAPPBASE_H_ */
