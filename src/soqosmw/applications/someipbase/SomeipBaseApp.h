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


class SomeipBaseApp: public inet::UDPBasicApp {
public:
    SomeipBaseApp();
    virtual ~SomeipBaseApp();
protected:
    virtual void initialize(int stage) override;
    virtual void handleMessage(omnetpp::cMessage *msg) override;
    virtual void handleMessageWhenUp(cMessage *msg) override;
    virtual void finish() override;
    virtual void refreshDisplay() const override;

    virtual void sendPacket();
    virtual void processPacket(cPacket *msg);

    virtual void processStart();
    virtual void processSend();

    virtual bool handleNodeStart(inet::IDoneCallback *doneCallback) override;
private:
    void scheduleSelfMsg(omnetpp::simtime_t scheduleTime);
    bool initialMsg;
};

#endif /* SOQOSMW_APPLICATIONS_SOMEIPBASE_SOMEIPBASEAPP_H_ */
