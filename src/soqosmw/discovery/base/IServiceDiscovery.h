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

#include <soqosmw/service/serviceidentifier/ServiceIdentifier.h>
#include <omnetpp.h>

namespace SOQoSMW {

/**
 * @brief Interface for the service discovery module of soqosmw to switch from dynamic to static service discovery.
 *
 * @ingroup soqosmw/discovery
 *
 * @author Timo Haeckel and Mehmet Cakir
 */
class IServiceDiscovery {
  public:
    virtual void discover(ServiceIdentifier serviceIdentifier) = 0;

  protected:
    static omnetpp::simsignal_t serviceFoundSignal;
};

} /*end namespace SOQoSMW*/

#endif
