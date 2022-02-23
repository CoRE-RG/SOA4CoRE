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

#ifndef __SOA4CORE_DISCOVERY_BASE_ISERVICEDISCOVERY_H_
#define __SOA4CORE_DISCOVERY_BASE_ISERVICEDISCOVERY_H_

#include <omnetpp.h>

#include <soa4core/serviceidentifier/ServiceIdentifier.h>

namespace SOA4CoRE {

/**
 * @brief Interface for the service discovery module to switch from dynamic to static service discovery.
 *
 * @ingroup soa4core/discovery
 *
 * @author Timo Haeckel and Mehmet Mueller for HAW Hamburg
 */
class IServiceDiscovery {

/**
 * Methods
 */
public:
    /**
     * Initiates the service discovery for the given service identifier
     * @param serviceIdentifier the service identifier
     */
    virtual void discover(ServiceIdentifier serviceIdentifier) = 0;
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

} /*end namespace SOA4CoRE*/

#endif
