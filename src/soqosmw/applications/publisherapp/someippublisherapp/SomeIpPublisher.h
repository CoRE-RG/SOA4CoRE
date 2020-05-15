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

#ifndef __SOQOSMW_SOMEIPPUBLISHER_H_
#define __SOQOSMW_SOMEIPPUBLISHER_H_

#include <soqosmw/applications/someipapp/base/SomeIpAppBase.h>
#include <soqosmw/servicemanager/someipservicemanager/SomeIpLocalServiceManager.h>
#include <utility>

namespace SOQoSMW {

/**
 * @brief A simple SOME/IP Publisher application that sends a dummy SOME/IP message.
 *
 * @ingroup soqosmw/applications
 *
 * @author Mehmet Cakir
 */
class SomeIpPublisher : public virtual SomeIpAppBase  {
    /**
     * Methods
     */
private:
protected:
    /**
     * Initializes the module and waits for find
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
public:
    /**
     * Returns the IP Address of this publisher
     *
     * @param adressType
     * @return local IP Address
     */
    inet::L3Address getIpAddress(inet::L3Address::AddressType adressType);

    /**
     * Returns the port of this publisher
     *
     * @return local Port
     */
    int getPort();

    /**
     * Returns the ID of the service to publish
     *
     * @return service ID to subscribe
     */
    uint16_t getPublishServiceID();

    /**
     * Returns the ID of the instance of the service to publish
     *
     * @return instance ID to publish
     */
    uint16_t getInstanceID();

    /**
     * Adds someip subscriber destination infromation
     */
    void addSomeipSubscriberDestinationInformartion(inet::L3Address ipAddress, uint16_t port);

    /**
     * Override == operator
     * @param other
     * @return true if equal
     */
    bool operator==(SomeIpPublisher* other);

    /**
     * Override != operator
     * @param other
     * @return true if unequal
     */
    bool operator!=(SomeIpPublisher* other);

    /**
     * Member variables
     */
private:
    /**
     * SOME/IP Local Service Manager reference
     */
    SomeIpLocalServiceManager* _someipLSM;

    /**
     * Service ID to publish
     */
    uint16_t _publishServiceID;

    /**
     * Instance ID of service id to publish
     */
    uint16_t _instanceID;

    /**
     * List of destination addresses and ports
     */
    std::list<std::pair<inet::L3Address,uint16_t>> _destinations;
protected:
public:

};
} /* end namespace SOQoSMW */
#endif

