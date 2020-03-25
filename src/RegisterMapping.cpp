/*
 * Copyright 2020 WolkAbout Technology s.r.o.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "RegisterMapping.h"

#include <algorithm>
#include <stdexcept>

namespace wolkabout
{
RegisterMapping::RegisterMapping(const std::string& reference, RegisterMapping::RegisterType registerType,
                                 int16_t address, bool readRestricted, int8_t slaveAddress)
: m_reference(reference)
, m_readRestricted(readRestricted)
, m_registerType(registerType)
, m_address(address)
, m_slaveAddress(slaveAddress)
, m_operationType(OperationType::NONE)
, m_byteValues(1)
{
    // On default, assign default OutputType for type of register.
    switch (m_registerType)
    {
    case RegisterType::INPUT_REGISTER:
    case RegisterType::HOLDING_REGISTER:
        m_outputType = OutputType::INT16;
        break;
    case RegisterType::COIL:
    case RegisterType::INPUT_CONTACT:
        m_outputType = OutputType::BOOL;
        break;
    }
}

RegisterMapping::RegisterMapping(const std::string& reference, RegisterMapping::RegisterType registerType,
                                 int16_t address, OutputType type, bool readRestricted, int8_t slaveAddress)
: m_reference(reference)
, m_readRestricted(readRestricted)
, m_registerType(registerType)
, m_address(address)
, m_slaveAddress(slaveAddress)
, m_outputType(type)
, m_operationType(OperationType::NONE)
, m_byteValues(1)
{
    switch (m_registerType)
    {
    case RegisterType::INPUT_REGISTER:
    case RegisterType::HOLDING_REGISTER:
        // Allow Registers to be INT16, UINT16.
        if (m_outputType != OutputType::INT16 && m_outputType != OutputType::UINT16)
        {
            throw std::logic_error("RegisterMapping: Single address register mapping can\'t"
                                   " be anything else than INT16, UINT16.");
        }
        break;
    case RegisterType::COIL:
    case RegisterType::INPUT_CONTACT:
        if (m_outputType != OutputType::BOOL)
        {
            throw std::logic_error("RegisterMapping: Single address discrete register can\'t"
                                   " be anything else than BOOL.");
        }
        break;
    }
}

RegisterMapping::RegisterMapping(const std::string& reference, RegisterMapping::RegisterType registerType,
                                 int16_t address, OperationType type, int8_t bitIndex, bool readRestricted,
                                 int8_t slaveAddress)
: m_reference(reference)
, m_readRestricted(readRestricted)
, m_registerType(registerType)
, m_address(address)
, m_slaveAddress(slaveAddress)
, m_outputType(OutputType::BOOL)
, m_operationType(OperationType::TAKE_BIT)
, m_bitIndex(bitIndex)
{
    if (m_registerType == RegisterType::COIL || m_registerType == RegisterType::INPUT_CONTACT)
    {
        throw std::logic_error("RegisterMapping: Take bit can\'t be done over COIL/INPUT_CONTACT.");
    }
}

RegisterMapping::RegisterMapping(const std::string& reference, RegisterMapping::RegisterType registerType,
                                 const std::vector<int16_t>& addresses, OutputType type, OperationType operation,
                                 bool readRestricted, int8_t slaveAddress)
: m_reference(reference)
, m_readRestricted(readRestricted)
, m_registerType(registerType)
, m_addresses(addresses)
, m_slaveAddress(slaveAddress)
, m_outputType(type)
, m_operationType(operation)
{
    if (m_registerType == RegisterType::COIL || m_registerType == RegisterType::INPUT_CONTACT)
    {
        throw std::logic_error("RegisterMapping: Multi register mapping can\'t be COIL or INPUT_CONTACT.");
    }

    if (m_outputType == OutputType::BOOL || m_outputType == OutputType::INT16 || m_outputType == OutputType::UINT16)
    {
        throw std::logic_error("RegisterMapping: Multi register mapping can\'t be BOOL, INT16 or UINT16.");
    }

    // Can be two registers that are being merged into a 32bit, or multiple ones merging into a string.
    if (m_operationType == OperationType::MERGE_BIG_ENDIAN || m_operationType == OperationType::MERGE_LITTLE_ENDIAN)
    {
        if (m_addresses.size() != 2)
        {
            throw std::logic_error("RegisterMapping: Merge operations work only with 2 registers.");
        }

        if (m_outputType != OutputType::INT32 && m_outputType != OutputType::UINT32)
        {
            throw std::logic_error("RegisterMapping: Merge operations (with endians)"
                                   " output 32bit types (INT32, UINT32).");
        }
    }
    else if (m_operationType == OperationType::MERGE_FLOAT)
    {
        if (m_addresses.size() != 2)
        {
            throw std::logic_error("RegisterMapping: Merge operations work only with 2 registers.");
        }

        if (m_outputType != OutputType::FLOAT)
        {
            throw std::logic_error("RegisterMapping: Merge for floats can only output FLOAT.");
        }
    }
    else if (m_operationType == OperationType::STRINGIFY_ASCII || m_operationType == OperationType::STRINGIFY_UNICODE)
    {
        if (m_outputType != OutputType::STRING)
        {
            throw std::logic_error("RegisterMapping: Stringify can only return string.");
        }
    }

    m_byteValues = std::vector<uint16_t>(m_addresses.size());
}

const std::string& RegisterMapping::getReference() const
{
    return m_reference;
}

bool RegisterMapping::isReadRestricted() const
{
    return m_readRestricted;
}

RegisterMapping::RegisterType RegisterMapping::getRegisterType() const
{
    return m_registerType;
}

int16_t RegisterMapping::getAddress() const
{
    return m_address;
}

const std::vector<int16_t>& RegisterMapping::getAddresses() const
{
    return m_addresses;
}

int8_t RegisterMapping::getSlaveAddress() const
{
    return m_slaveAddress;
}

void RegisterMapping::setSlaveAddress(int8_t slaveAddress)
{
    m_slaveAddress = slaveAddress;
}

int16_t RegisterMapping::getStartingAddress() const
{
    if (m_address == -1)
    {
        return m_addresses.at(0);
    }
    return m_address;
}

int16_t RegisterMapping::getRegisterCount() const
{
    if (m_address == -1)
    {
        return static_cast<int16_t>(m_addresses.size());
    }
    return 1;
}

RegisterMapping::OutputType RegisterMapping::getOutputType() const
{
    return m_outputType;
}

RegisterMapping::OperationType RegisterMapping::getOperationType() const
{
    return m_operationType;
}

int8_t RegisterMapping::getBitIndex() const
{
    return m_bitIndex;
}

bool RegisterMapping::update(const std::vector<uint16_t>& newValues)
{
    if (newValues.size() != m_byteValues.size())
    {
        throw std::logic_error("RegisterMapping: The value array has to be the same size, it cannot change.");
    }

    bool different = false;
    uint i = 0;
    while (!different && i < newValues.size())
    {
        if (m_byteValues[i] != newValues[i])
        {
            different = true;
        }
        ++i;
    }
    m_byteValues = newValues;

    bool isValueInitialized = m_isInitialized;
    m_isInitialized = true;

    bool isValid = m_isValid;
    m_isValid = true;

    return !isValueInitialized || different || !isValid;
}

bool RegisterMapping::update(bool newRegisterValue)
{
    bool different = m_boolValue != newRegisterValue;
    m_boolValue = newRegisterValue;

    bool isValueInitialized = m_isInitialized;
    m_isInitialized = true;

    bool isValid = m_isValid;
    m_isValid = true;

    return !isValueInitialized || different || !isValid;
}

const std::vector<uint16_t>& RegisterMapping::getBytesValues() const
{
    return m_byteValues;
}

bool RegisterMapping::getBoolValue() const
{
    return m_boolValue;
}

void RegisterMapping::setValid(bool valid)
{
    m_isValid = valid;
}
}    // namespace wolkabout
