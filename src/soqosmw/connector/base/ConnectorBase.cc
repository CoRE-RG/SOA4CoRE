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

#include "ConnectorBase.h"

#include "soqosmw/endpoints/base/EndpointBase.h"
#include "soqosmw/applications/base/SOQoSMWApplicationBase.h"
#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>

namespace SOQoSMW {

void ConnectorBase::initialize()
{
    handleParameterChange(nullptr);
    this->_forwardedToEndpointsSignal = registerSignal("forwardedToEPs");
    this->_forwardedToApplicationsSignal = registerSignal("forwardedToApps");
    this->_messageDroppedSignal = registerSignal("messageDropped");
}

void ConnectorBase::handleMessage(cMessage *msg)
{
    bool forwarded = false;
    if (_endpointFwdEnabled && msg->arrivedOn("applicationIn")){
        //from applications --> forward to all endpoints
        for(auto endpoint : _endpoints) {
            sendDirect(msg->dup(), endpoint->gate("connectorIn"));
            forwarded = true;
        }
        if (forwarded){
            // signal forwardedToEndpoints
            emit(this->_forwardedToEndpointsSignal, msg);
        }

    } else if (_applicationFwdEnabled && msg->arrivedOn("endpointIn")) {
        //from endpoints --> forward to applications
        for(auto application : _applications) {
            sendDirect(msg->dup(), application->gate("connectorIn"));
            forwarded = true;
        }
        if (forwarded){
            // signal forwardedToApplications
            emit(this->_forwardedToApplicationsSignal, msg);
        }
    }

    if(!forwarded){
        //message was dropped so emit.
        emit(this->_messageDroppedSignal,msg);
    }

    delete msg;
}

void ConnectorBase::handleParameterChange(const char* parname) {
    if (!parname || !strcmp(parname, "maxApplications")) {
        _maxApplications = par("maxApplications");
    }
    if (!parname || !strcmp(parname, "maxEndpoints")) {
        _maxEndpoints = par("maxEndpoints");
    }
    if (!parname || !strcmp(parname, "applicationFwdEnabled")) {
        _applicationFwdEnabled = par("applicationFwdEnabled");
    }
    if (!parname || !strcmp(parname, "endpointFwdEnabled")) {
        _endpointFwdEnabled = par("endpointFwdEnabled");
    }
    if (!parname || !strcmp(parname, "createConnectorMappingEnabled")) {
        _createConnectorMappingEnabled = par("createConnectorMappingEnabled");
    }
}

bool ConnectorBase::addEndpoint(EndpointBase* endpoint) {
    if(endpoint && (_maxEndpoints < 0 || (int)_endpoints.size() < _maxEndpoints)) {
        //check if not already in the list, then add.
        auto it = find(_endpoints.begin(), _endpoints.end(), endpoint);
        if (it == _endpoints.end()){
            _endpoints.push_back(endpoint);
            // TODO emit ...
            return true;
        }
    }
    return false;
}

EndpointBase* ConnectorBase::removeEndpoint(EndpointBase* endpoint) {
    //check if in the list, then remove.
    if(endpoint){
        auto it = find(_endpoints.begin(), _endpoints.end(), endpoint);
        if (it == _endpoints.end()){
            EndpointBase* temp = *it;
            _endpoints.erase(it);
            return temp;
        }
    }
    return nullptr;
}

bool ConnectorBase::addApplication(SOQoSMWApplicationBase* application) {
    if(application && (_maxApplications < 0 || (int)_applications.size() < _maxApplications)){ // maxApplications < 0 ?
        //check if not already in the list, then add.
        auto it = find(_applications.begin(), _applications.end(), application);
        if (it == _applications.end()){
            _applications.push_back(application);
            return true;
        }
    }
    return false;
}

SOQoSMWApplicationBase* ConnectorBase::removeApplication(
        SOQoSMWApplicationBase* application) {
    //check if in the list, then remove.
    if(application){
        auto it = find(_applications.begin(), _applications.end(), application);
        if (it == _applications.end()){
            SOQoSMWApplicationBase* temp = *it;
            _applications.erase(it);
            return temp;
        }
    }
    return nullptr;
}

void ConnectorBase::finish(){
    if (_createConnectorMappingEnabled) {
        static std::mutex mutex;
        std::stringstream ss;
        std::string hostname = getParentModule()->getParentModule()->getName();
        ss << "{";
        if (hostname == "tte") {
            ss << "\"gatewayName\":" << "\"" << getParentModule()->getParentModule()->getParentModule()->getFullName() << "\""; // Gateway name
        } else {
            ss << "\"hostName\":" << "\"" << getParentModule()->getParentModule()->getFullName() << "\""; // Host name
        }
        ss << ",";
        ss << "\"applications\":[";
        int elemCounter = 0;
        int listLength = this->_applications.size();
        for (SOQoSMWApplicationBase* application : this->_applications) {
            // additional information on real application name
            ss << "\"" << application->getServiceName() << "\"";
            elemCounter++;
            if (elemCounter != listLength) {
                ss << ",";
            }
        }
        ss << "]";
        ss << ",";
        ss << "\"connectorName\":" << "\"" << this->getFullName() << "\""; // connector name
        listLength = this->_endpoints.size();
        ss << ",";
        ss << "\"endpoints\":[";
        elemCounter = 0;
        for (EndpointBase* endpoint : this->_endpoints) {
            ss << "{\"" << "name" << "\":\"" << endpoint->getFullName() << "\",\"" << "created" << "\":\"" << endpoint->getCreationTime() << "\"}" ;
            elemCounter++;
            if (elemCounter != listLength) {
                ss << ",";
            }
        }
        ss << "]";
        ss << "}";
        mutex.lock();
        std::ofstream outfile;
        outfile.open("connectorsmapping.txt", std::ios::app);
        outfile << ss.str() << std::endl;
        outfile.close();
        mutex.unlock();
    }
}

} /*end namespace SOQoSMW*/
