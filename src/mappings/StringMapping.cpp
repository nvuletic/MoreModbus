/*
 * Copyright (C) 2020 WolkAbout Technology s.r.o.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "StringMapping.h"

#include "utilities/DataParsers.h"

#include <stdexcept>

namespace wolkabout
{
StringMapping::StringMapping(const std::string& reference, RegisterMapping::RegisterType registerType,
                             const std::vector<int32_t>& addresses, RegisterMapping::OperationType operation,
                             bool readRestricted, int16_t slaveAddress, std::chrono::milliseconds frequencyFilterValue)
: RegisterMapping(reference, registerType, addresses, OutputType::STRING, operation, readRestricted, slaveAddress, 0.0,
                  frequencyFilterValue)
{
    if (operation != OperationType::STRINGIFY_ASCII && operation != OperationType::STRINGIFY_UNICODE)
    {
        throw std::logic_error("StringMapping: Illegal operation type set.");
    }
}

bool wolkabout::StringMapping::update(const std::vector<uint16_t>& newValues)
{
    switch (m_operationType)
    {
    case OperationType::STRINGIFY_ASCII:
        m_stringValue = DataParsers::registersToAsciiString(newValues);
        break;
    case OperationType ::STRINGIFY_UNICODE:
        m_stringValue = DataParsers::registersToUnicodeString(newValues);
        break;
    default:
        throw std::logic_error("StringMapping: Illegal operation type set.");
    }

    return RegisterMapping::update(newValues);
}

bool StringMapping::writeValue(const std::string& newValue)
{
    if (newValue.size() > static_cast<uint16_t>(getRegisterCount() * 2))
    {
        throw std::logic_error("StringMapping: You can\'t write a string that\'s longer than " +
                               std::to_string(getRegisterCount() * 2) + " characters!");
    }

    std::vector<uint16_t> bytes;
    if (m_operationType == OperationType::STRINGIFY_ASCII)
    {
        bytes = DataParsers::asciiStringToRegisters(newValue);
        while (bytes.size() < this->m_addresses.size())
            bytes.emplace_back(0);
    }
    else if (m_operationType == OperationType::STRINGIFY_UNICODE)
    {
        bytes = DataParsers::unicodeStringToRegisters(newValue);
        while (bytes.size() < this->m_addresses.size())
            bytes.emplace_back(0);
    }
    else
        throw std::logic_error("StringMapping: Illegal operation type set.");

    while (bytes.size() < this->m_addresses.size())
        bytes.emplace_back(0);

    if (getGroup()->getDevice()->getReader().expired())
        return false;

    const auto reader = getGroup()->getDevice()->getReader().lock();
    bool success = reader->writeMapping(*this, bytes);
    if (success)
        m_stringValue = newValue;

    return success;
}

const std::string& StringMapping::getStringValue() const
{
    return m_stringValue;
}
}    // namespace wolkabout
