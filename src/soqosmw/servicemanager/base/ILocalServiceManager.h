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

#ifndef __SOQOSMW_ILOCALSERVICEMANAGER_H_
#define __SOQOSMW_ILOCALSERVICEMANAGER_H_

#include "soqosmw/service/base/IServiceIdentifier.h"

namespace SOQoSMW {

/**
 * @brief Interface for the local service manager module
 *
 * @ingroup soqosmw/servicemanager
 *
 * @author Mehmet Cakir
 */
class ILocalServiceManager
{
public:
  virtual ~ILocalServiceManager();
  virtual void subscribeService(IServiceIdentifier& subscriberServiceIdentifier, IServiceIdentifier& publisherServiceIdentifier) = 0;

};

} /*end namespace SOQoSMW*/

#endif
