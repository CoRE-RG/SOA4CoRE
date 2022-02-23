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

#ifndef __SOA4CORE_ENDPOINTBASE_H_
#define __SOA4CORE_ENDPOINTBASE_H_

#include <omnetpp.h>
#include <string>
#include "soa4core/utility/processing/ProcessingTimeSimulation.h"
//AUTO-GENERATED MESSAGES
#include "soa4core/messages/qosnegotiation/ConnectionSpecificInformation_m.h"
#include "soa4core/messages/qosnegotiation/QoSNegotiationProtocol_m.h"

using namespace omnetpp;

namespace SOA4CoRE {

/**
 * @brief The EndpointBase provides a common interface for all endpoint modules.
 * Endpoints are created by the Manager module (@see~Manager)
 * during runtime to connect a service application to its protocol specific service endpoints.
 *
 * @ingroup soa4core/endpoints
 *
 * @author Timo Haeckel and Mehmet Mueller for HAW Hamburg
 */
class EndpointBase: public ProcessingTimeSimulation {

/**
 * Methods
 */
public:

    /**
     * Creates a connection specific information for this endpoint.
     * @return  the connection specific information.
     */
    virtual ConnectionSpecificInformation* getConnectionSpecificInformation() = 0;

    /**
     * Returns the state whether this endpoint is connected
     * @return the state whether this endpoint is connected
     */
    bool isIsConnected() const {
        return _isConnected;
    }

    /**
     * Sets the state for the endpoint whether it is connected
     * @param isConnected the state whether this endpoint is connected
     */
    void setIsConnected(bool isConnected = false) {
        _isConnected = isConnected;
    }

    /**
     * Returns the QoS group for which this endpoint serves
     * @return the QoS group for which this endpoint serves
     */
    QoSGroup getQos() const {
        return _qos;
    }

    /**
     * Returns the endpoint path or name of the service for which this endpoint serves
     * @return the endpoint path or name of the service for which this endpoint serves
     */
    const std::string& getEndpointPath() const {
        return _endpointPath;
    }

    /**
     * Sets the endpoint path or name of the service for which this endpoint serves
     * @param path the endpoint path or name of the service for which this endpoint serves
     */
    void setEndpointPath(const std::string& path) {
        _endpointPath = path;
    }

    /**
     * Returns the creation time of this endpoint
     * @return the creation time of this endpoint
     */
    simtime_t getCreationTime();

    /**
     * Returns the ID of the service for which this endpoint serves
     * @return the ID of the service for which this endpoint serves
     */
    int getServiceId() const {
        return _serviceId;
    }

    /**
     * Sets the service ID for which this endpoint serves
     * @param serviceId the service ID for which this endpoint serves
     */
    void setServiceId(int serviceId) {
        _serviceId = serviceId;
    }
protected:
    /**
     * Initializes the module
     */
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
     * Helper Function to connect the endpoints transport in and out gates to a
     * transport modules in and out gate.
     * A gate in the middleware compound module will be created as a pass through.
     * (endpoit <--> middleware <--> transport)
     *
     * @param tpModule Transport module to connect to.
     * @param tpInGateName Name of the transport modules input gate.
     * @param tpGateOut Name of the transport modules output gate.
     */
    void connectToTransportGate(cModule* tpModule,
            const char* tpInGateName, const char* tpGateOut);

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
 * Member variables
 */
public:
protected:

    /**
     * Is this endpoint connected to transport
     */
    bool _isConnected = false;

    /**
     * The name of the endpoint as a path for better specifying (e.g. "Reifendruck/links").
     */
    std::string _endpointPath;

    /**
     * The service ID for which this endpoint serves
     */
    int _serviceId;

    /**
     * Holds the information about the QoS group realized in this endpoint.
     */
    QoSGroup _qos;

    /**
     * Gate name (@directIn) to forward messages to the connector
     */
    static const char CONNECTOR_OUT_GATE_NAME[]; // = "endpointIn";
    /**
     * Gate name (@directIn) to receive messages from the connector
     */
    static const char CONNECTOR_IN_GATE_NAME[]; // = "connectorIn";
    /**
     * Gate name for transport inputs
     */
    static const char TRANSPORT_IN_GATE_NAME[]; //= "transportIn";
    /**
     * Gate name for transport outputs
     */
    static const char TRANSPORT_OUT_GATE_NAME[]; //= "transportOut";
    /**
     * Gate name for transport inputs
     */
    static const char TRANSPORT_MIDDLEWARE_IN_GATE_NAME[]; //= "tpEndpointsIn";
    /**
     * Gate name for transport outputs
     */
    static const char TRANSPORT_MIDDLEWARE_OUT_GATE_NAME[]; //= "tpEndpointsOut";

private:
    /**
     * Signal to emit creationTime
     */
    simsignal_t _creationTimeSignal;

    /**
     * The time the connector is created at.
     */
    simtime_t _creationTime;
};

} /*end namespace SOA4CoRE*/

#endif
