#pragma once
#include <string>
#include <vector>
#include <unordered_map>


struct CsvRow
{
    std::unordered_map<std::string, std::string> fields;

    const std::string& operator[](const std::string& key) const
    {
        static const std::string empty;
        auto it = fields.find(key);
        return (it != fields.end()) ? it->second : empty;
    }
};

struct CsvTable
{
    std::vector<std::string> headers;
    std::vector<CsvRow> rows;
};

class CsvReader
{
public:
    static CsvTable Read(const std::string& path);

private:
    static std::vector<std::string> ParseLine(const std::string& line);
};

