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

#ifndef SOQOSMW_QOSPOLICY_BASE_QOSPOLICY_H_
#define SOQOSMW_QOSPOLICY_BASE_QOSPOLICY_H_

#include <soqosmw/qospolicy/base/IQoSPolicy.h>

#include <soqosmw/qospolicy/base/types/DoubleQoSPolicy.h>
#include <soqosmw/qospolicy/base/types/IntQoSPolicy.h>
#include <soqosmw/qospolicy/base/types/SizeQoSPolicy.h>
#include <soqosmw/qospolicy/base/types/StringQoSPolicy.h>
#include <soqosmw/qospolicy/base/types/UInt16QoSPolicy.h>

#include <soqosmw/qospolicy/avb/FramesizeQoSPolicy.h>
#include <soqosmw/qospolicy/avb/IntervalFramesQoSPolicy.h>
#include <soqosmw/qospolicy/avb/SRClassQoSPolicy.h>
#include <soqosmw/qospolicy/avb/StreamIDQoSPolicy.h>

#include <soqosmw/qospolicy/management/QoSGroup.h>

#include <soqosmw/qospolicy/std/LocalAddressQoSPolicy.h>
#include <soqosmw/qospolicy/std/LocalPortQoSPolicy.h>

#include <string>
#include <unordered_map>

/**
 * @brief The QoSPolicy.
 *
 * @ingroup soqosmw/qospolicy
 *
 * @author Timo Haeckel & Mehmet Cakir for HAW Hamburg
 */
namespace SOQoSMW {
typedef std::unordered_map<std::string, IQoSPolicy*> QoSPolicyMap;

/**
 * This function checks if the policies in the left and right map are equal.
 * @param left      one policy list
 * @param right     the other policy list
 * @return          left == right
 */
bool equalQoSMap(QoSPolicyMap& left, QoSPolicyMap& right);

} /* end namespace  */
#endif /* SOQOSMW_QOSPOLICY_BASE_QOSPOLICY_H_ */
