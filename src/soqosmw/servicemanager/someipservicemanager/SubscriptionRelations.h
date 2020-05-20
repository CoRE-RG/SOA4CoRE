/*
 * SubscriptionRelations.h
 *
 *  Created on: May 20, 2020
 *      Author: mehkir
 */

#ifndef SOQOSMW_SERVICEMANAGER_SOMEIPSERVICEMANAGER_SUBSCRIPTIONRELATIONS_H_
#define SOQOSMW_SERVICEMANAGER_SOMEIPSERVICEMANAGER_SUBSCRIPTIONRELATIONS_H_

#include <map>
#include <utility>
#include "inet/networklayer/common/L3Address.h"

namespace SOQoSMW {

enum class SubscriptionState {NOT_SUBSCRIBED, SUBSCRIBING, SUBSCRIBED};

class SomeIpSubscriber;
class SomeIpPublisher;

class SubscriptionRelations {

/**
 * Member variables
 */
public:
    /**
     * Adds a subscription relation to a local publisher
     * @param someIpPublisher
     * @param subscriptionState
     */
    void addLocalSubscriptionRelation(SomeIpPublisher* someIpPublisher, SubscriptionState subscriptionState);

    /**
     * Adds a subscription relation to a remote publisher
     * @param pair<inet::L3Address, uint16_t>
     * @param subscriptionState
     */
    void addRemoteSubscriptionRelation(std::pair<inet::L3Address, uint16_t> remotePublisherEndpoint, SubscriptionState subscriptionState);

    /**
     * Returns a local subscription relation
     * @param someIpPublisher
     * @return local subscription relation
     */
    SubscriptionState getLocalRelation(SomeIpPublisher* someIpPublisher);

    /**
     * Returns a remote subscription relation
     * @param remotePublisherEndpoint
     * @return remote subscription relation
     */
    SubscriptionState getRemoteRelation(std::pair<inet::L3Address, uint16_t> remotePublisherEndpoint);

protected:
private:


/**
 * Methods
 */
public:
    SubscriptionRelations();
    virtual ~SubscriptionRelations();
protected:
private:
    /**
     * Subscription state for local publishers
     */
    std::map<SomeIpPublisher*, SubscriptionState> _subscriptionRelationsLocal;

    /**
     * Subscription state for remote publishers
     */
    std::map<std::pair<inet::L3Address, uint16_t>, SubscriptionState> _subscriptionRelationsRemote;
};

} /* namespace SOQoSMW */

#endif /* SOQOSMW_SERVICEMANAGER_SOMEIPSERVICEMANAGER_SUBSCRIPTIONRELATIONS_H_ */
