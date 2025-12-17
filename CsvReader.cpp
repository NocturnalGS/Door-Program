#include "CsvReader.h"
#include <fstream>

std::vector<std::string> CsvReader::ParseLine(const std::string& line)
{
    std::vector<std::string> result;
    std::string field;
    bool inQuotes = false;

    for (size_t i = 0; i < line.size(); ++i)
    {
        char c = line[i];

        if (c == '"')
        {
            inQuotes = !inQuotes;
        }
        else if (c == ',' && !inQuotes)
        {
            result.push_back(field);
            field.clear();
        }
        else
        {
            field += c;
        }
    }

    result.push_back(field);
    return result;
}

CsvTable CsvReader::Read(const std::string& path)
{
    CsvTable table;
    std::ifstream file(path);

    if (!file.is_open())
        return table;

    std::string line;

    // 1) Read header row
    if (!std::getline(file, line))
        return table;

    table.headers = ParseLine(line);

    // 2) Read data rows
    while (std::getline(file, line))
    {
        auto values = ParseLine(line);
        CsvRow row;

        for (size_t i = 0; i < table.headers.size(); ++i)
        {
            std::string value;
            if (i < values.size())
                value = values[i];

            row.fields[table.headers[i]] = value;
        }

        table.rows.push_back(std::move(row));
    }

    return table;
}
