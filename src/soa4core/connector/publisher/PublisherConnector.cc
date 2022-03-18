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

#include "soa4core/connector/publisher/PublisherConnector.h"
//STD
#include <algorithm>
#include <fstream>

namespace SOA4CoRE {

Define_Module(PublisherConnector);

void PublisherConnector::initialize() {
    handleParameterChange(nullptr);
    this->_forwardedToEndpointsSignal = registerSignal("forwardedToEPs");
    this->_messageDroppedSignal = registerSignal("messageDropped");
}

void PublisherConnector::handleMessage(cMessage *msg)
{
    bool forwarded = false;
    if (msg->arrivedOn("applicationIn")){
        //from application --> forward to all endpoints
        for(auto endpoint : _endpoints) {
            sendDirect(msg->dup(), endpoint->gate("connectorIn"));
            forwarded = true;
        }
    }

    if (forwarded){
        // signal forwardedToEndpoints
        emit(this->_forwardedToEndpointsSignal, msg);
    } else {
        //message was dropped so emit.
        emit(this->_messageDroppedSignal,msg);
    }
    delete msg;
}

void PublisherConnector::handleParameterChange(const char* parname) {
    ConnectorBase::handleParameterChange(parname);
    if (!parname || !strcmp(parname, "maxEndpoints")) {
        _maxEndpoints = par("maxEndpoints");
    }
}

bool PublisherConnector::addEndpoint(PublisherEndpointBase* endpoint) {
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

PublisherEndpointBase* PublisherConnector::removeEndpoint(PublisherEndpointBase* endpoint) {
    //check if in the list, then remove.
    if(endpoint){
        auto it = find(_endpoints.begin(), _endpoints.end(), endpoint);
        if (it != _endpoints.end()){
            PublisherEndpointBase* temp = *it;
            _endpoints.erase(it);
            return temp;
        }
    }
    return nullptr;
}

void PublisherConnector::setApplication(ServiceBase* publisherApplication) {
    _publisherApplication = publisherApplication;
}

ServiceBase* PublisherConnector::getApplication() {
    return _publisherApplication;
}

const std::vector<PublisherEndpointBase*>& PublisherConnector::getEndpoints() const {
    return _endpoints;
}

void PublisherConnector::finish(){
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
        ss << "\"" << _publisherApplication->getServiceId() << "\"";
        ss << "]";
        ss << ",";
        ss << "\"connectorName\":" << "\"" << this->getFullName() << "\""; // connector name
        int listLength = this->_endpoints.size();
        ss << ",";
        ss << "\"endpoints\":[";
        int elemCounter = 0;
        for (PublisherEndpointBase* endpoint : this->_endpoints) {
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
        outfile.open("publisherconnectorsmapping.txt", std::ios::app);
        outfile << ss.str() << std::endl;
        outfile.close();
        mutex.unlock();
    }
}

} /*end namespace SOA4CoRE*/
