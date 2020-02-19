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

#ifndef __SOQOSMW_ENDPOINTBASE_H_
#define __SOQOSMW_ENDPOINTBASE_H_

#include <omnetpp.h>
#include <soqosmw/messages/qosnegotiation/ConnectionSpecificInformation_m.h>
#include <soqosmw/messages/qosnegotiation/QoSNegotiationProtocol_m.h>
#include <string>

#include "soqosmw/connector/base/ConnectorBase.h"
#include "soqosmw/utility/processing/ProcessingTimeSimulation.h"
//AUTO-GENERATED MESSAGES

using namespace omnetpp;

namespace SOQoSMW {

/**
 * @brief The EndpointBase provides a common interface for all endpoint modules.
 * Endpoints are created by the LocalServiceManager module (@see~LocalServiceManager)
 * during runtime to connect a service application to its protocol specific service endpoints.
 *
 * @ingroup soqosmw/endpoints
 *
 * @author Timo Haeckel and Mehmet Cakir for HAW Hamburg
 */
class EndpointBase : public ProcessingTimeSimulation
{
public:

    /**
     * Creates a connection specific information for this endpoint.
     * @return  the connection specific information.
     */
    virtual ConnectionSpecificInformation* getConnectionSpecificInformation() = 0;

    QoSGroups getQos() const {
        return _qos;
    }

    const ConnectorBase* getConnector() const {
        return _connector;
    }

    void setConnector(ConnectorBase* connector) {
        _connector = connector;
    }

    const std::string& getEndpointPath() const {
        return _endpointPath;
    }

    void setEndpointPath(const std::string& path) {
        _endpointPath = path;
    }

    simtime_t getCreationTime();

private:
    /**
     * Signal to emit creationTime
     */
    simsignal_t _creationTimeSignal;

    /**
     * The time the connector is created at.
     */
    simtime_t _creationTime;

  protected:
    virtual void initialize() override;

    /**
     * @brief Indicates a parameter has changed.
     *
     * @param parname Name of the changed parameter or nullptr if multiple parameter changed.
     */
    virtual void handleParameterChange(const char* parname) override;

    /**
    * Processes the scheduled message. Needs to delete the message after handling it.
    * @param msg  the incoming message
    */
   virtual void processScheduledMessage(cMessage *msg) override;

    /**
     * Is called during module initialization to initialize the transport connection;
     */
    virtual void initializeTransportConnection() = 0;

    /**
     * Handles messages incoming from transport gate and
     * forwards them to the connector if they are for applications.
     *
     * @param msg   the message to handle.
     */
    virtual void handleTransportIn(cMessage *msg) = 0;

    /**
     * Handles messages incoming from the connector gate and
     * forwards them to the transport if needed.
     *
     * @param msg   the message to handle.
     */
    virtual void handleConnectorIn(cMessage *msg) = 0;


    /**
     * The name of the endpoint as a path for better specifying (e.g. "Reifendruck/links").
     */
    std::string _endpointPath;

    /**
     * The connector to applications.
     */
    ConnectorBase* _connector;

    /**
     * Holds the information about the QoS group realized in this endpoint.
     */
    QoSGroups _qos;

    /**
     * Gate name (@directIn) to forward messages to the connector
     */
    static const char CONNECTOR_OUT_GATE_NAME []; // = "endpointIn";
    /**
     * Gate name (@directIn) to receive messages from the connector
     */
    static const char CONNECTOR_IN_GATE_NAME []; // = "connectorIn";
    /**
     * Gate name for transport inputs
     */
    static const char TRANSPORT_IN_GATE_NAME []; //= "transportIn";
    /**
     * Gate name for transport outputs
     */
    static const char TRANSPORT_OUT_GATE_NAME []; //= "transportOut";
};

} /*end namespace SOQoSMW*/

#endif
