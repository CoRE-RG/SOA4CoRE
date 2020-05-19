/*
 * ISomeIpServiceApp.h
 *
 *  Created on: May 19, 2020
 *      Author: mehkir
 */

#ifndef SOQOSMW_APPLICATIONS_SOMEIPAPP_BASE_ISOMEIPSERVICEAPP_H_
#define SOQOSMW_APPLICATIONS_SOMEIPAPP_BASE_ISOMEIPSERVICEAPP_H_

#include "inet/networklayer/common/L3Address.h"

namespace SOQoSMW {

class ISomeIpServiceApp {
public:
    virtual ~ISomeIpServiceApp(){}
    /**
     * Returns the IP Address
     *
     * @param adressType
     * @return local IP Address
     */
    virtual inet::L3Address getIpAddress(inet::L3Address::AddressType adressType) = 0;

    /**
     * Returns the port
     *
     * @return local Port
     */
    virtual uint16_t getPort() = 0;

    /**
     * Returns the ID of the service
     *
     * @return service ID to subscribe
     */
    virtual uint16_t getServiceID() = 0;

    /**
     * Returns the ID of the service instance
     *
     * @return instance ID
     */
    virtual uint16_t getInstanceID() = 0;
};

} /* namespace SOQoSMW */

#endif /* SOQOSMW_APPLICATIONS_SOMEIPAPP_BASE_ISOMEIPSERVICEAPP_H_ */
