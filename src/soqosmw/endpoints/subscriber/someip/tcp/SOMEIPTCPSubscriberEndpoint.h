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

#ifndef __SOQOSMW_SOMEIPTCPSUBSCRIBERENDPOINT_H_
#define __SOQOSMW_SOMEIPTCPSUBSCRIBERENDPOINT_H_

#include "soqosmw/endpoints/subscriber/standard/tcp/TCPSubscriberEndpoint.h"

namespace SOQoSMW {

/**
 * @author Mehmet Cakir
 */
class SOMEIPTCPSubscriberEndpoint : public TCPSubscriberEndpoint
{
    /**
     * Methods
     */
public:
    /**
     * Creates a connection specific information for this endpoint.
     * @return  the connection specific information.
     */
    virtual ConnectionSpecificInformation* getConnectionSpecificInformation() override;
protected:
private:

    /**
     * Member variables
     */
public:
protected:
private:
};

} /* end namespace SOQoSMW */

#endif
