/*
 * SomeIpSDHeader.cc
 *
 *  Created on: Apr 30, 2020
 *      Author: mehkir
 */

#include "soa4core/messages/someip/SomeIpSDHeader.h"

using namespace std;

namespace SOA4CoRE {

Register_Class(SomeIpSDHeader);

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

