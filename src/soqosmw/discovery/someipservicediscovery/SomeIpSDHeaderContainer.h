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

#ifndef SOQOSMW_DISCOVERY_SOMEIPSERVICEDISCOVERY_SOMEIPSDHEADERCONTAINER_H_
#define SOQOSMW_DISCOVERY_SOMEIPSERVICEDISCOVERY_SOMEIPSDHEADERCONTAINER_H_

#include <omnetpp.h>
#include "soqosmw/messages/someip/SomeIpSDHeader.h"
#include "soqosmw/messages/someip/SomeIpSDEntry_m.h"
#include "soqosmw/service/base/IService.h"

namespace SOQoSMW {

/**
 * @brief This class provides a container for the SOME/IP SD header, its current investigated
 * SOME/IP SD entry and an optional Service
 *
 * @ingroup soqosmw/discovery/someipservicediscovery
 *
 * @author Mehmet Cakir
 */
class SomeIpSDHeaderContainer : public omnetpp::cObject{
/**
 * Methods
 */
public:
    SomeIpSDHeaderContainer(SomeIpSDEntry& someIpSDEntry, SomeIpSDHeader& someIpSDHeader);
    virtual ~SomeIpSDHeaderContainer();

    /**
     * Getter for the SOME/IP SD Entry
     * @return the SOME/IP SD Entry
     */
    SomeIpSDEntry& getSomeIpSdEntry() const;

    /**
     * Getter for the SOME/IP SD Header
     * @return the SOME/IP SD Header
     */
    SomeIpSDHeader& getSomeIpSdHeader() const;

    /**
     * Sets the service
     * @param service
     */
    void setService(IService* service);

    /**
     * Getter for the service
     * @return the service
     */
    IService* getService() const;



protected:
private:
    /**
     * The SOME/IP SD entry
     */
    SomeIpSDEntry& _someIpSDEntry;

    /**
     * The SOME/IP SD Header
     */
    SomeIpSDHeader& _someIpSDHeader;

    /**
     * The service
     */
    IService* _service;
/**
 * Member variables
 */
public:
protected:
private:
};
} /* end namespace SOQoSMW */
#endif /* SOQOSMW_DISCOVERY_SOMEIPSERVICEDISCOVERY_SOMEIPSDHEADERCONTAINER_H_ */
