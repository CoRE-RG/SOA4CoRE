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

#ifndef SOQOSMW_DISCOVERY_SOMEIPSERVICEDISCOVERY_SOMEIPSDFINDRESULT_H_
#define SOQOSMW_DISCOVERY_SOMEIPSERVICEDISCOVERY_SOMEIPSDFINDRESULT_H_

#include "soqosmw/service/qosservice/QoSService.h"
#include "soqosmw/discovery/someipservicediscovery/SomeIpSDFindRequest.h"
#include "inet/networklayer/common/IPProtocolId_m.h"

using namespace inet;
namespace SOQoSMW {

/**
 * @brief This class provides a container for results resulting from SOME/IP SD header find entry
 *
 * @ingroup soqosmw/discovery/someipservicediscovery
 *
 * @author Mehmet Cakir
 */
class SomeIpSDFindResult : public SomeIpSDFindRequest {
/**
 * Methods
 */
public:
    SomeIpSDFindResult(int serviceId, int instanceId, L3Address remoteAddress, QoSService service, IPProtocolId ipProtocolId);
    virtual ~SomeIpSDFindResult();

    /**
     * Getter for the service
     * @return the service
     */
    QoSService getService() const;

    /**
     * Getter for the used L4 Protocol
     * @return the IP Protocol ID of the used L4 protocol
     */
    IPProtocolId getIPProtocolId() const;

protected:
private:
/**
 * Member variables
 */
public:
protected:
private:
    /**
     * The service
     */
    QoSService _service;

    /**
     * The used L4 protocol
     */
    IPProtocolId _ipProtocolId;
};
} /* end namespace SOQoSMW */
#endif /* SOQOSMW_DISCOVERY_SOMEIPSERVICEDISCOVERY_SOMEIPSDFINDRESULT_H_ */
