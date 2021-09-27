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

#ifndef __SOQOSMW_SOMEIPPUBLISHER_H_
#define __SOQOSMW_SOMEIPPUBLISHER_H_

#include <soqosmw/applications/someipapp/base/ISomeIpAppBase.h>
#include "soqosmw/applications/publisherapp/base/PublisherAppBase.h"

namespace SOQoSMW {

/**
 * @brief A simple SOME/IP Publisher application that sends a dummy SOME/IP message.
 *
 * @ingroup soqosmw/applications
 *
 * @author Mehmet Cakir
 */
class SomeIpPublisher : public virtual ISomeIpAppBase, public virtual PublisherAppBase  {
    /**
     * Methods
     */
public:
    SomeIpPublisher();
    ~SomeIpPublisher();

    /**
     * Returns the ID of the service instance to publish
     *
     * @return instance ID to publish
     */
    virtual uint16_t getInstanceId() override;

protected:
    /**
     * Initializes the module
     */
    virtual void initialize() override;

private:
    /**
     * Member variables
     */
public:
protected:
private:
    /**
     * Instance ID of service id to publish
     */
    uint16_t _instanceID;

};
} /* end namespace SOQoSMW */
#endif

