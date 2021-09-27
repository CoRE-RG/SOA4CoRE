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

#ifndef SOQOSMW_APPLICATIONS_SOMEIPAPP_BASE_ISOMEIPAPPBASE_H_
#define SOQOSMW_APPLICATIONS_SOMEIPAPP_BASE_ISOMEIPAPPBASE_H_

#include <stdint.h>

namespace SOQoSMW {

/**
 * @brief Interface for SOME/IP applications.
 *
 * @ingroup soqosmw/applications/someipapp/base/
 *
 * @author Mehmet Cakir
 */

class ISomeIpAppBase {
public:
    virtual ~ISomeIpAppBase(){}

    /**
     * Returns the ID of the service instance
     *
     * @return instance ID
     */
    virtual uint16_t getInstanceId() = 0;
};

} /* end namespace SOQoSMW */

#endif /* SOQOSMW_APPLICATIONS_SOMEIPAPP_BASE_ISOMEIPAPPBASE_H_ */
