#pragma once

#include <string>
#include <cstdlib>   // std::strtol, std::strtod
#include <cerrno>
#include <climits>
#include <cstring>
#include "CsvReader.h"
#include "Door.h"
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <cctype>

//struct CsvError
//{
//    size_t row_index;     // 1-based, CSV row number
//    std::string message;
//};

inline std::string Trim(std::string s)
{
    auto not_space = [](unsigned char c) { return !std::isspace(c); };

    s.erase(s.begin(), std::find_if(s.begin(), s.end(), not_space));
    s.erase(std::find_if(s.rbegin(), s.rend(), not_space).base(), s.end());

    return s;
}

inline std::string ToUpper(std::string s)
{
    s = Trim(s);
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return std::toupper(c); });
    return s;
}

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

inline bool ReadDouble(const CsvRow& row, const char* columnName, double& outValue)
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
    const std::string& s = ToUpper(row["Type"]);

    if (s == "DOOR") { out = FaceType::Door; return true; }
    if (s == "DRAWER") { out = FaceType::Drawer;   return true; }
    if (s == "PANEL") { out = FaceType::Panel; return true; }

    return false;
}

inline bool ReadConstruction(const CsvRow& row, Construction& out)
{
    const std::string& s = ToUpper(row["Construction"]);

    if (s == "SLAB") { out = Construction::Slab; return true; }
    if (s == "SHAKER") { out = Construction::Shaker;   return true; }
    if (s == "SMALL_SHAKER") { out = Construction::SmallShaker; return true; }

    return false;
}

inline bool ReadOrientation(const CsvRow& row, Orientation& out)
{
    const std::string& s = ToUpper(row["Grain Direction"]);

    if (s == "VERTICAL") { out = Orientation::VERTICAL; return true; }
    if (s == "HORIZONTAL") { out = Orientation::HORIZONTAL;   return true; }

    return false;
}

inline bool ReadPanel(const CsvRow& row, bool& out)
{
    const std::string& s = ToUpper(row["Panel"]);

    if (s == "YES") { out = true; return true; }
    else out = false;

    return false;
}

inline std::string extractparentFolderName(const std::string& fullPath)
{
    std::filesystem::path p(fullPath);
    return p.parent_path().filename().string();
}
