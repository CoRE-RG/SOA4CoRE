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

#include <soqosmw/applications/subscriberapp/someip/SomeIpSubscriber.h>
#include "soqosmw/service/someipserviceidentifier/SomeIpServiceIdentifier.h"
#include "soqosmw/servicemanager/someipservicemanager/SomeIpLocalServiceManager.h"
namespace SOQoSMW {

#define START_MSG_NAME "Start Message"

Define_Module(SomeIpSubscriber);

void SomeIpSubscriber::initialize() {
    SubscriberAppBase::initialize();
    _instanceId = par("instanceID").intValue();
}

SomeIpSubscriber::SomeIpSubscriber() : SubscriberAppBase() {

}

SomeIpSubscriber::~SomeIpSubscriber() {

}

void SomeIpSubscriber::handleMessage(cMessage *msg)
{
    if(msg->isSelfMessage() && (strcmp(msg->getName(), START_MSG_NAME) == 0)){
        setQoS();
        //create a subscriber
        SomeIpLocalServiceManager* localServiceManager = dynamic_cast<SomeIpLocalServiceManager*>(_localServiceManager);
        localServiceManager->registerSubscriberService(this->_publisherServiceId, this->_qosPolicies, this);
        _connector = localServiceManager->getSubscriberConnector(this->_publisherServiceId);
        SomeIpServiceIdentifier publisherServiceIdentifier = SomeIpServiceIdentifier(this->_publisherServiceId,this->_instanceId);
        localServiceManager->subscribeQoSService(publisherServiceIdentifier, this->_qosPolicies);
        if (getEnvir()->isGUI()) {
            getDisplayString().setTagArg("i2", 0, "status/active");
        }

    } else {
        EV_DEBUG << "Subscriber " << _subscriberName << " received a message."  << endl;
        //this is a subscription message so handle it.
        if (omnetpp::cPacket *frame = dynamic_cast<omnetpp::cPacket*>(msg))
        {
            emit(_rxPkSignal, frame);
        }
    }
    delete msg;
}

uint16_t SomeIpSubscriber::getInstanceId() {
    return _instanceId;
}

} /* end namespace SOQoSMW */
