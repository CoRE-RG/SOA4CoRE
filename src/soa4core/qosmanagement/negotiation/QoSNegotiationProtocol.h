//
// c Timo Haeckel for HAW Hamburg
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

#ifndef __SOA4CORE_QOSMANAGEMENT_NEGOTIATION_QoSNegotiationProtocol_H_
#define __SOA4CORE_QOSMANAGEMENT_NEGOTIATION_QoSNegotiationProtocol_H_

#include "soa4core/qosmanagement/negotiation/broker/QoSBroker.h"
#include "soa4core/utility/processing/ProcessingTimeSimulation.h"
//INET
#include <inet/networklayer/common/L3Address.h>
#include <inet/transportlayer/contract/udp/UDPSocket.h>
#include <inet/common/InitStages.h>
//STD
#include <vector>

using namespace omnetpp;

namespace SOA4CoRE {

/**
 * @brief QoSNegotiationProtocol provides all functionality to negotiatiate the QoS Policies for a connection of to @class{IEndpoints}.
 *
 * @ingroup soa4core/qosmanagement
 *
 * @author Timo Haeckel and Mehmet Mueller for HAW Hamburg
 */
class QoSNegotiationProtocol: public ProcessingTimeSimulation {
    friend class QoSLocalServiceManager;
public:
    QoSNegotiationProtocol();
    virtual ~QoSNegotiationProtocol();

protected:
    using ProcessingTimeSimulation::initialize;
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override {
        return inet::NUM_INIT_STAGES;
    }
    virtual void handleParameterChange(const char* parname) override;

    /**
     * Processes the scheduled message. Needs to delete the message after handling it.
     * @param msg  the incoming message
     */
    virtual void processScheduledMessage(cMessage *msg) override;

private:

    int getProtocolPort();

    void createQoSBroker(Request* request);

    /**
     * Sets up the UDP socket.
     */
    void socketSetup();

    /**
     * Checks if a socket is bound.
     * @return true if the socket is bound, otherwise false.
     */
    bool isSocketBound();

    /**
     * Closes the UDP connection.
     */
    void socketClose();

    /**
     * Caches the localAddress parameter.
     */
    inet::L3Address _localAddress;

    /**
     * Caches the protocolPort parameter.
     */
    int _protocolPort;

    /**
     * The UDP Socket for sending and receiving messages.
     */
    inet::UDPSocket _socket;

    /**
     * Check if the socket is bound.
     */
    bool _socketBound;

    /**
     * A QoS Broker that handles the negotiation.
     */
    std::vector<QoSBroker*> _brokers;

    /**
     * Reference to the local service manager.
     */
    LocalServiceManager* _lsm;

    /**
     * Signal to capture time points of negotiation messages.
     */
    simsignal_t _qosNt;

    /**
     * Signal to emit QOSNP frames.
     */
    simsignal_t _rxPkSignal;
};

} /* namespace SOA4CoRE */

#endif
