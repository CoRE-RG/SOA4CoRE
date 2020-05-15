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

#ifndef SOQOSMW_APPLICATIONS_SOMEIPAPP_TICTOC_TICTOCAPP_H_
#define SOQOSMW_APPLICATIONS_SOMEIPAPP_TICTOC_TICTOCAPP_H_

#include <soqosmw/applications/someipapp/base/SomeIpAppBase.h>

namespace SOQoSMW {

/**
 * @brief A SOME/IP TicToc application.
 *
 * @ingroup soqosmw/applications
 *
 * @author Mehmet Cakir
 */
class TicTocApp: public virtual SomeIpAppBase {
public:
    TicTocApp();
    virtual ~TicTocApp();
protected:
    /**
     * Schedules a self message
     *
     * @param stage indicates the initialization stage
     */
    virtual void initialize(int stage) override;

    /**
     * Handles incoming message as soon as node is up and
     * responds with a new message
     *
     * @param msg
     */
    virtual void handleMessageWhenUp(cMessage *msg) override;

    /**
     * Processes a packet
     *
     * @param packet
     */
    virtual void processPacket(cPacket *packet) override;
};
} /* end namespace SOQoSMW */
#endif /* SOQOSMW_APPLICATIONS_SOMEIPAPP_TICTOC_TICTOCAPP_H_ */
