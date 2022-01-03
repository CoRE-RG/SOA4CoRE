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

#include "soqosmw/discovery/static/StaticServiceDiscovery.h"
#include "soqosmw/serviceregistry/localserviceregistry/LocalServiceRegistry.h"
//INET
#include "inet/networklayer/common/L3AddressResolver.h"
//STD
#include <iostream>
#include <vector>


namespace SOQoSMW{

using namespace std;
using namespace inet;

Define_Module(StaticServiceDiscovery);

StaticServiceDiscovery::~StaticServiceDiscovery() {
}

void StaticServiceDiscovery::initialize(int stage)
{
    if(stage == INITSTAGE_ROUTING_PROTOCOLS) {
        IServiceDiscovery::_serviceOfferSignal = omnetpp::cComponent::registerSignal("serviceOfferSignal");
        EV_DEBUG << "Initialising SD:";
        cXMLElement *config = par("services").xmlValue();
        EV_DEBUG << " read config: " << config->str();
        //navigate to services and get all entries
        cXMLElementList services = config->getChildrenByTagName("service");
        if(services.empty()){
            EV_DEBUG << " --> list is empty";
        }
        for (size_t i = 0; i < services.size(); i++){
            //check each element and get the service name and the node
            EV_DEBUG << " --> element " << i << ": ";
            cXMLElement* service = services[i];
            const char* node = service->getAttribute("node");
            int id = atoi(service->getAttribute("id"));
            int port = atoi(service->getAttribute("port"));

            const char* qosGroupsStr = service->getAttribute("qosGroups");
            std::set<QoSGroup> qosGroups;
            const char* delim = " ";
            char *token = strtok(const_cast<char*>(qosGroupsStr), delim);
            while (token != nullptr)
            {
                QoSGroup qosGroup;
                if (std::string(token) == "STD_TCP") {
                    qosGroup = QoSGroup::STD_TCP;
                } else if (std::string(token) == "STD_UDP") {
                    qosGroup = QoSGroup::STD_UDP;
                } else if (std::string(token) == "SOMEIP_TCP") {
                    qosGroup = QoSGroup::SOMEIP_TCP;
                } else if (std::string(token) == "SOMEIP_UDP") {
                    qosGroup = QoSGroup::SOMEIP_UDP;
                } else if (std::string(token) == "RT") {
                    qosGroup = QoSGroup::RT;
                } else if (std::string(token) == "WEB") {
                    throw cRuntimeError("WEB QoS is not implemented yet.");
                }
                qosGroups.insert(qosGroup);
                token = strtok(nullptr, delim);
            }

            EV_DEBUG << node;

            //ressolve the address
            const inet::L3Address address = inet::L3AddressResolver().resolve(node);

            //add entry to map
            _discoveryAbstractionMap[id] = PublisherApplicationInformation(id, address, 0xFFFF, qosGroups, port, port);
            //_servicesInNetwork[ServiceIdentifier(id,name)] = new ServiceBase(name, id, address, port);
        }
        EV_DEBUG << endl;
    }

}

void StaticServiceDiscovery::discover(QoSServiceIdentifier qosServiceIdentifier) {
    Enter_Method("SD::discover()");
    if (!(_discoveryAbstractionMap.count(qosServiceIdentifier.getServiceId()))) {
        throw cRuntimeError("The publisher you are requesting is unknown and has no entry in the ServiceRegistry.");
    }
    PublisherApplicationInformation publisherApplicationInformation = _discoveryAbstractionMap[qosServiceIdentifier.getServiceId()];

    PublisherApplicationInformation* publisherApplicationInformationPtr = new PublisherApplicationInformation(publisherApplicationInformation);
    emit(_serviceOfferSignal,publisherApplicationInformationPtr);
}

/*
bool StaticServiceDiscovery::contains(ServiceIdentifier serviceIdentifier) {
    Enter_Method("SD::contains()");
    return _servicesInNetwork.count(serviceIdentifier)>0;
}
*/

void StaticServiceDiscovery::handleMessage(cMessage *msg) {
    // TODO - Generated method body
}

} /*end namespace SOQoSMW*/
