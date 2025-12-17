#pragma once

#include <ostream>
#include <cstring>
#include "TigerStop.h"

/*
    csv_writer.h

    Minimal, header-only CSV writer.

    - No dynamic allocation
    - No std::string required
    - Proper CSV quoting/escaping
    - Mixed-type rows supported
*/

namespace csv
{

    // ------------------------------------------------------------
    // Internal helper: write one CSV field with proper escaping
    // ------------------------------------------------------------
    inline void WriteField(std::ostream& os, const char* s)
    {
        if (!s)
            return;

        bool needsQuotes = false;
        for (const char* p = s; *p; ++p)
        {
            if (*p == ',' || *p == '"' || *p == '\n' || *p == '\r')
            {
                needsQuotes = true;
                break;
            }
        }

        if (!needsQuotes)
        {
            os << s;
            return;
        }

        os << '"';
        for (const char* p = s; *p; ++p)
        {
            if (*p == '"')
                os << "\"\"";
            else
                os << *p;
        }
        os << '"';
    }

    // ------------------------------------------------------------
    // CSV row writer (mixed-type, streaming)
    // ------------------------------------------------------------
    class Row
    {
    public:
        explicit Row(std::ostream& os)
            : os_(os), first_(true)
        {
        }

        Row& Field(const char* s)
        {
            WriteSeparator();
            if (s)
                WriteField(os_, s);
            // else write empty field
            return *this;
        }

        Row& Field(int v)
        {
            WriteSeparator();
            os_ << v;
            return *this;
        }

        Row& Field(double v)
        {
            WriteSeparator();
            os_ << v;
            return *this;
        }

        void End()
        {
            os_ << '\n';
        }

    private:
        void WriteSeparator()
        {
            if (!first_)
                os_ << ',';
            first_ = false;
        }

        std::ostream& os_;
        bool first_;
    };

}

//static void AggregateTigerStopCuts(std::vector<TigerStopCut>& cuts)
//{
//    constexpr double EPS = 0.0001;
//    std::vector<TigerStopCut> out;
//
//    for (const auto& c : cuts)
//    {
//        bool merged = false;
//
//        for (auto& o : out)
//        {
//            if (o.group == c.group &&
//                std::fabs(o.nominal_width - c.nominal_width) < EPS &&
//                std::fabs(o.length - c.length) < EPS)
//            {
//                o.quantity += c.quantity;
//                merged = true;
//                break;
//            }
//        }
//
//        if (!merged)
//            out.push_back(c);
//    }
//
//    cuts.swap(out);
//}


//static std::string MakeTigerStopFilename(StockGroup group, double width)
//{
//    char buf[64];
//
//    const char* g = (group == StockGroup::Rail) ? "Rails" : "Stiles";
//    snprintf(buf, sizeof(buf), "%s_%.3f.csv", g, width);
//
//    return buf;
//}



//void WriteTigerStopCsvs(const char* folder,
//    const std::vector<TigerStopCut>& cuts)
//{
//    using Key = std::pair<StockGroup, double>;
//    std::map<Key, std::vector<TigerStopCut>> groups;
//
//    for (const auto& c : cuts)
//        groups[{c.group, c.nominal_width}].push_back(c);
//
//    for (const auto& [key, list] : groups)
//    {
//        const auto& [group, width] = key;
//        std::string filename = MakeTigerStopFilename(group, width);
//
//        std::string fullpath = std::string(folder) + "\\" + filename;
//        std::ofstream file(fullpath);
//
//        csv::Row header(file);
//        header.Field("Length")
//            .Field("Quantity")
//            .End();
//
//        for (const auto& c : list)
//        {
//            csv::Row row(file);
//            row.Field(c.length)
//                .Field(static_cast<int>(c.quantity))
//                .End();
//        }
//    }
//}
