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

#ifndef WOLKABOUT_MODBUS_REGISTERMAPPING_H
#define WOLKABOUT_MODBUS_REGISTERMAPPING_H

#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace wolkabout
{
class RegisterGroup;

/**
 * @brief Indicates a logical unit, resulting in one value, involving one/multiple/part of Modbus register(s).
 * @details As definition, it takes in the RegisterType and other parameters, in which there is an OutputType,
 *         and parameters to indicate how to read values.
 */
class RegisterMapping
{
public:
    /**
     * @brief Indicates the modbus register type defined by the protocol.
     */
    enum class RegisterType
    {
        COIL = 0,
        INPUT_CONTACT,
        HOLDING_REGISTER,
        INPUT_REGISTER
    };

    /**
     * @brief Indicates the output type for mapping.
     */
    enum class OutputType
    {
        BOOL = 0,
        UINT16,
        INT16,
        UINT32,
        INT32,
        FLOAT,
        STRING
    };

    /**
     * @brief Indicates the operation type for mapping data, when it is read/written to registers.
     */
    enum class OperationType
    {
        NONE = 0,
        MERGE_BIG_ENDIAN,
        MERGE_LITTLE_ENDIAN,
        MERGE_FLOAT,
        STRINGIFY_ASCII,
        STRINGIFY_UNICODE,
        TAKE_BIT
    };

    /**
     * @brief Default constructor for mapping
     * @details For registerTypes COIL/INPUT_CONTACT, the output type is set to BOOL.
     *         For registerTypes HOLDING_REGISTER/INPUT_REGISTER, the output type is set to UINT16.
     * @param reference of the mapping, unique key for access.
     * @param registerType indicates the modbus register type.
     * @param address of the register
     * @param readRestricted indicates whether or not the mapping can be read
     * @param slaveAddress of the devices being accessed, leave as default on -1
     * @param deadbandValue indicates a change in value of the register that is insignificant data
     * @param frequencyFilterValue changes that occur within the given time (in miliseconds) that will be ignored
     */
    RegisterMapping(const std::string& reference, RegisterType registerType, int32_t address,
                    bool readRestricted = false, int16_t slaveAddress = -1, double deadbandValue = 0.0,
                    std::chrono::milliseconds frequencyFilterValue = std::chrono::milliseconds(0));

    /**
     * @brief Default constructor for mapping with custom OutputType.
     * @details For registerTypes COIL/INPUT_CONTACT, the output type is set to BOOL.
     *         For registerTypes HOLDING_REGISTER/INPUT_REGISTER, the output type can be INT16/UINT16.
     * @param reference of the mapping, unique key for access.
     * @param registerType indicates the modbus register type.
     * @param address of the register
     * @param type of the output wanted by the mapping
     * @param readRestricted indicates whether or not the mapping can be read
     * @param slaveAddress of the devices being accessed, leave as default on -1
     * @param deadbandValue indicates a change in value of the register that is insignificant data
     * @param frequencyFilterValue changes that occur within the given time (in miliseconds) that will be ignored
     */
    RegisterMapping(const std::string& reference, RegisterType registerType, int32_t address, OutputType type,
                    bool readRestricted = false, int16_t slaveAddress = -1, double deadbandValue = 0.0,
                    std::chrono::milliseconds frequencyFilterValue = std::chrono::milliseconds(0));

    /**
     * @brief Constructor for cases where bit is taken from a 16 bit register.
     * @details Takes in registerTypes HOLDING_REGISTER and INPUT_REGISTER
     * @param reference of the mapping, unique key for access.
     * @param registerType indicates the modbus register type.
     * @param address of the register
     * @param operation should be set to TAKE_BIT
     * @param bitIndex which bit to take from the value (0-15)
     * @param readRestricted indicates whether or not the mapping can be read
     * @param slaveAddress of the devices being accessed, leave as default on -1
     * @param frequencyFilterValue changes that occur within the given time (in miliseconds) that will be ignored
     */
    RegisterMapping(const std::string& reference, RegisterType registerType, int32_t address, OperationType operation,
                    int8_t bitIndex, bool readRestricted = false, int16_t slaveAddress = -1,
                    std::chrono::milliseconds frequencyFilterValue = std::chrono::milliseconds(0));

    /**
     * @brief Constructor for cases where there is multiple registers merged into a single output value.
     * @details Takes in 2 registers to get 32 bit types (such as UINT32, INT32, FLOAT)
     *         using operations as MERGE_BIG_ENDIAN & MERGE_LITTLE_ENDIAN for UINT32, INT32, and MERGE_FLOAT for FLOAT,
     *         and can take in any number of register for STRING, where string will contain two characters per register
     *         interpreted by operations STRINGIFY_ASCII & STRINGIFY_UNICODE.
     * @param reference of the mapping, unique key for access.
     * @param registerType indicates the modbus register type.
     * @param addresses of the registers
     * @param type of output, way the data will be interpreted for writing/reading
     * @param operation describing what will happen to read data from mappings
     * @param readRestricted indicates whether or not the mapping can be read
     * @param slaveAddress of the devices being accessed, leave as default on -1
     * @param deadbandValue indicates a change in value of the register that is insignificant data
     * @param frequencyFilterValue changes that occur within the given time (in miliseconds) that will be ignored
     */
    RegisterMapping(const std::string& reference, RegisterType registerType, const std::vector<int32_t>& addresses,
                    OutputType type, OperationType operation, bool readRestricted = false, int16_t slaveAddress = -1,
                    double deadbandValue = 0.0,
                    std::chrono::milliseconds frequencyFilterValue = std::chrono::milliseconds(0));

    virtual ~RegisterMapping() = default;

    const std::shared_ptr<RegisterGroup>& getGroup() const;

    void setGroup(const std::shared_ptr<RegisterGroup>& group);

    const std::string& getReference() const;

    bool isReadRestricted() const;

    RegisterType getRegisterType() const;

    int32_t getAddress() const;

    const std::vector<int32_t>& getAddresses() const;

    int16_t getSlaveAddress() const;

    int32_t getStartingAddress() const;

    void setSlaveAddress(int16_t slaveAddress);

    int16_t getRegisterCount() const;

    OutputType getOutputType() const;

    OperationType getOperationType() const;

    int8_t getBitIndex() const;

    /**
     * @brief The method check whether or not the passed value will update the value inside of the mapping.
     * @param newValues The value as bytes that will be parsed into the appropriate type.
     * @return Whether or not the value will update the mapping.
     */
    virtual bool doesUpdate(const std::vector<uint16_t>& newValues) const;

    /**
     * @brief Value watcher method, used for reader to write in new uint16_t values received by the ModbusGroupReader.
     * @details Stores one, two, or however many uint16_t values the mapping requires.
     * @param newValues vector of new uint16_t values.
     * @return whether or not the update operation was successful.
     */
    virtual bool update(const std::vector<uint16_t>& newValues);

    /**
     * @brief The method check whether or not the passed value will update the value inside of the mapping.
     * @param newRegisterValue The value as bool.
     * @return Whether or not the value will update the mapping;
     */
    virtual bool doesUpdate(bool newRegisterValue) const;

    /**
     * @brief Value watcher method, used for reader to write in a new value received by the ModbusGroupReader.
     * @details This is the method used for BoolMapping - ones used by COIL/INPUT_CONTACT or TAKE_BIT mappings.
     * @param newRegisterValue new boolean value
     * @return whether or not the update operation was successful.
     */
    bool update(bool newRegisterValue);

    /**
     * @brief Return uint16_t values for mappings that use such values, otherwise return vector of length 0.
     * @return the uint16_t values received by the ModbusGroupReader, before parsing.
     */
    const std::vector<uint16_t>& getBytesValues() const;

    /**
     * @return the bool value received by the ModbusGroupReader.
     */
    bool getBoolValue() const;

    bool isInitialized() const;

    bool isValid() const;

    void setValid(bool valid);

protected:
    // General mapping data
    std::string m_reference;
    bool m_readRestricted;
    std::shared_ptr<RegisterGroup> m_group;

    // Modbus registers data
    RegisterType m_registerType;
    int32_t m_address = -1;
    std::vector<int32_t> m_addresses;
    int16_t m_slaveAddress = -1;

    // Wolkabout output data
    OutputType m_outputType;
    OperationType m_operationType;
    int8_t m_bitIndex = -1;

    // Value change watching logic
    bool m_boolValue{};
    std::vector<uint16_t> m_byteValues;
    bool m_isInitialized = false;
    bool m_isValid = false;
    double m_deadbandValue = 0.0;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_lastUpdateTime;
    std::chrono::milliseconds m_frequencyFilterValue = std::chrono::milliseconds(0);

private:
    bool deadbandFilter(const std::vector<uint16_t>& newValues) const;
};
}    // namespace wolkabout

#endif    // WOLKABOUT_MODBUS_REGISTERMAPPING_H
