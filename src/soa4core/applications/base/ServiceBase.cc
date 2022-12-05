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

#include "soa4core/applications/base/ServiceBase.h"
#include "soa4core/manager/base/IManager.h"
#include "soa4core/connector/base/ConnectorBase.h"
//CoRE4INET
#include <core4inet/base/CoRE4INET_Defs.h>
#include <core4inet/utilities/ConfigFunctions.h>
//INET
#include <inet/networklayer/common/L3AddressResolver.h>
//STD
#include <cstring>

namespace SOA4CoRE {

#define SERVICE_START_MSG_NAME "Start Message"

ServiceBase::~ServiceBase()
{
}

bool ServiceBase::isEnabled() {
    return this->_enabled;
}

void ServiceBase::handleMessage(cMessage *msg) {
    if (msg->isSelfMessage() && (strcmp(msg->getName(), SERVICE_START_MSG_NAME) == 0)) {
        this->handleStart();
        if (getEnvir()->isGUI()) {
            getDisplayString().setTagArg("i2", 0, "status/active");
        }
    } else {
        throw cRuntimeError("ServiceBase does not handle messages. Implementations need to handle them.");
    }
    delete msg;
}

void ServiceBase::initialize() {
    handleParameterChange(nullptr);
    _localServiceManager =
            dynamic_cast<IManager*>(getParentModule()->getModuleByPath(par("serviceManagerModulePath")));
    if (!_localServiceManager) {
        throw cRuntimeError(
                "Configuration problem of parameter serviceManagerModulePath in module %s.",
                this->getFullName());
    }

    if (this->isEnabled()) {
        scheduleAt(_startTime, new cMessage(SERVICE_START_MSG_NAME));
        if (getEnvir()->isGUI()) {
            getDisplayString().setTagArg("i2", 0, "status/asleep");
        }
    } else {
        if (getEnvir()->isGUI()) {
            getDisplayString().setTagArg("i2", 0, "status/stop");
        }
    }
}

std::string ServiceBase::getServiceName() {
    return this->_serviceName;
}

int ServiceBase::getServiceId() {
    return this->_serviceId;
}

 inet::L3Address ServiceBase::getAddress() {
    return inet::L3AddressResolver().resolve(this->_localAddress.c_str());
}

uint16_t ServiceBase::getInstanceId() {
    return this->_instanceId;
}

int ServiceBase::getTcpPort() {
    return this->_tcpPort;
}

int ServiceBase::getUdpPort() {
    return this->_udpPort;
}

void ServiceBase::handleParameterChange(const char* parname) {
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

    if (!parname || !strcmp(parname, "serviceName")) {
        this->_serviceName = par("serviceName").stdstringValue();
    }

    if (!parname || !strcmp(parname, "enabled")) {
        this->_enabled = par("enabled").boolValue();
    }
    if (!parname || !strcmp(parname, "startTime")) {
        this->_startTime = CoRE4INET::parameterDoubleCheckRange(
                par("startTime"), 0, SIMTIME_MAX.dbl());
    }

    if (!parname || !strcmp(parname, "instanceID")) {
        _instanceId = par("instanceID").intValue();
    }
    if (!parname || !strcmp(parname, "serviceId"))
    {
        this->_serviceId = par("serviceId").intValue();
    }
}

} /* end namespace SOA4CoRE */
