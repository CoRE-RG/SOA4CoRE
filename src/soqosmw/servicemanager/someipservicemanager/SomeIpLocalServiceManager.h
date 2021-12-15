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
#include "soqosmw/discovery/someipservicediscovery/SomeIpSD.h"
#include "soqosmw/servicemanager/LocalServiceManager.h"
#include "soqosmw/service/qosservice/QoSService.h"

using namespace omnetpp;
namespace SOQoSMW {

/**
 * @brief Base class for a SOME/IP local service manager.
 *
 * @ingroup soqosmw/servicemanager
 *
 * @author Mehmet Cakir
 */
class SomeIpLocalServiceManager : public LocalServiceManager
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

    /**
     * @brief Subscribes the given service if it is already discovered otherwise a service discovery is initiated
     * @param publisherServiceIdentifier service identifier of the service to be subscribed to
     * @param qosPolicyMap the QoS policy map
     * @param instanceId the instanceId
     */
    void subscribeService(IServiceIdentifier& publisherServiceIdentifier, QoSPolicyMap& qosPolicyMap, uint16_t instanceId) override;
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
  private:
    /**
     * Looks for a requested service in the local registry
     * @param obj the SOME/IP SD header container containing the SOME/IP SD header and its corresponding entry
     */
    void lookForService(cObject* obj);

    /**
     * Adds the offered service into the _pendingOffersMap and initiates the subscription
     * @param obj the offered service
     */
    void addToPendingOffersAndSubscribe(cObject* obj);

    /**
     * Acknowledges the subscription
     * @param obj the service to be subscribed to
     */
    void acknowledgeSubscription(cObject* obj);

    /**
     * Processes an acknowledged subscription
     * @param obj the service to be subscribed to
     */
    void processAcknowledgedSubscription(cObject* obj);

    /**
     * Subscribes to the found service
     * @param obj
     */
    void subscribeFoundService(cObject* obj);

    /**
     * Creates a negotiation request
     * @param publisherService
     * @param qosPolicies
     * @return the negotiation request
     */
    Request* createNegotiationRequest(IService* publisherService, QoSPolicyMap qosPolicies);

    /**
     * Member variables
     */
  public:
  protected:
    /**
     * The signal which is emitted when a requested service is found
     */
    omnetpp::simsignal_t _findResultSignal;

    /**
     * The signal which is emitted when a service is subscribed
     */
    omnetpp::simsignal_t _subscribeSignal;

    /**
     * The signal which is emitted when a subscription has been acknowledged
     */
    omnetpp::simsignal_t _subscribeAckSignal;
  private:
    /**
     * Contains received offers whose subscription has not yet been acknowledged
     * TODO: Check if it is necessary. No real use at the moment
     */
    std::map<IServiceRegistry::ServiceId, std::list<QoSService>> _pendingOffersMap;

    /**
     * The QoS Negotiation Protocol module.
     */
    QoSNegotiationProtocol* _qosnp;

    /**
     * The service discovery.
     */
    IServiceDiscovery* _sd;

    /**
     * Indicates if QoS negotiation is available
     */
    bool _qosnpAvailable;
};
} /* end namespace SOQoSMW */
#endif
