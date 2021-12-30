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

#ifndef __SOQOSMW_QOSLOCALSERVICEMANAGER_H_
#define __SOQOSMW_QOSLOCALSERVICEMANAGER_H_

#include <omnetpp.h>
#include "soqosmw/servicemanager/LocalServiceManager.h"
#include "soqosmw/discovery/base/IServiceDiscovery.h"

using namespace omnetpp;
namespace SOQoSMW {
/**
 * @brief The QoSLocalServiceManager is used to create, find and remove local Services.
 *
 * @ingroup soqosmw/servicemanager
 *
 * @author Mehmet Cakir
 */
class QoSLocalServiceManager : public LocalServiceManager
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
     * @param qosService the QoS service representation of the subscriber
     */
    void subscribeService(QoSServiceIdentifier publisherServiceIdentifier, QoSService qosService) override;

  private:
    /**
     * Creates a negotiation request
     * @param publisherService
     * @return the negotiation request
     */
    Request* createNegotiationRequest(QoSService publisherService);

    /**
     * Subscribes to the offered service
     * @param obj
     */
    void subscribeOfferedService(cObject* obj);

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
} /* end namespace SOQoSMW */
#endif
