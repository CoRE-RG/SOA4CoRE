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

#ifndef SOA4CORE_DISCOVERY_DISCOVERYNOTIFICATION_H_
#define SOA4CORE_DISCOVERY_DISCOVERYNOTIFICATION_H_

//INET
#include <inet/networklayer/common/L3Address.h>

namespace SOA4CoRE {

/**
 * @brief The DiscoveryNotification is used as a notification class for a found service.
 *
 * @ingroup soa4core/discovery
 *
 * @author Mehmet Mueller
 */

class DiscoveryNotification: public cObject, noncopyable {

    /**
     * Methods
     */
public:
    DiscoveryNotification(inet::L3Address address);
    virtual ~DiscoveryNotification();

    /**
     * Returns the IP address
     * @return the IP address
     */
    inet::L3Address getAddress() const;

protected:
private:

    /**
     * Member variables
     */
public:
protected:
private:
    /**
     * The IP address
     */
    inet::L3Address _address;
};

}

#endif /* SOA4CORE_DISCOVERY_DISCOVERYNOTIFICATION_H_ */
