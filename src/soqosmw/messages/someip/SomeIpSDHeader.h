/*
 * SomeIpSDHeader.h
 *
 *  Created on: Apr 30, 2020
 *      Author: mehkir
 */

#ifndef SOQOSMW_MESSAGES_SOMEIP_SOMEIPSDHEADER_H_
#define SOQOSMW_MESSAGES_SOMEIP_SOMEIPSDHEADER_H_

#define UNCOVEREDBYTESBYLENGTH 8

//Std
#include <list>
//SignalsAndGateways auto-generated messages
#include "soqosmw/messages/someip/SomeIpSDHeader_m.h"
#include "soqosmw/messages/someip/Entry_m.h"
#include "soqosmw/messages/someip/Option_m.h"

namespace SOQoSMW {

/**
 * @brief Message for encapsulation of multiple Entry and Option messages
 *
 * @author Mehmet Cakir (adapted from Philipp Meyer GatewayAggregationMessage in SignalsAndGateways)
 */
class SomeIpSDHeader : public SomeIpSDHeader_Base {
private:
    std::list<Entry*> entryList;
    std::list<Option*> optionList;

    void copy(const SomeIpSDHeader& other);

public:
    /**
     * @brief Constructor
     *
     * @param newName message name
     * @param kind message type
     */
    SomeIpSDHeader(const char *newName=nullptr, short kind=0) : SomeIpSDHeader_Base(newName,kind)
    {
    }

    /**
     * @brief Copy constructor
     */
    SomeIpSDHeader(const SomeIpSDHeader& other) : SomeIpSDHeader_Base(other)
    {
        copy(other);
    }

    /**
     * @brief Assignment operator
     */
    SomeIpSDHeader& operator=(const SomeIpSDHeader& other)
    {
        if (this==&other) return *this;
        SomeIpSDHeader_Base::operator=(other);
        copy(other);
        return *this;
    }

    /**
     * @brief SomeIpSDHeader duplication
     */
    virtual SomeIpSDHeader *dup() const
    {
        return new SomeIpSDHeader(*this);
    }

    /**
     * @brief encapsulates an Entry
     *
     * @param entry Entry pointer
     */
    virtual void encapEntry(Entry* entry);

    /**
     * @brief decapsulates an Entry
     *
     * @return Entry pointer
     */
    virtual Entry* decapEntry();

    /**
     * @brief get encapsulated Entries
     *
     * @return list of Entry pointers
     */
    virtual std::list<Entry*> getEncapEntries() const;

    /**
     * @brief Get number of encapsulated Entries
     *
     * @return number of encapsulated Entries
     */
    virtual size_t getEntryCnt();

    /**
     * @brief encapsulates an Option
     *
     * @param option Option pointer
     */
    virtual void encapOption(Option* option);

    /**
     * @brief decapsulates an Option
     *
     * @return Option pointer
     */
    virtual Option* decapOption();

    /**
     * @brief get encapsulated Options
     *
     * @return list of Option pointers
     */
    virtual std::list<Option*> getEncapOptions() const;

    /**
     * @brief Get number of encapsulated Options
     *
     * @return number of encapsulated Options
     */
    virtual size_t getOptionCnt();

private:
    /**
     * @brief Updates the header length of the SOME/IP Header
     */
    void updateHeaderLength();
};
} /* end namespace SOQoSMW */
#endif /* SOQOSMW_MESSAGES_SOMEIP_SOMEIPSDHEADER_H_ */
