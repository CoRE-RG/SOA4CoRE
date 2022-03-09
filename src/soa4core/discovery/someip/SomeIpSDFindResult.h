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

#ifndef SOA4CORE_DISCOVERY_SOMEIP_SOMEIPSDFINDRESULT_H_
#define SOA4CORE_DISCOVERY_SOMEIP_SOMEIPSDFINDRESULT_H_

#include "soa4core/applicationinformation/publisher/PublisherApplicationInformation.h"

using namespace inet;
namespace SOA4CoRE {

/**
 * @brief This class provides a container for results resulting from SOME/IP SD header find entry
 * that is used as a notification object.
 *
 * @ingroup soa4core/discovery
 *
 * @author Mehmet Mueller for HAW Hamburg
 */
class SomeIpSDFindResult : public cObject, noncopyable {
/**
 * Methods
 */
public:
    SomeIpSDFindResult(L3Address remoteAddress, PublisherApplicationInformation publisherApplicationInformation);
    virtual ~SomeIpSDFindResult();

    /**
     * Getter for the remote ip address
     * @return the remote address
     */
    inet::L3Address getRemoteAddress() const;

    /**
     * Getter for the PublisherApplicationInformation
     * @return the PublisherApplicationInformation
     */
    PublisherApplicationInformation getPublisherApplicationInformation() const;
protected:
private:

/**
 * Member variables
 */
public:
protected:
private:
    /**
     * The remote ip address
     */
    inet::L3Address _remoteAddress;

    /**
     * The service
     */
    PublisherApplicationInformation _publisherApplicationInformation;
};
} /* end namespace SOA4CoRE */
#endif /* SOA4CORE_DISCOVERY_SOMEIP_SOMEIPSDFINDRESULT_H_ */
