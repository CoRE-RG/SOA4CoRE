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

#ifndef __SOQOSMW_SOMEIPLOCALSERVICEREGISTRY_H_
#define __SOQOSMW_SOMEIPLOCALSERVICEREGISTRY_H_

#include <omnetpp.h>
#include <map>
#include <list>
#include "soqosmw/applications/someipapp/someipSDTestApp/publisherapp/SomeipPublisher.h"
#include "soqosmw/applications/someipapp/someipSDTestApp/subscriberapp/SomeipSubscriber.h"

using namespace omnetpp;
namespace SOQoSMW {

/**
 * @brief Basic SomeipLocalServiceRegistry
 *
 * @ingroup soqosmw/applications
 *
 * @author Mehmet Cakir
 */
class SomeipLocalServiceRegistry : public cSimpleModule
{
  /**
   * Methods
   */
  public:
    void registerPublisherService(SomeipPublisher *someipPublisher);
    void registerSubscriberService(SomeipSubscriber *someipSubscriber);
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
  private:

  /**
   * Member variables
   */
  public:
  protected:
  private:
    std::map<int,std::list<SomeipPublisher*>> _serviceIDToPublisher;
    std::map<int,std::list<SomeipSubscriber*>> _serviceIDToSubscriber;

};
}
#endif
