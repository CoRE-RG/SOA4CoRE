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

#ifndef SOQOSMW_DISCOVERY_SOMEIPSERVICEDISCOVERY_SOMEIPSDSUBSCRIPTIONINFORMATION_H_
#define SOQOSMW_DISCOVERY_SOMEIPSERVICEDISCOVERY_SOMEIPSDSUBSCRIPTIONINFORMATION_H_

#include <stdint.h>
#include <inet/networklayer/common/L3Address.h>

namespace SOQoSMW {

/**
 * @brief This class provides holding the informations needed for subscription
 *
 * @ingroup soqosmw/discovery/someipservicediscovery
 *
 * @author Mehmet Cakir
 */
class SomeIpSDSubscriptionInformation : public cObject{
/**
 * Methods
 */
public:
    SomeIpSDSubscriptionInformation(uint16_t serviceId, uint16_t instanceId, inet::L3Address localAddress, uint16_t localPort, inet::L3Address remoteAddress);
    virtual ~SomeIpSDSubscriptionInformation();

    uint16_t getInstanceId() const;

    const inet::L3Address& getLocalAddress() const;

    uint16_t getLocalPort() const;

    const inet::L3Address& getRemoteAddress() const;

    uint16_t getServiceId() const;

protected:
private:
/**
 * Member variables
 */
public:
protected:
private:
    uint16_t _serviceId;
    uint16_t _instanceId;
    inet::L3Address _localAddress;
    uint16_t _localPort;
    inet::L3Address _remoteAddress;
};

} /* end namespace SOQoSMW */
#endif /* SOQOSMW_DISCOVERY_SOMEIPSERVICEDISCOVERY_SOMEIPSDSUBSCRIPTIONINFORMATION_H_ */
