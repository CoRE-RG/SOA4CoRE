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

#ifndef SOQOSMW_ENDPOINTS_PUBLISHER_BASE_IPUBLISHER_H_
#define SOQOSMW_ENDPOINTS_PUBLISHER_BASE_IPUBLISHER_H_

#include <endpoints/base/IEndpoint.h>
#include <string>

#include <messages/QoSNegotiationProtocol/QoSNegotiationProtocol_m.h>

namespace soqosmw {
class PublisherWriter;
} /* namespace soqosmw */

namespace omnetpp {
class cPacket;
} /* namespace omnetpp */

namespace soqosmw {

/**
 * @brief Publisher interface. Base class for all publishing modules. Provides the behavior the application knows about.
 *
 * @ingroup soqosmw/endpoints
 *
 * @author Timo Haeckel
 */
class IPublisher: public IEndpoint {
public:
    /**
     * Constructor.
     * @param path from IEndpoint.
     * @param qosPolicies from IEndpoint.
     * @param executingApplication from IEndpoint.
     */
    IPublisher(std::string path,  PublisherWriter* writer);
    virtual ~IPublisher();

    /**
     * Publish a payload to all subscribers. This needs to be implemented by all subclasses.
     * @param payload
     */
    virtual void publish(omnetpp::cPacket* payload) = 0;
    /**
     * get the connection specific information for this publisher
     */
    virtual ConnectionSpecificInformation* getConnectionSpecificInformation() = 0;
    /**
     * Get the qos class for this publisher
     */
    virtual QoSGroups getQoSClass() = 0;

    PublisherWriter* getWriter() {
        return _writer;
    }

    void setWriter( PublisherWriter* writer) {
        _writer = writer;
    }

private:
    PublisherWriter* _writer;
};

} /*end namespace soqosmw*/

#endif /* SOQOSMW_ENDPOINTS_PUBLISHER_BASE_IPUBLISHER_H_ */
