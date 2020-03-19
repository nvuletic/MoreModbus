//
// Created by astrihale on 19.3.20..
//

#ifndef WOLKABOUT_MODBUS_DATAPARSERS_H
#define WOLKABOUT_MODBUS_DATAPARSERS_H

#include <cstdint>
#include <string>
#include <vector>

namespace wolkabout
{
class DataParsers
{
public:
    enum class Endian
    {
        BIG = 0,
        SMALL
    };

    static std::vector<uint16_t> asciiStringToRegisters(const std::string& value);
    static std::vector<uint16_t> unicodeStringToRegisters(const std::string& value);
    static std::vector<uint16_t> utf16StringToRegisters(const std::string& value);

    static std::vector<uint16_t> int32ToRegisters(int32_t value, Endian endian);
    static std::vector<uint16_t> uint32ToRegisters(uint32_t value, Endian endian);
    static std::vector<uint16_t> floatToRegisters(float value);

    static std::string registersToAsciiString(const std::vector<uint16_t>& value);
    static std::string registersToUnicodeString(const std::vector<uint16_t>& value);
    static std::string registersToUtf16String(const std::vector<uint16_t>& value);

    static int32_t registersToInt32(const std::vector<uint16_t>& value, Endian endian);
    static uint32_t registersToUint32(const std::vector<uint16_t>& value, Endian endian);
    static float registersToFloat(const std::vector<uint16_t>& value);
};
}    // namespace wolkabout

#endif    // WOLKABOUT_MODBUS_DATAPARSERS_H
