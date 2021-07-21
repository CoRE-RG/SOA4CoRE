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

#ifndef SOQOSMW_QOSMANAGEMENT_NEGOTIATION_DATATYPES_REQUEST_H_
#define SOQOSMW_QOSMANAGEMENT_NEGOTIATION_DATATYPES_REQUEST_H_

#include "soqosmw/base/EndpointDescription.h"
#include "soqosmw/qospolicy/base/qospolicy.h"

#include <string>
#include <unordered_map>

namespace SOQoSMW {
class SOQoSMWApplicationBase;
} /* namespace SOQoSMW */

namespace omnetpp {
class cGate;
} /* namespace omnetpp */

namespace SOQoSMW {

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
 * @ingroup soqosmw/applications
 *
 * @author Timo Haeckel for HAW Hamburg
 */
class Request {
public:
    Request(int serviceId, EndpointDescription& subscriberEndpointDescription, EndpointDescription& publisherEndpointDescription,
            QoSPolicyMap& qosPolicies,
            omnetpp::cGate *notificationGate) :
            _serviceId(serviceId), _subscriberEndpointDescription(subscriberEndpointDescription),
            _publisherEndpointDescription(publisherEndpointDescription), _qosPolicies(qosPolicies), _notificationGate(notificationGate) {
        this->setStatus(CREATED);
    }

    virtual ~Request(){

    }

    int getServiceId() const {
        return _serviceId;
    }

    omnetpp::cGate* getNotificationGate() {
        return _notificationGate;
    }

    const EndpointDescription& getPublisherEndpointDescription() const {
        return _publisherEndpointDescription;
    }

    const QoSPolicyMap& getQosPolicies() const {
        return _qosPolicies;
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
    const int _serviceId;
    const EndpointDescription _subscriberEndpointDescription;
    const EndpointDescription _publisherEndpointDescription;
    const QoSPolicyMap _qosPolicies;
    omnetpp::cGate *_notificationGate;
    RequestStatus _status;
};

} /* namespace SOQoSMW */

#endif /* SOQOSMW_QOSMANAGEMENT_NEGOTIATION_DATATYPES_REQUEST_H_ */
