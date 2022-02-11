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

#include <soa4core/applicationinformation/publisher/PublisherApplicationInformationNotification.h>
#include <soa4core/registry/Registry.h>
#include "soa4core/discovery/static/StaticServiceDiscovery.h"
#include "inet/networklayer/common/L3AddressResolver.h"
//STD
#include <iostream>
#include <vector>


namespace SOA4CoRE{

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
            EV_DEBUG << node;

            //ressolve the address
            const inet::L3Address address = inet::L3AddressResolver().resolve(node);

            //add entry to map
            if (!_discoveryAbstractionMap.count(id)) {
                _discoveryAbstractionMap[id] = PublisherApplicationInformation(id, address);
            } else {
                throw cRuntimeError("There can not be multiple publishers with the same service id.");
            }
        }
        EV_DEBUG << endl;
    }

}

void StaticServiceDiscovery::discover(ServiceIdentifier serviceIdentifier) {
    Enter_Method("SD::discover()");
    if (!(_discoveryAbstractionMap.count(serviceIdentifier.getServiceId()))) {
        throw cRuntimeError("The publisher you are requesting is unknown and has no entry in the ServiceRegistry.");
    }
    PublisherApplicationInformation publisherApplicationInformation = _discoveryAbstractionMap[serviceIdentifier.getServiceId()];
    PublisherApplicationInformationNotification* publisherApplicationInformationNotification = new PublisherApplicationInformationNotification(publisherApplicationInformation);
    emit(_serviceOfferSignal, publisherApplicationInformationNotification);
}

void StaticServiceDiscovery::handleMessage(cMessage *msg) {
    // TODO - Generated method body
}

} /*end namespace SOA4CoRE*/
