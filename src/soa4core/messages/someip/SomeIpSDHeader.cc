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

#include "soa4core/messages/someip/SomeIpSDHeader.h"

using namespace std;

namespace SOA4CoRE {

Register_Class(SomeIpSDHeader);

std::ostream& operator<<(std::ostream& os, const IPv4EndpointOption& endpoint)
{
    os << "IPv4EndpointOption {";
    os << " IP=" << endpoint.getIpv4Address().str();
    os << ", port=" << endpoint.getPort();
    os << ", protocol=" << (uint32) endpoint.getL4Protocol();
    os << " } ";
    return os;
}

std::ostream& operator<<(std::ostream& os, const IEEE8021QConfigurationOption& config)
{
    os << "IEEE8021QConfigurationOption {";
    os << " pcp=" << config.getPcp();
    os << ", vlan_id=" << config.getVlan_id();
    os << " } ";
    return os;
}

std::ostream& operator<<(std::ostream& os, const RessourceConfigurationOption& config)
{
    os << "RessourceConfigurationOption {";
    os << " maxFrameSize=" << config.getMaxPayload();
    os << ", minInterval=" << config.getMinInterval();
    os << "s } ";
    return os;
}

std::ostream& operator<<(std::ostream& os, const RealTimeConfigurationOption& config)
{
    os << "RealTimeConfigurationOption {";
    os << " deadline=" << config.getDeadline();
    os << "s } ";
    return os;
}

void SomeIpSDHeader::copy(const SomeIpSDHeader& other){
    for(list<SomeIpSDEntry*>::const_iterator it = other.entryList.begin(); it != other.entryList.end(); ++it){
        SomeIpSDEntry* entry = (*it)->dup();
        take(entry);
        this->entryList.push_back(entry);
    }
    for(list<SomeIpSDOption*>::const_iterator it = other.optionList.begin(); it != other.optionList.end(); ++it){
        SomeIpSDOption* option = (*it)->dup();
        take(option);
        this->optionList.push_back(option);
    }
}

void SomeIpSDHeader::encapEntry(SomeIpSDEntry* entry){
    take(entry);
    setByteLength(getByteLength() + entry->getByteLength());
    setEntriesLength(getEntriesLength() + entry->getByteLength());
    updateHeaderLength();
    entryList.push_back(entry);
}

SomeIpSDEntry* SomeIpSDHeader::decapEntry(){
    SomeIpSDEntry* entry = nullptr;
    if(entryList.size() > 0){
        entry = entryList.front();
        setByteLength(getByteLength() - entry->getByteLength());
        setEntriesLength(getEntriesLength() - entry->getByteLength());
        updateHeaderLength();
        entryList.pop_front();
        drop(entry);
    }
    return entry;
}

list<SomeIpSDEntry*> SomeIpSDHeader::getEncapEntries() const{
    return entryList;
}

size_t SomeIpSDHeader::getEntryCnt(){
    return entryList.size();
}

void SomeIpSDHeader::encapOption(SomeIpSDOption* option){
    take(option);
    setByteLength(getByteLength() + option->getByteLength());
    setOptionsLength(getOptionsLength() + option->getByteLength());
    updateHeaderLength();
    optionList.push_back(option);
}

SomeIpSDOption* SomeIpSDHeader::decapOption(){
    SomeIpSDOption* option = nullptr;
    if(optionList.size() > 0){
        option = optionList.front();
        setByteLength(getByteLength() - option->getByteLength());
        setOptionsLength(getOptionsLength() - option->getByteLength());
        updateHeaderLength();
        optionList.pop_front();
        drop(option);
    }
    return option;
}

list<SomeIpSDOption*> SomeIpSDHeader::getEncapOptions() const{
    return optionList;
}

size_t SomeIpSDHeader::getOptionCnt(){
    return optionList.size();
}

void SomeIpSDHeader::updateHeaderLength() {
    setLength(getByteLength() - UNCOVEREDBYTESBYLENGTH);
}

} /* end namespace SOA4CoRE */

