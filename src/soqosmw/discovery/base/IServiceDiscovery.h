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

#ifndef __SOQOSMW_DISCOVERY_BASE_ISERVICEDISCOVERY_H_
#define __SOQOSMW_DISCOVERY_BASE_ISERVICEDISCOVERY_H_

#include <omnetpp.h>
#include "soqosmw/service/qosserviceidentifier/QoSServiceIdentifier.h"

namespace SOQoSMW {

/**
 * @brief Interface for the service discovery module of soqosmw to switch from dynamic to static service discovery.
 *
 * @ingroup soqosmw/discovery
 *
 * @author Timo Haeckel and Mehmet Cakir
 */
class IServiceDiscovery {

    /**
     * Methods
     */
public:
    virtual void discover(QoSServiceIdentifier qosServiceIdentifier) = 0;
protected:
private:

    /**
     * Member variables
     */
public:

protected:
    /**
     * The signal which is emitted when a offer is received
     */
    omnetpp::simsignal_t _serviceOfferSignal;
private:

};

} /*end namespace SOQoSMW*/

#endif
