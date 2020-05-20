/*
 * SubscriptionRelations.cpp
 *
 *  Created on: May 20, 2020
 *      Author: mehkir
 */

#include <soqosmw/servicemanager/someipservicemanager/SubscriptionRelations.h>
#include <soqosmw/applications/publisherapp/someip/SomeIpPublisher.h>
#include <soqosmw/applications/subscriberapp/someip/SomeIpSubscriber.h>

namespace SOQoSMW {

SubscriptionRelations::SubscriptionRelations() {
}

SubscriptionRelations::~SubscriptionRelations() {
}

void SubscriptionRelations::addLocalSubscriptionRelation(SomeIpPublisher* someIpPublisher, SubscriptionState subscriptionState) {
    _subscriptionRelationsLocal[someIpPublisher] = subscriptionState;
}

void SubscriptionRelations::addRemoteSubscriptionRelation(std::pair<inet::L3Address, uint16_t> remotePublisherEndpoint, SubscriptionState subscriptionState) {
    _subscriptionRelationsRemote[remotePublisherEndpoint] = subscriptionState;
}

SubscriptionState SubscriptionRelations::getLocalRelation(SomeIpPublisher* someIpPublisher) {
    return _subscriptionRelationsLocal[someIpPublisher];
}

SubscriptionState SubscriptionRelations::getRemoteRelation(std::pair<inet::L3Address, uint16_t> remotePublisherEndpoint) {
    return _subscriptionRelationsRemote[remotePublisherEndpoint];
}

} /* namespace SOQoSMW */
