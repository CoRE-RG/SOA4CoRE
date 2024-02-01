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

using namespace omnetpp;

namespace SOA4CoRE {

/**
 * @brief The DiscoveryNotification is used as a notification class for a found service.
 *
 * @ingroup soa4core/discovery
 *
 * @author Mehmet Mueller
 */

class DiscoveryNotification: public cObject {

    /**
     * Methods
     */
public:
    /**
     * Constructor
     * @param serviceId the service id of the application
     * @param address the IP address of the node where the application resides
     */
    DiscoveryNotification(int serviceId, inet::L3Address address);
    /**
     * Default Constructor. Set all values manually!
     */
    DiscoveryNotification();
    virtual ~DiscoveryNotification();

    /**
     * Copy constructor
     * @param discoveryNotification the other discovery notification
     */
    DiscoveryNotification(const DiscoveryNotification& discoveryNotification);

    /**
     * Assignment operator
     * @param other the discovery notification which is assigned to this discovery notification
     * @return this discovery notification
     */
    DiscoveryNotification& operator=(const DiscoveryNotification& other);

    /**
     * Returns a duplicate of this discovery notification
     * @return a duplicate of this discovery notification
     */
    virtual DiscoveryNotification* dup() const override;

    /**
     * Returns the service ID
     * @return the service ID
     */
    int getServiceId() const;

    /**
     * Returns the IP address
     * @return the IP address
     */
    inet::L3Address getAddress() const;

    void setAddress(const inet::L3Address& address);
    void setServiceId(int serviceId);

    virtual bool operator==(const DiscoveryNotification& discoveryNotification) const;
    virtual bool operator!=(const DiscoveryNotification& discoveryNotification) const;

protected:
private:

    /**
     * Member variables
     */
public:
protected:
    /**
     * The service ID
     */
    int _serviceId;

    /**
     * The IP address
     */
    inet::L3Address _address;
private:
};

} /* end namespace SOA4CoRE */

#endif /* SOA4CORE_DISCOVERY_DISCOVERYNOTIFICATION_H_ */
