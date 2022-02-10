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

#ifndef SOA4CORE_MESSAGES_SOMEIP_SOMEIPSDHEADER_H_
#define SOA4CORE_MESSAGES_SOMEIP_SOMEIPSDHEADER_H_

#define UNCOVEREDBYTESBYLENGTH 8

//Std
#include <list>
//AUTO-GENERATED MESSAGES
#include "soa4core/messages/someip/SomeIpSDHeader_m.h"
#include "soa4core/messages/someip/SomeIpSDEntry_m.h"
#include "soa4core/messages/someip/SomeIpSDOption_m.h"

namespace SOA4CoRE {

/**
 * @brief Message for encapsulation of multiple SomeIpSDEntry and SomeIpSDOption messages
 *
 * @ingroup soa4core/messages
 *
 * @author Mehmet Mueller for HAW Hamburg (adapted from Philipp Meyer GatewayAggregationMessage in SignalsAndGateways)
 */
class SomeIpSDHeader : public SomeIpSDHeader_Base {

    /**
     * Methods
     */
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
protected:
private:
    /**
     * @brief Updates the header length of the SOME/IP Header
     */
    void updateHeaderLength();

    void copy(const SomeIpSDHeader& other);

    /**
     * Member variables
     */
public:
protected:
private:
    std::list<SomeIpSDEntry*> entryList;
    std::list<SomeIpSDOption*> optionList;
};
} /* end namespace SOA4CoRE */
#endif /* SOA4CORE_MESSAGES_SOMEIP_SOMEIPSDHEADER_H_ */
