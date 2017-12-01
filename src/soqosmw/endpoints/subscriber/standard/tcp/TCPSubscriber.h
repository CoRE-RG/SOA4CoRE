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

#ifndef SOQOSMW_ENDPOINTS_SUBSCRIBER_STANDARD_TCP_TCPSUBSCRIBER_H_
#define SOQOSMW_ENDPOINTS_SUBSCRIBER_STANDARD_TCP_TCPSUBSCRIBER_H_

#include <endpoints/subscriber/standard/base/IStdSubscriber.h>

namespace soqosmw {

class TCPSubscriber: public IStdSubscriber {
public:
    TCPSubscriber(std::string path);
    virtual ~TCPSubscriber();
};

} /*end namespace soqosmw*/

#endif /* SOQOSMW_ENDPOINTS_SUBSCRIBER_STANDARD_TCP_TCPSUBSCRIBER_H_ */
