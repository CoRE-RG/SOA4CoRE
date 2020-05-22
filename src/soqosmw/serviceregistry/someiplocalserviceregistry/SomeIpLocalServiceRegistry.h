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
class ISomeIpServiceApp;

/**
 * @brief This class manages all registered publishers and subscribers
 *
 * @ingroup soqosmw/serviceregistry
 *
 * @author Mehmet Cakir
 */
class SomeIpLocalServiceRegistry : public cSimpleModule
{
  /**
   * Methods
   */
  public:
    /**
     * Registers a SomeIpPublisher
     * @param someIpPublisher
     */
    void registerPublisherService(ISomeIpServiceApp *someIpPublisher);

    /**
     * Registers a SomeIpSubscriber
     * @param someIpSubscriber
     */
    void registerSubscriberService(ISomeIpServiceApp *someIpSubscriber);

    /**
     * Registers a remote publisher endpoint
     * @param serviceID
     * @param publisherIP
     * @param publisherPort
     */
    void registerRemotePublisherService(uint16_t serviceID, inet::L3Address publisherIP, uint16_t publisherPort);

    /**
     * Returns all local publisher services
     * @param serviceID
     * @return list of all local publisher services
     */
    std::list<ISomeIpServiceApp*> getPublisherService(uint16_t serviceID);

    /**
     * Returns all local subscriber services
     * @param serviceID
     * @return list of all local subscriber services
     */
    std::list<ISomeIpServiceApp*> getSubscriberService(uint16_t serviceID);

    /**
     * Returns all known remote publisher endpoints
     * @param serviceID
     * @return list of all known remote publisher endpoints
     */
    std::list<std::pair<inet::L3Address,uint16_t>> getRemotePublisherEndpoints(uint16_t serviceID);
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
     * List of all local publishers
     */
    std::map<uint16_t,std::list<ISomeIpServiceApp*>> _serviceIDToPublisher;

    /**
     * List of all local subscribers
     */
    std::map<uint16_t,std::list<ISomeIpServiceApp*>> _serviceIDToSubscriber;

    /**
     * List of all remote publisher endpoints
     */
    std::map<uint16_t,std::list<std::pair<inet::L3Address,uint16_t>>> _remoteServiceIDToPublisher;

    /**
     * Registers a service
     * @param someIpServiceList the list, where the service will be registered
     * @param someIpService the service which will be registered
     */
    void registerService(std::map<uint16_t,std::list<ISomeIpServiceApp*>>& someIpServiceMap, ISomeIpServiceApp* someIpService);

    /**
     * Returns services from a map related to a service id
     * @param someIpServiceList
     * @param serviceId
     * @return list over ISomeIpServiceApp
     */
    std::list<ISomeIpServiceApp*> getServices(const std::map<uint16_t,std::list<ISomeIpServiceApp*>>& someIpServiceMap, uint16_t serviceID);

};
} /* end namespace SOQoSMW */
#endif
