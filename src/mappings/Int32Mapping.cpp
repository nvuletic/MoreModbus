//
// Created by astrihale on 24.3.20..
//

#include "Int32Mapping.h"
#include "utility/DataParsers.h"

#include <stdexcept>

namespace wolkabout
{
Int32Mapping::Int32Mapping(const std::string& reference, RegisterMapping::RegisterType registerType,
                           const std::vector<int16_t>& addresses, RegisterMapping::OperationType operation,
                           bool readRestricted, int8_t slaveAddress)
: RegisterMapping(reference, registerType, addresses, OutputType::INT32, operation, readRestricted, slaveAddress)
{
    if (operation != OperationType::MERGE_BIG_ENDIAN && operation != OperationType::MERGE_LITTLE_ENDIAN)
    {
        throw std::logic_error("Int32Mapping: Illegal operation type set.");
    }
}

bool Int32Mapping::update(const std::vector<uint16_t>& newValues)
{
    switch (m_operationType)
    {
    case OperationType::MERGE_BIG_ENDIAN:
        m_int32Value = DataParsers::registersToInt32(newValues, DataParsers::Endian::BIG);
        break;
    case OperationType::MERGE_LITTLE_ENDIAN:
        m_int32Value = DataParsers::registersToInt32(newValues, DataParsers::Endian::LITTLE);
        break;
    default:
        throw std::logic_error("Int32Mapping: Illegal operation type set.");
    }

    return RegisterMapping::update(newValues);
}

bool Int32Mapping::writeValue(int32_t value)
{
    std::vector<uint16_t> bytes;
    if (m_operationType == OperationType::MERGE_BIG_ENDIAN)
        bytes = DataParsers::int32ToRegisters(value, DataParsers::Endian::BIG);
    else if (m_operationType == OperationType::MERGE_LITTLE_ENDIAN)
        bytes = DataParsers::int32ToRegisters(value, DataParsers::Endian::LITTLE);
    else
        throw std::logic_error("Int32Mapping: Illegal operation type set.");

    bool success = ModbusReader::getInstance()->writeMapping(*this, bytes);
    if (success)
        m_int32Value = value;

    return success;
}

int32_t Int32Mapping::getInt32Value() const
{
    return m_int32Value;
}
}    // namespace wolkabout