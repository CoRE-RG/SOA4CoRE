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

#ifndef __SOQOSMW_SOMEIPSUBSCRIBER_H_
#define __SOQOSMW_SOMEIPSUBSCRIBER_H_

#include "soqosmw/applications/someipapp/base/SomeipSD.h"
#include "soqosmw/applications/someipapp/base/SomeipAppBase.h"

namespace SOQoSMW {

/**
 * @brief A simple dummy SOME/IP Subscriber application.
 *
 * @ingroup soqosmw/applications
 *
 * @author Mehmet Cakir
 */
class SomeipSubscriber : public virtual SomeipAppBase {
protected:
    /**
     * Schedules a self message and offers service with
     * using the SOME/IP SD protocol.
     *
     * @param stage indicates the initialization stage
     */
    virtual void initialize(int stage) override;

    /**
     * Handles incoming message as soon as node is up and
     * processes the packet
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

private:
    SomeipSD* _someipSD;

    /**
     * Service ID to subscribe
     */
    uint16_t _subscribeServiceID;

    /**
     * Instance ID of service id to subscribe
     */
    uint16_t _instanceID;

public:
    /**
     * Returns the IP Address of this subscriber
     *
     * @param adressType
     * @return local IP Address
     */
    inet::L3Address getIpAddress(inet::L3Address::AddressType adressType);

    /**
     * Returns the port of this subscriber
     *
     * @return local Port
     */
    int getPort();

    /**
     * Returns the ID of the service to subscribe
     *
     * @return service ID to subscribe
     */
    uint16_t getSubscribeServiceID();

    /**
     * Returns the ID of the instance of the service to subscribe
     *
     * @return instance ID to subscribe
     */
    uint16_t getInstanceID();
};
} /* end namespace SOQoSMW */
#endif
