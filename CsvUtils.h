#pragma once

#include <string>
#include <cstdlib>   // std::strtol, std::strtod
#include <cerrno>
#include <climits>
#include <cstring>
#include "CsvReader.h"
#include "Door.h"

inline void CopyCsvText(const CsvRow& row, const char* columnName, char* dst, size_t dstSize)
{
    const std::string& value = row[columnName];
    strncpy_s(dst, dstSize, value.c_str(), _TRUNCATE);
}

inline bool ReadInt(const CsvRow& row, const char* columnName, unsigned int& outValue)
{
    const std::string& s = row[columnName];

    if (s.empty())
        return false;

    char* end = nullptr;
    errno = 0;

    long v = std::strtol(s.c_str(), &end, 10);

    if (errno != 0 || end == s.c_str() || *end != '\0')
        return false;

    if (v < INT_MIN || v > INT_MAX)
        return false;

    outValue = static_cast<int>(v);
    return true;
}

inline bool ReadDouble(
    const CsvRow& row,
    const char* columnName,
    double& outValue)
{
    const std::string& s = row[columnName];

    if (s.empty())
        return false;

    char* end = nullptr;
    errno = 0;

    double v = std::strtod(s.c_str(), &end);

    if (errno != 0 || end == s.c_str() || *end != '\0')
        return false;

    outValue = v;
    return true;
}

struct EnumMap
{
    const char* name;
    int value;
};


inline bool ReadFaceType(const CsvRow& row, FaceType& out)
{
    const std::string& s = row["Type"];

    if (s == "Door") { out = FaceType::Door; return true; }
    if (s == "Drawer") { out = FaceType::Drawer;   return true; }
    if (s == "Panel") { out = FaceType::Panel; return true; }

    return false;
}

inline bool ReadConstruction(const CsvRow& row, Construction& out)
{
    const std::string& s = row["Construction"];

    if (s == "SLAB") { out = Construction::Slab; return true; }
    if (s == "SHAKER") { out = Construction::Shaker;   return true; }
    if (s == "SMALL_SHAKER") { out = Construction::SmallShaker; return true; }

    return false;
}

inline bool ReadOrientation(const CsvRow& row, Orientation& out)
{
    const std::string& s = row["Grain Direction"];

    if (s == "VERTICAL") { out = Orientation::VERTICAL; return true; }
    if (s == "HORIZONTAL") { out = Orientation::HORIZONTAL;   return true; }

    return false;
}

inline bool ReadPanel(const CsvRow& row, bool& out)
{
    const std::string& s = row["Panel"];

    if (s == "Yes") { out = true; return true; }
    else out = false;

    return false;
}
