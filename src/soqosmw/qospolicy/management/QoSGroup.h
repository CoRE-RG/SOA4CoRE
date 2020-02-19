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

#ifndef SOQOSMW_QOSPOLICY_MANAGEMENT_QOSGROUP_H_
#define SOQOSMW_QOSPOLICY_MANAGEMENT_QOSGROUP_H_

#include <soqosmw/messages/qosnegotiation/QoSNegotiationProtocol_m.h>
#include "soqosmw/qospolicy/base/types/IntQoSPolicy.h"


namespace SOQoSMW {

namespace QoSPolicyNames {

const std::string QoSGroup = "QoSGroup"; //one of enum WEB, STD, RT

} /* namespace QoSPolicyNames */

/**
 * @brief The QoSGroup.
 *
 * @ingroup soqosmw/qospolicy
 *
 * @author Timo Haeckel for HAW Hamburg
 */
class QoSGroup: public IQoSPolicy {
public:

    QoSGroup(QoSGroups value) : _value (value){

    }
    virtual ~QoSGroup(){

    }

    const QoSGroups getValue(){
        return _value;
    }

    virtual bool operator==(IQoSPolicy& other){
        QoSGroup& real = static_cast<QoSGroup&> (other);
        return real._value == _value;
    }

private:
    const QoSGroups _value;
};

} /* namespace SOQoSMW */

#endif /* SOQOSMW_QOSPOLICY_MANAGEMENT_QOSGROUP_H_ */
