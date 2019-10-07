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

#ifndef __SOQOSMW_STDSUBSCRIBERENDPOINTBASE_H_
#define __SOQOSMW_STDSUBSCRIBERENDPOINTBASE_H_

#include "soqosmw/endpoints/subscriber/base/SubscriberEndpointBase.h"

namespace SOQoSMW {

/**
 * @brief The STDSubscriberEndpointBase is a subscriber endpoint module with STD qos (@see~EndpointBase).
 * Subscribers are created by the QoSNegotiationProtocol module (@see~QoSNegotiationProtocol)
 * during runtime to connect realise a subscribing endpoint for a service.
 * Subscribers are connected to a specific transport and a subscriber connector module (@see~SubscriberConnector)
 * which loosely couples the subscriber endpoint with a subscriber application (@see~SubscriberAppBase).
 *
 * @ingroup soqosmw/endpoints
 *
 * @author Timo Haeckel for HAW Hamburg
 */
class STDSubscriberEndpointBase : public SubscriberEndpointBase
{
};

} /*end namespace SOQoSMW*/

#endif
