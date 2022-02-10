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

#ifndef __SOA4CORE_TEST_DISCOVERY_STATIC_STATICSERVICEDISCOVERYTEST_H_
#define __SOA4CORE_TEST_DISCOVERY_STATIC_STATICSERVICEDISCOVERYTEST_H_

#include <omnetpp.h>
#include "soa4core/serviceregistry/base/IServiceRegistry.h"

using namespace omnetpp;

namespace SOA4CoRE {

/**
 * @brief The StaticServiceDiscoveryTest.
 *
 * @ingroup soa4core/test
 *
 * @author Timo Haeckel for HAW Hamburg
 */
class StaticServiceDiscoveryTest : public cSimpleModule
{
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

  private:
    IServiceRegistry* _sr;
};

} /*end namespace SOA4CoRE*/

#endif
