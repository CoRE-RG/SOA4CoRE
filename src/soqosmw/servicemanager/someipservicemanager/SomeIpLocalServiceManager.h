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
     * @param qosService the QoS service
     */
    void subscribeService(QoSServiceIdentifier publisherServiceIdentifier, QoSService qosService) override;
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
     * Adds a service into the local service registry
     * @param obj the service
     */
    void addToLocalServiceRegistry(cObject* obj);

    /**
     * Subscribes a service if there is a pending request for it
     * @param obj the given service
     */
    void subscribeServiceIfThereIsAPendingRequest(cObject* obj);

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
     * Creates a negotiation request
     * @param publisherService
     * @param qosPolicies
     * @return the negotiation request
     */
    Request* createNegotiationRequest(QoSService publisherService);

    /**
     * Creates a subscriber endpoint for a service
     * @param service the service
     */
    void createSubscriberEndpoint(QoSService service);

    /**
     * Returns the corresponding L4 protocol of the service
     * @param service
     * @return the corresponding L4 protocol of the service
     */
    IPProtocolId getIPProtocolId(QoSService service);

    /**
     * Returns a copy of the given QoSService but only including the given qosGroup in its QoSGroups vector.
     * This way the QoSService includes only the QoSGroup which is wanted to be subscribed to
     * @param qosGroup
     * @return the QoSService copy
     */
    QoSService getQoSServiceCopyWithGivenQoSGroupOnly(QoSService qosService, QoSGroups qosGroup);

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
