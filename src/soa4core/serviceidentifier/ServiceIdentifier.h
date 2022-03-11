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

#ifndef SOA4CORE_SERVICEIDENTIFIER_SERVICEIDENTIFIER_H_
#define SOA4CORE_SERVICEIDENTIFIER_SERVICEIDENTIFIER_H_


namespace SOA4CoRE {

/**
 * @brief The service identifier used to discover a service
 *
 * @ingroup soa4core/service
 *
 * @author Mehmet Mueller for HAW Hamburg
 */

class ServiceIdentifier {
public:
    ServiceIdentifier();

    /**
     * Contructor
     * @param serviceId the service ID
     * @param instanceId the instance ID
     */
    ServiceIdentifier(int serviceId, int instanceId);
    virtual ~ServiceIdentifier();
    virtual bool operator==(const ServiceIdentifier& serviceIdentifier) const;
    virtual bool operator!=(const ServiceIdentifier& serviceIdentifier) const;

    /**
     * Returns the service ID
     * @return the service ID
     */
    int getServiceId() const;

    /**
     * Returns the instance ID
     * @return the instance ID
     */
    int getInstanceId() const;

protected:
    /**
     * The service ID
     */
    int _serviceId;

    /**
     * The instance ID
     */
    int _instanceId;
};
} /* namespace SOA4CoRE */

#endif /* SOA4CORE_SERVICEIDENTIFIER_SERVICEIDENTIFIER_H_ */
