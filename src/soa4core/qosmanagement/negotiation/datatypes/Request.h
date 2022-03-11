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

#ifndef SOA4CORE_QOSMANAGEMENT_NEGOTIATION_DATATYPES_REQUEST_H_
#define SOA4CORE_QOSMANAGEMENT_NEGOTIATION_DATATYPES_REQUEST_H_

#include "soa4core/qosmanagement/negotiation/datatypes/EndpointDescription.h"
//AUTO-GENERATED MESSAGE
#include "soa4core/messages/qosnegotiation/QoSNegotiationProtocol_m.h"
//STD
#include <string>
#include <unordered_map>

namespace omnetpp {
class cGate;
} /* namespace omnetpp */

namespace SOA4CoRE {

/**
 * The request status
 */
enum RequestStatus {
    CREATED,              //!< CREATED
    REQUEST_SEND,         //!< REQUEST_SEND
    ACKKNOWLEDGED_FAILURE,//!< ACKKNOWLEDGED_FAILURE
    ESTABLISH_SEND,       //!< ESTABLISH_SEND
    FINALISED_SUCCESS,    //!< FINALISED_SUCCESS
    FINALISED_FAILURE,    //!< FINALISED_FAILURE
    TIMEOUT,              //!< TIMEOUT
    INVALID               //!< INVALID
};

/**
 * @brief Request class which contains information about the local and remote endpoint.
 *
 * @ingroup soa4core/applications
 *
 * @author Timo Haeckel and Mehmet Mueller for HAW Hamburg
 */
class Request {
public:
    /**
     * Constructor
     * @param requestId The request ID
     * @param subscriberEndpointDescription The subscriber endpoint description
     * @param publisherEndpointDescription The publisher endpoint description
     * @param qosGroup The QoS group
     * @param notificationGate The notification gate
     */
    Request(int requestId, EndpointDescription& subscriberEndpointDescription, EndpointDescription& publisherEndpointDescription,
            QoSGroup qosGroup,
            omnetpp::cGate *notificationGate) :
            _requestId(requestId), _subscriberEndpointDescription(subscriberEndpointDescription),
            _publisherEndpointDescription(publisherEndpointDescription), _qosGroup(qosGroup), _notificationGate(notificationGate) {
        this->setStatus(CREATED);
    }

    virtual ~Request(){

    }

    /**
     * Returns the request ID
     * @return the request ID
     */
    int getRequestId() const {
        return _requestId;
    }

    /**
     * Returns the notification gate
     * @return the notification gate
     */
    omnetpp::cGate* getNotificationGate() {
        return _notificationGate;
    }

    /**
     * Returns the publisher endpoint description
     * @return the publisher endpoint description
     */
    const EndpointDescription& getPublisherEndpointDescription() const {
        return _publisherEndpointDescription;
    }

    /**
     * Returns the QoS group
     * @return the QoS group
     */
    const QoSGroup getQosGroup() const {
        return _qosGroup;
    }

    /**
     * Returns the subscriber endpoint description
     * @return the subscriber endpoint description
     */
    const EndpointDescription& getSubscriberEndpointDescription() const {
        return _subscriberEndpointDescription;
    }

    /**
     * Returns the status of this request
     * @return the status of this request
     */
    RequestStatus getStatus() const {
        return _status;
    }

    /**
     * Sets the status of this request
     * @param status the status to be set
     */
    void setStatus(RequestStatus status) {
        _status = status;
    }

private:
    /**
     * The request ID of this request
     */
    const int _requestId;

    /**
     * The subscriber endpoint description
     */
    const EndpointDescription _subscriberEndpointDescription;

    /**
     * The publisher endpoint description
     */
    const EndpointDescription _publisherEndpointDescription;

    /**
     * The requested QoS group
     */
    const QoSGroup _qosGroup;

    /**
     * The notification gate
     */
    omnetpp::cGate *_notificationGate;

    /**
     * The status of this request
     */
    RequestStatus _status;
};

} /* namespace SOA4CoRE */

#endif /* SOA4CORE_QOSMANAGEMENT_NEGOTIATION_DATATYPES_REQUEST_H_ */
