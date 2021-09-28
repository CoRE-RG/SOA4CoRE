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

#include "soqosmw/service/base/IService.h"
#include "soqosmw/discovery/someipservicediscovery/SomeIpSDFindRequest.h"

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
    SomeIpSDFindResult(int serviceId, int instanceId, inet::L3Address remoteAddress, IService *service);
    virtual ~SomeIpSDFindResult();

    /**
     * Getter for the service
     * @return the service
     */
    IService* getService() const;

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
    IService* _service;
};
} /* end namespace SOQoSMW */
#endif /* SOQOSMW_DISCOVERY_SOMEIPSERVICEDISCOVERY_SOMEIPSDFINDRESULT_H_ */
