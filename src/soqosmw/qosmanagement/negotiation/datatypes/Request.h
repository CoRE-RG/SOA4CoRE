/*
 * Request.h
 *
 *  Created on: 16.01.2018
 *      Author: skrec
 */

#ifndef SOQOSMW_QOSMANAGEMENT_NEGOTIATION_DATATYPES_REQUEST_H_
#define SOQOSMW_QOSMANAGEMENT_NEGOTIATION_DATATYPES_REQUEST_H_

#include <base/EndpointDescription.h>
#include <qospolicy/base/qospolicy.h>
#include <string>
#include <unordered_map>

namespace soqosmw {
class SOQoSMWApplicationBase;
} /* namespace soqosmw */

namespace omnetpp {
class cGate;
} /* namespace omnetpp */

namespace soqosmw {

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

class Request {
public:
    Request(int id, EndpointDescription& local, EndpointDescription& remote,
            std::unordered_map<std::string, IQoSPolicy*>& qosPolicies,
            omnetpp::cGate *notificationGate) :
            _id(id), _local(local), _remote(remote), _qosPolicies(qosPolicies), _notificationGate(
                    notificationGate) {
        this->setStatus(CREATED);
    }

    virtual ~Request(){

    }

    int getId() const {
        return _id;
    }

    omnetpp::cGate* getNotificationGate() {
        return _notificationGate;
    }

    const EndpointDescription& getRemote() const {
        return _remote;
    }

    const std::unordered_map<std::string, IQoSPolicy*>& getQosPolicies() const {
        return _qosPolicies;
    }

    const EndpointDescription& getLocal() const {
        return _local;
    }

    RequestStatus getStatus() const {
        return _status;
    }

    void setStatus(RequestStatus status) {
        _status = status;
    }

private:
    const int _id;
    const EndpointDescription _local;
    const EndpointDescription _remote;
    const std::unordered_map<std::string, IQoSPolicy*> _qosPolicies;
    omnetpp::cGate *_notificationGate;
    RequestStatus _status;
};

} /* namespace soqosmw */

#endif /* SOQOSMW_QOSMANAGEMENT_NEGOTIATION_DATATYPES_REQUEST_H_ */
