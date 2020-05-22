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
#include <soqosmw/discovery/someipservicediscovery/SomeIpSD.h>
#include <soqosmw/serviceregistry/someiplocalserviceregistry/SomeIpLocalServiceRegistry.h>

using namespace omnetpp;
namespace SOQoSMW {

class SomeIpPublisher;
class SomeIpSubscriber;
class SubscriptionRelations;

/**
 * @brief Base class for a SOME/IP local service manager.
 *
 * @ingroup soqosmw/servicemanager
 *
 * @author Mehmet Cakir
 */
class SomeIpLocalServiceManager : public cSimpleModule
{
    /**
     * Methods
     */
  public:
    /**
     * Registers a SOME/IP Publisher
     * @param someipPublisher
     */
    void registerPublisherService(SomeIpPublisher *someIpPublisher);

    /**
     * Registers a SOME/IP Subscriber
     * @param someipSubscriber
     */
    void registerSubscriberService(SomeIpSubscriber *someIpSubscriber);

    /**
     * Discovers a service
     * @param serviceID of service that need to be discovered
     * @param instanceID of service that need to be discovered
     * @param subscriberIP IP address of subscriber that wants to subscribe
     * @param subscriberPort port of subscriber that wants to subscribe
     */
    void discoverService(SomeIpSubscriber* someIpSubscriber);

    /**
     * Looks for publisher with given service id in local registry
     * @param serviceID service id of service to search
     * @return list of publishers that publish service
     */
    std::list<ISomeIpServiceApp*> lookLocalForPublisherService(uint16_t serviceID);

    /**
     * Adds a remote publisher
     * @param serviceID of remote publisher
     * @param publisherIP IP address of remote publisher
     * @param publisherPort port of remote publisher
     */
    void addRemotePublisher(uint16_t serviceID, inet::L3Address publisherIP, uint16_t publisherPort);

    /**
     * Starts a publisher to publish his service
     * @param serviceID
     * @param subscriberIP
     * @param subscriberPort
     */
    void publishToSubscriber(uint16_t serviceID, L3Address subscriberIP, uint16_t subscriberPort);


    /**
     * Acknowledges a service
     * @param serviceID
     * @param publisherIp
     * @param port
     */
    void acknowledgeService(uint16_t serviceID, inet::L3Address publisherIp, uint16_t publisherPort);
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
    /**
     * SOME/IP Service Discovery reference
     */
    SomeIpSD* _someIpSD;

    /**
     * SOME/IP Local Service Registry reference
     */
    SomeIpLocalServiceRegistry* _someIpLSR;

    /**
     * Map for subscription relation
     */
    std::map<uint16_t,std::map<SomeIpSubscriber*,SubscriptionRelations>> _subscriptionRelations;
};
}
#endif
