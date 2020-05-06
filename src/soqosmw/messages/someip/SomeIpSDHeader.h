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
#include "soqosmw/messages/someip/SomeIpSDEntry_m.h"
#include "soqosmw/messages/someip/SomeIpSDOption_m.h"

namespace SOQoSMW {

/**
 * @brief Message for encapsulation of multiple SomeIpSDEntry and SomeIpSDOption messages
 *
 * @author Mehmet Cakir (adapted from Philipp Meyer GatewayAggregationMessage in SignalsAndGateways)
 */
class SomeIpSDHeader : public SomeIpSDHeader_Base {
private:
    std::list<SomeIpSDEntry*> entryList;
    std::list<SomeIpSDOption*> optionList;

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
     * @brief encapsulates an SomeIpSDEntry
     *
     * @param entry SomeIpSDEntry pointer
     */
    virtual void encapEntry(SomeIpSDEntry* entry);

    /**
     * @brief decapsulates an SomeIpSDEntry
     *
     * @return SomeIpSDEntry pointer
     */
    virtual SomeIpSDEntry* decapEntry();

    /**
     * @brief get encapsulated Entries
     *
     * @return list of SomeIpSDEntry pointers
     */
    virtual std::list<SomeIpSDEntry*> getEncapEntries() const;

    /**
     * @brief Get number of encapsulated Entries
     *
     * @return number of encapsulated Entries
     */
    virtual size_t getEntryCnt();

    /**
     * @brief encapsulates an SomeIpSDOption
     *
     * @param option SomeIpSDOption pointer
     */
    virtual void encapOption(SomeIpSDOption* option);

    /**
     * @brief decapsulates an SomeIpSDOption
     *
     * @return SomeIpSDOption pointer
     */
    virtual SomeIpSDOption* decapOption();

    /**
     * @brief get encapsulated Options
     *
     * @return list of SomeIpSDOption pointers
     */
    virtual std::list<SomeIpSDOption*> getEncapOptions() const;

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
