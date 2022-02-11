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

#ifndef __SOA4CORE_SERVICEMANAGER_QOS_QOSMANAGER_H_
#define __SOA4CORE_SERVICEMANAGER_QOS_QOSMANAGER_H_

#include <omnetpp.h>
#include <soa4core/manager/Manager.h>
#include "soa4core/discovery/base/IServiceDiscovery.h"
#include "soa4core/qosmanagement/negotiation/QoSNegotiationProtocol.h"

using namespace omnetpp;
namespace SOA4CoRE {
/**
 * @brief The QoSManager is used to create, find and remove local Services.
 *
 * @ingroup soa4core/manager
 *
 * @author Mehmet Mueller for HAW Hamburg
 */
class QoSManager : public Manager
{

    /**
     * Methods
     */
  public:
    /**
     * @brief Receives discovery response
     * @param source
     * @param signalID
     * @param obj
     * @param details
     */
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj, cObject *details) override;

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
     * @param msg the received message
     */
    virtual void handleMessage(cMessage *msg) override;

    /**
     * @brief Subscribes the given service if it is already discovered otherwise a service discovery is initiated
     * @param publisherServiceIdentifier service identifier of the service to be subscribed to
     * @param subscriberApplicationInformation the application information about the subscriber
     */
    void subscribeService(QoSServiceIdentifier publisherServiceIdentifier, SubscriberApplicationInformation subscriberApplicationInformation) override;

  private:
    /**
     * Creates a negotiation request
     * @param publisherService
     * @return the negotiation request
     */
    Request* createNegotiationRequest(PublisherApplicationInformation publisherApplicationInformation, QoSGroup qosGroup);

    /**
     * Subscribes to the offered service
     * @param obj
     */
    void subscribeOfferedService(cObject* obj);

    /**
     * Looks for a requested service in the local registry
     * @param obj the SomeIpSDFindRequest
     */
    void lookForService(cObject* obj);

    /**
     * Member variables
     */
  public:
  protected:
    /**
     * The signal which is emitted when a requested service is found
     */
    omnetpp::simsignal_t _findResultSignal;
  private:
    /**
     * The QoS Negotiation Protocol module.
     */
    QoSNegotiationProtocol* _qosnp;

    /**
     * The service discovery.
     */
    IServiceDiscovery* _sd;

};
} /* end namespace SOA4CoRE */
#endif
