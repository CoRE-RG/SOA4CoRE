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

#ifndef __SOA4CORE_STDPUBLISHERENDPOINTBASE_H_
#define __SOA4CORE_STDPUBLISHERENDPOINTBASE_H_

#include "soa4core/endpoints/publisher/base/PublisherEndpointBase.h"

namespace SOA4CoRE {

/**
 * @brief The STDPublisherEndpointBase is a publisher endpoint module with STD qos (@see~EndpointBase).
 * Publishers are created by the QoSNegotiationProtocol module (@see~QoSNegotiationProtocol)
 * during runtime to connect realise a publishing endpoint for a service.
 * Publishers are connected to a specific transport and a publisher connector module (@see~PublisherConnector)
 * which loosely couples the publisher endpoint with a publisher application (@see~Publisher).
 *
 * @ingroup soa4core/endpoints
 *
 * @author Timo Haeckel and Mehmet Mueller for HAW Hamburg
 */
class STDPublisherEndpointBase : public PublisherEndpointBase
{
/**
 * Methods
 */
public:
protected:
    virtual void handleParameterChange(const char* parname) override;
private:

/**
 * Member variables
 */
public:
protected:

    /**
     * Local ip address for UDP socket
     */
    std::string _localAddress;

    /**
     * Local port for UDP socket
     */
    int _localPort;
private:
};

} /*end namespace SOA4CoRE*/

#endif
