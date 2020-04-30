/*
 * SomeIpSDHeader.cpp
 *
 *  Created on: Apr 30, 2020
 *      Author: mehkir
 */

#include <soqosmw/messages/someip/SomeIpSDHeader.h>

using namespace std;

namespace SOQoSMW {

Register_Class(SomeIpSDHeader);

void SomeIpSDHeader::copy(const SomeIpSDHeader& other){
    for(list<Entry*>::const_iterator it = other.entryList.begin(); it != other.entryList.end(); ++it){
        Entry* entry = (*it)->dup();
        take(entry);
        this->entryList.push_back(entry);
    }
    for(list<Option*>::const_iterator it = other.optionList.begin(); it != other.optionList.end(); ++it){
        Option* option = (*it)->dup();
        take(option);
        this->optionList.push_back(option);
    }
}

void SomeIpSDHeader::encapEntry(Entry* entry){
    take(entry);
    setByteLength(getByteLength() + entry->getByteLength());
    entryList.push_back(entry);
}

Entry* SomeIpSDHeader::decapEntry(){
    Entry* entry = nullptr;
    if(entryList.size() > 0){
        entry = entryList.front();
        setByteLength(getByteLength() - entry->getByteLength());
        entryList.pop_front();
        drop(entry);
    }
    return entry;
}

list<Entry*> SomeIpSDHeader::getEncapEntries() const{
    return entryList;
}

size_t SomeIpSDHeader::getEntryCnt(){
    return entryList.size();
}

void SomeIpSDHeader::encapOption(Option* option){
    take(option);
    setByteLength(getByteLength() + option->getByteLength());
    optionList.push_back(option);
}

Option* SomeIpSDHeader::decapOption(){
    Option* option = nullptr;
    if(optionList.size() > 0){
        option = optionList.front();
        setByteLength(getByteLength() - option->getByteLength());
        optionList.pop_front();
        drop(option);
    }
    return option;
}

list<Option*> SomeIpSDHeader::getEncapOptions() const{
    return optionList;
}

size_t SomeIpSDHeader::getOptionCnt(){
    return optionList.size();
}

} /* end namespace SOQoSMW */

