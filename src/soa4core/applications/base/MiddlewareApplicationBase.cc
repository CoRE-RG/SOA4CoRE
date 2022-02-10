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

#include "MiddlewareApplicationBase.h"
#include "soa4core/connector/base/ConnectorBase.h"
#include "soa4core/servicemanager/base/ILocalServiceManager.h"

#include <cstring>

namespace SOA4CoRE {

MiddlewareApplicationBase::~MiddlewareApplicationBase()
{
}

void MiddlewareApplicationBase::handleMessage(cMessage *msg) {
    throw cRuntimeError("MiddlewareApplicationBase does not handle messages. Implementations need to handle them.");
}

void MiddlewareApplicationBase::initialize() {
    this->_subscriberName = "";
    this->_publisherName = "";
    _localServiceManager =
            dynamic_cast<ILocalServiceManager*>(getParentModule()->getModuleByPath(par("serviceManagerModulePath")));
    if (!_localServiceManager) {
        throw cRuntimeError(
                "Configuration problem of parameter serviceManagerModulePath in module %s.",
                this->getFullName());
    }
}

std::string MiddlewareApplicationBase::getServiceName() {
    std::string serviceName = "";
    if (this->_publisherName.length() > 0) {
        serviceName = this->_publisherName;
    } else if (this->_subscriberName.length() > 0) {
        serviceName = this->_subscriberName;
    }
    return serviceName;
}

void MiddlewareApplicationBase::handleParameterChange(const char* parname) {
    if (!parname || !strcmp(parname, "applicationUDPPort")) {
        _udpPort = par("applicationUDPPort");
        if (_udpPort <= 0) {
            throw cRuntimeError(
                    "Configuration problem of parameter applicationUDPPort in module %s.",
                    this->getFullName());
        }
    }
    if (!parname || !strcmp(parname, "applicationTCPPort")) {
        _tcpPort = par("applicationTCPPort");
        if (_tcpPort <= 0) {
            throw cRuntimeError(
                    "Configuration problem of parameter applicationTCPPort in module %s.",
                    this->getFullName());
        }
    }
    if (!parname || !strcmp(parname, "localAddress")) {
        _localAddress = par("localAddress").stdstringValue();
    }

    if (!parname || !strcmp(parname, "instanceID")) {
        _instanceId = par("instanceID").intValue();
    }
}

} /* end namespace SOA4CoRE */
