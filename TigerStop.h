#pragma once
#include <cstring>
#include "Door.h"
#include <format>

enum class StockGroup
{
    Rail,
    Stile
};

inline StockGroup GetStockGroup(ShakerPart part)
{
    switch (part)
    {
    case TOP_RAIL:
    case BOTTOM_RAIL:
    case MID_RAIL:
        return StockGroup::Rail;

    case LEFT_STILE:
    case RIGHT_STILE:
    case MID_STILE:
        return StockGroup::Stile;

    default:
        return StockGroup::Rail; // safe fallback
    }
}

struct TigerStopCut
{
    double length;
    unsigned int quantity;
    double nominal_width;
};

inline std::string FormatTrimmed(double value)
{
    // 1. Round to 2 decimal places
    std::string s = std::format("{:.2f}", value);

    // 2. Remove trailing zeros
    if (s.find('.') != std::string::npos)
    {
        s.erase(s.find_last_not_of('0') + 1, std::string::npos);
        // 3. Remove trailing decimal point if it's the last character
        if (s.back() == '.')
        {
            s.pop_back();
        }
    }
    return s;
}

inline std::string MakeTigerStopFilename(StockGroup group, double width, std::string jobname)
{
    std::string g = (group == StockGroup::Rail) ? "Rails" : "Stiles";
    std::string s_width = FormatTrimmed(width);
    std::string formatted_str = std::format("{} {} {}inch", jobname, g, s_width);
    return formatted_str;
}