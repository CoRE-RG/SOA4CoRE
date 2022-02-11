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

#include <soa4core/qosmanagement/negotiation/datatypes/EndpointDescription.h>
#include "soa4core/messages/qosnegotiation/QoSNegotiationProtocol_m.h"
#include <string>
#include <unordered_map>

namespace omnetpp {
class cGate;
} /* namespace omnetpp */

namespace SOA4CoRE {

enum RequestStatus {
    CREATED,
    REQUEST_SEND,
    ACKKNOWLEDGED_FAILURE,
    ESTABLISH_SEND,
    FINALISED_SUCCESS,
    FINALISED_FAILURE,
    TIMEOUT,
    INVALID
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
    Request(int requestId, EndpointDescription& subscriberEndpointDescription, EndpointDescription& publisherEndpointDescription,
            QoSGroup qosGroup,
            omnetpp::cGate *notificationGate) :
            _requestId(requestId), _subscriberEndpointDescription(subscriberEndpointDescription),
            _publisherEndpointDescription(publisherEndpointDescription), _qosGroup(qosGroup), _notificationGate(notificationGate) {
        this->setStatus(CREATED);
    }

    virtual ~Request(){

    }

    int getRequestId() const {
        return _requestId;
    }

    omnetpp::cGate* getNotificationGate() {
        return _notificationGate;
    }

    const EndpointDescription& getPublisherEndpointDescription() const {
        return _publisherEndpointDescription;
    }

    const QoSGroup getQosGroup() const {
        return _qosGroup;
    }

    const EndpointDescription& getSubscriberEndpointDescription() const {
        return _subscriberEndpointDescription;
    }

    RequestStatus getStatus() const {
        return _status;
    }

    void setStatus(RequestStatus status) {
        _status = status;
    }

private:
    const int _requestId;
    const EndpointDescription _subscriberEndpointDescription;
    const EndpointDescription _publisherEndpointDescription;
    const QoSGroup _qosGroup;
    omnetpp::cGate *_notificationGate;
    RequestStatus _status;
};

} /* namespace SOA4CoRE */

#endif /* SOA4CORE_QOSMANAGEMENT_NEGOTIATION_DATATYPES_REQUEST_H_ */
