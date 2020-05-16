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

using namespace omnetpp;
namespace SOQoSMW {

class SomeIpPublisher;
class SomeIpSubscriber;

/**
 * @brief Basic SomeIpLocalServiceRegistry
 *
 * @ingroup soqosmw/applications
 *
 * @author Mehmet Cakir
 */
class SomeIpLocalServiceRegistry : public cSimpleModule
{
  /**
   * Methods
   */
  public:
    SomeIpLocalServiceRegistry();
    void registerPublisherService(SomeIpPublisher *someIpPublisher);
    void registerSubscriberService(SomeIpSubscriber *someIpSubscriber);
    void registerRemotePublisherService(uint16_t serviceID, inet::L3Address publisherIP, uint16_t publisherPort);
    std::list<SomeIpPublisher*> getPublisherService(uint16_t serviceID);
    std::list<SomeIpSubscriber*> getSubscriberService(uint16_t serviceID);
    std::list<std::pair<inet::L3Address,uint16_t>> getRemotePublisherInfoList (uint16_t serviceID);
  protected:
    /**
     * Initializes the module and waits for find
     *
     * @param stage indicates the initialization stage
     */
    virtual void initialize(int stage) override;

    /**
     * Handles incoming message
     *
     * @param msg
     */
    virtual void handleMessage(cMessage *msg) override;
  private:

  /**
   * Member variables
   */
  public:
  protected:
  private:
    std::map<uint16_t,std::list<SomeIpPublisher*>> _serviceIDToPublisher;
    std::map<uint16_t,std::list<SomeIpSubscriber*>> _serviceIDToSubscriber;
    std::map<uint16_t,std::list<std::pair<inet::L3Address,uint16_t>>> _remoteServiceIDToPublisher;

};
}
#endif
