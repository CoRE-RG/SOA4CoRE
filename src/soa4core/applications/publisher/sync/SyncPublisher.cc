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

#include "soa4core/applications/publisher/sync/SyncPublisher.h"
//CoRE4INET
#include "core4inet/base/NotifierConsts.h"
#include "core4inet/utilities/ConfigFunctions.h"
//INET
#include "inet/common/ModuleAccess.h"
//STD
//Auto-generated Messages
#include "core4inet/synchronisation/base/SyncNotification_m.h"

using namespace inet;
using namespace CoRE4INET;
using namespace std;

namespace SOA4CoRE {

Define_Module(SyncPublisher);

double SyncPublisher::getIntervalMin() {
    return getPeriod()->getCycleLength().dbl()*modulo;
}

void SyncPublisher::initialize()
{
    Scheduled::initialize();
    Publisher::initialize();
    inet::findContainingNode(this)->subscribe(NF_SYNC_STATE_CHANGE, this);
}

void SyncPublisher::handleParameterChange(const char* parname)
{
    Scheduled::handleParameterChange(parname);
    Publisher::handleParameterChange(parname);
    if (!parname || !strcmp(parname, "actionTime"))
    {
        this->actionTime = parameterDoubleCheckRange(par("actionTime"), 0, SIMTIME_MAX.dbl(), false, true);
    }
    if (!parname || !strcmp(parname, "modulo"))
    {
        this->modulo = static_cast<unsigned int>(parameterULongCheckRange(par("modulo"), 0, ULONG_MAX));
    }
}

void SyncPublisher::scheduleNextMessage() {
    if(this->event) {
       event->setNext_cycle(true);
    }
    else {
        SchedulerActionTimeEvent *event = new SchedulerActionTimeEvent("API Scheduler Task Event", ACTION_TIME_EVENT);
        event->setAction_time(static_cast<uint32_t>(ceil(this->actionTime / getOscillator()->getPreciseTick())));
        event->setDestinationGate(this->gate("schedulerIn"));
        if (event->getAction_time() >= getPeriod()->getCycleTicks())
        {
            throw cRuntimeError("The actionTime (%d ticks) starts outside of the period (%d ticks)",
                    event->getAction_time(), getPeriod()->getCycleTicks());
        }
    }
    getPeriod()->registerEvent(event);
}

void SyncPublisher::handleMessage(cMessage *msg)
{
    if (msg && msg->arrivedOn("schedulerIn"))
    {
        uint32_t currentCycle = getPeriod()->getCycles();
        if(this->synchronized && this->nextCycle <= currentCycle) {
            sendMessage();
            this->nextCycle = currentCycle + this->modulo;
        }
        scheduleNextMessage();
    }
    else
    {
        Publisher::handleMessage(msg);
    }
}

void SyncPublisher::receiveSignal(__attribute__((unused)) cComponent *src, __attribute__((unused)) simsignal_t id, cObject *obj, __attribute__((unused)) cObject *details)
{
    Enter_Method_Silent();
    if (SyncNotification *notification = dynamic_cast<SyncNotification *>(obj))
    {
        if (notification->getKind() == SYNC)
        {
            synchronized = true;
        }
        else
        {
            synchronized = false;
        }
    }
}

} /* end namespace SOA4CoRE */
