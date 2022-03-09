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

#include "ConnectorBase.h"

//STD
#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>

namespace SOA4CoRE {

Define_Module(ConnectorBase);

void ConnectorBase::initialize()
{
    handleParameterChange(nullptr);
    this->_messageDroppedSignal = registerSignal("messageDropped");
}

void ConnectorBase::handleParameterChange(const char* parname) {
    if (!parname || !strcmp(parname, "createConnectorMappingEnabled")) {
        _createConnectorMappingEnabled = par("createConnectorMappingEnabled");
    }
}

} /*end namespace SOA4CoRE*/
