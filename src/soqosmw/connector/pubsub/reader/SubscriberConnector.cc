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

#include "soqosmw/connector/pubsub/reader/SubscriberConnector.h"
#include <algorithm>
#include <fstream>

namespace SOQoSMW {

Define_Module(SubscriberConnector);

void SubscriberConnector::initialize() {
    handleParameterChange(nullptr);
    this->_forwardedToApplicationsSignal = registerSignal("forwardedToApps");
    this->_messageDroppedSignal = registerSignal("messageDropped");
}

void SubscriberConnector::handleMessage(cMessage *msg)
{
    bool forwarded = false;
    if (msg->arrivedOn("endpointIn")) {
        //from endpoint --> forward to applications
        for(auto application : _applications) {
            sendDirect(msg->dup(), application->gate("connectorIn"));
            forwarded = true;
        }
    }

    if (forwarded){
        // signal forwardedToApplications
        emit(this->_forwardedToApplicationsSignal, msg);
    } else {
        //message was dropped so emit.
        emit(this->_messageDroppedSignal,msg);
    }
    delete msg;
}

void SubscriberConnector::handleParameterChange(const char* parname) {
    ConnectorBase::handleParameterChange(parname);
    if (!parname || !strcmp(parname, "maxApplications")) {
        _maxApplications = par("maxApplications");
    }
}

bool SubscriberConnector::addApplication(SOQoSMWApplicationBase* soqosMwApplicationBase) {
    if(soqosMwApplicationBase && (_maxApplications < 0 || (int)_applications.size() < _maxApplications)){
        //check if not already in the list, then add.
        auto it = find(_applications.begin(), _applications.end(), soqosMwApplicationBase);
        if (it == _applications.end()){
            _applications.push_back(soqosMwApplicationBase);
            return true;
        }
    }
    return false;
}

SOQoSMWApplicationBase* SubscriberConnector::removeApplication(
        SOQoSMWApplicationBase* soqosMwApplicationBase) {
    //check if in the list, then remove.
    if(soqosMwApplicationBase){
        auto it = find(_applications.begin(), _applications.end(), soqosMwApplicationBase);
        if (it == _applications.end()){
            SOQoSMWApplicationBase* temp = *it;
            _applications.erase(it);
            return temp;
        }
    }
    return nullptr;
}

SubscriberApplicationInformation SubscriberConnector::getSubscriberApplicationInformation(){
    return _subscriberApplicationInformation;
}

void SubscriberConnector::setSubscriberApplicationInformation(SubscriberApplicationInformation subscriberApplicationInformation) {
    _subscriberApplicationInformation = subscriberApplicationInformation;
}


const std::vector<SOQoSMWApplicationBase*>& SubscriberConnector::getApplications() const {
    return _applications;
}

SubscriberEndpointBase* SubscriberConnector::getEndpoint() {
    return _subscriberEndpointBase;
}

void SubscriberConnector::setEndpoint(SubscriberEndpointBase* subscriberEndpointBase) {
    _subscriberEndpointBase = subscriberEndpointBase;
}

void SubscriberConnector::finish(){
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
        ss << ",";
        ss << "\"endpoint\":[";
        ss << "{\"" << "name" << "\":\"" << _subscriberEndpointBase->getFullName() << "\",\"" << "created" << "\":\"" << _subscriberEndpointBase->getCreationTime() << "\"}" ;
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
