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

#ifndef __SOQOSMW_SOMEIPLOCALSERVICEMANAGER_H_
#define __SOQOSMW_SOMEIPLOCALSERVICEMANAGER_H_

#include <omnetpp.h>
#include <soqosmw/applications/someipapp/someipservicediscovery/SomeipSD.h>
#include <soqosmw/applications/someipapp/someiplocalserviceregistry/SomeipLocalServiceRegistry.h>
#include "soqosmw/applications/someipapp/base/SomeipAppBase.h"
#define SOMEIPLOCALSERVICEMANAGERIDX 1

using namespace omnetpp;
namespace SOQoSMW {

class SomeipPublisher;
class SomeipSubscriber;

/**
 * @brief Base class for a SOME/IP Local service manager.
 *
 * @ingroup soqosmw/applications
 *
 * @author Mehmet Cakir
 */
class SomeipLocalServiceManager : public virtual SomeipAppBase
{
  public:
    /**
     * Registers a Someip Publisher
     * @param someipPublisher
     */
    void registerPublisherService(SomeipPublisher *someipPublisher);

    /**
     * Registers a Someip Subscriber
     * @param someipSubscriber
     */
    void registerSubscriberService(SomeipSubscriber *someipSubscriber);
    void discoverService(uint16_t serviceID, uint16_t instanceID, inet::L3Address subscriberIP, uint16_t subscriberPort);
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
  private:
  public:
  protected:
  private:
    /**
     * SOME/IP Service Discovery reference
     */
    SomeipSD* _someipSD;

    /**
     * SOME/IP Local Service Registry reference
     */
    SomeipLocalServiceRegistry* _someipLSR;
};
}
#endif
