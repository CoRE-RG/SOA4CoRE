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

#ifndef __SOQOSMW_SOMEIPLOCALSERVICEREGISTRY_H_
#define __SOQOSMW_SOMEIPLOCALSERVICEREGISTRY_H_

#include <omnetpp.h>
#include <map>
#include <list>
#include <utility>
#include "soqosmw/applications/someipapp/base/SomeipAppBase.h"

#define SOMEIPLOCALSERVICEREGISTRYIDX 2

using namespace omnetpp;
namespace SOQoSMW {

class SomeipPublisher;
class SomeipSubscriber;

/**
 * @brief Basic SomeipLocalServiceRegistry
 *
 * @ingroup soqosmw/applications
 *
 * @author Mehmet Cakir
 */
class SomeipLocalServiceRegistry : public virtual SomeipAppBase
{
  /**
   * Methods
   */
  public:
    void registerPublisherService(SomeipPublisher *someipPublisher);
    void registerSubscriberService(SomeipSubscriber *someipSubscriber);
    void registerRemotePublisherService(uint16_t serviceID, inet::L3Address publisherIP, uint16_t publisherPort);
    std::list<SomeipPublisher*> getPublisherService(uint16_t serviceID);
    std::list<SomeipSubscriber*> getSubscriberService(uint16_t serviceID);
    std::list<std::pair<inet::L3Address,uint16_t>> getRemotePublisherInfoList (uint16_t serviceID);
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

  /**
   * Member variables
   */
  public:
  protected:
  private:
    std::map<uint16_t,std::list<SomeipPublisher*>> _serviceIDToPublisher;
    std::map<uint16_t,std::list<SomeipSubscriber*>> _serviceIDToSubscriber;
    std::map<uint16_t,std::list<std::pair<inet::L3Address,uint16_t>>> _remoteServiceIDToPublisher;

};
}
#endif
