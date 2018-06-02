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

#include <applications/base/SOQoSMWApplicationBase.h>
#include <connector/pubsub/reader/SubscriptionReader.h>
#include <endpoints/base/IEndpoint.h>
#include <omnetpp/cmessage.h>
#include <algorithm>
#include <iterator>

using namespace std;
using namespace omnetpp;
namespace soqosmw {

SubscriptionReader::SubscriptionReader(SOQoSMWApplicationBase* executingApplication, std::unordered_map<std::string, IQoSPolicy*> qos) :
                IConnector(executingApplication), _qos(qos){

}

SubscriptionReader::~SubscriptionReader() {
    // TODO Auto-generated destructor stub
}



void SubscriptionReader::put(cPacket* packet) {
    //todo forward to application gate if notify is active.
    getExecutingApplication()->notify(packet);
    //todo save otherwise.
}

void SubscriptionReader::addSubscriber(ISubscriber* subscriber) {
    //check if not already in the list, then add.
    auto it = find(_subscribers.begin(), _subscribers.end(), subscriber);
    if (it == _subscribers.end()){
        _subscribers.push_back(subscriber);
    }
}

omnetpp::cPacket* SubscriptionReader::get() {
    return nullptr;
}

void SubscriptionReader::removeSubscriber(ISubscriber* subscriber) {
    //check if in the list, then remove.
    auto it = find(_subscribers.begin(), _subscribers.end(), subscriber);
    if (it == _subscribers.end()){
        _subscribers.erase(it);
    }
}

IQoSPolicy* SubscriptionReader::getQoSValueFor(std::string property) {
    return _qos[property];
}


void SubscriptionReader::notify(cMessage* msg) {
    for (auto sub : _subscribers){
        sub->notify(msg->dup());
    }
    delete msg;
}


} /*end namespace soqosmw*/
