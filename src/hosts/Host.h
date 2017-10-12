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

#ifndef __HAUPTPROJEKT_TIMO_HAECKEL_TCX_H
#define __HAUPTPROJEKT_TIMO_HAECKEL_TCX_H

#include <omnetpp.h>

using namespace omnetpp;

namespace hauptprojekt_timo_haeckel {

/**
 * Implements the Txc simple module. See the NED file for more information.
 */
class Host : public cSimpleModule
{
  protected:
    virtual void initialize() = 0;
    virtual void handleMessage(cMessage *msg) = 0;
};

}; // namespace

#endif
