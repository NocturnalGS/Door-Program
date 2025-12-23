#pragma once
#include <string>        // std::string
#include <vector>        // std::vector
#include <unordered_map>// std::unordered_map
#include <fstream>       // std::ifstream, std::ofstream
#include <iostream>      // std::ostream
#include <algorithm>    // std::transform, std::find_if
#include <cctype>       // std::isspace, std::toupper
#include <cstdlib>      // std::strtol, std::strtod
#include <climits>      // INT_MIN, INT_MAX
#include <cstring>      // strncpy_s
#include <filesystem>   // extractparentFolderName
#include <shobjidl.h>   // IFileDialog


//forward declarations
struct CsvError;
struct CsvRow;
struct CsvTable;
class CsvReader;
inline std::string Trim(std::string s);
inline std::string ToUpper(std::string s);
inline void CopyCsvText(const CsvRow& row, const char* columnName, char* dst, size_t dstSize);
inline bool ReadInt(const CsvRow& row, const char* columnName, int& outValue);
inline bool ReadUInt(const CsvRow& row, const char* columnName, unsigned int& outValue);
inline bool ReadDouble(const CsvRow& row, const char* columnName, double& outValue);
inline std::string extractparentFolderName(const std::string& fullPath);
inline void WriteField(std::ostream& os, const char* s);

struct CsvError
{
    size_t row_index;     // 1-based, CSV row number
    std::string message;
};

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

class CsvFileDialog
{
public:
    inline static std::string Open();
};

class CsvReader
{
public:
    static CsvTable Read(const std::string& path);

private:
    static std::vector<std::string> ParseLine(const std::string& line);
};


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

inline bool ReadInt(const CsvRow& row, const char* columnName, int& outValue)
{
    const std::string& s = row[columnName];

    if (s.empty())
        return false;

    char* end = nullptr;
    errno = 0;

    long v = std::strtol(s.c_str(), &end, 10);

    if (errno != 0 || end == s.c_str() || *end != '\0')
        return false;

    if (v < INT_MIN || v > static_cast<long>(INT_MAX))
        return false;

    outValue = static_cast<int>(v);
    return true;
}

inline bool ReadUInt(const CsvRow& row, const char* columnName, unsigned int& outValue)
{
    const std::string& s = row[columnName];

    if (s.empty())
        return false;

    char* end = nullptr;
    errno = 0;

    unsigned long v = std::strtoul(s.c_str(), &end, 10);

    if (errno != 0 || end == s.c_str() || *end != '\0')
        return false;

    if (v > UINT_MAX)
        return false;

    outValue = static_cast<unsigned int>(v);
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

inline std::string extractparentFolderName(const std::string& fullPath)
{
    std::filesystem::path p(fullPath);
    return p.parent_path().filename().string();
}


// NOTE:
// This CSV parser assumes:
// - One record per line
// - No multiline fields
// - LibreOffice / internally generated CSVs
// Escaped quotes ("") ARE supported.

inline std::vector<std::string> CsvReader::ParseLine(const std::string& line)
{
    std::vector<std::string> result;
    std::string field;
    bool inQuotes = false;

    for (size_t i = 0; i < line.size(); ++i)
    {
        char c = line[i];

        if (c == '"')
        {
            if (inQuotes && i + 1 < line.size() && line[i + 1] == '"')
            {
                field += '"';  // escaped quote
                ++i;           // skip second quote
            }
            else
            {
                inQuotes = !inQuotes;
            }
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

inline CsvTable CsvReader::Read(const std::string& path)
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

//inline std::string CsvFileDialog::Open()
//{
//    char fileName[MAX_PATH] = "";
//
//    char cwd[MAX_PATH];
//    GetCurrentDirectoryA(MAX_PATH, cwd);
//
//    printf("CWD: %s\n", cwd);
//
//    OPENFILENAMEA ofn {};
//    ofn.lStructSize = sizeof(ofn);
//    ofn.lpstrFilter =
//        "CSV Files (*.csv)\0*.csv\0"
//        "All Files (*.*)\0*.*\0";
//    ofn.lpstrFile = fileName;
//    ofn.lpstrInitialDir = cwd;
//    ofn.nMaxFile = MAX_PATH;
//    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
//    ofn.lpstrDefExt = "csv";
//
//    if (GetOpenFileNameA(&ofn))
//        return fileName;
//
//    return {};
//}

inline std::string CsvFileDialog::Open()
{
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    bool comInitialized = SUCCEEDED(hr);

    IFileDialog* pfd = nullptr;
    std::string result;

    if (SUCCEEDED(CoCreateInstance(CLSID_FileOpenDialog, nullptr,
        CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd))))
    {
        // Build a filter: CSV + All files
        COMDLG_FILTERSPEC filters[] =
        {
            { L"CSV Files (*.csv)", L"*.csv" },
            { L"All Files (*.*)",   L"*.*"   }
        };
        pfd->SetFileTypes(2, filters);
        pfd->SetDefaultExtension(L"csv");
        pfd->SetTitle(L"Select CSV File");

        // Get CWD as wide string
        wchar_t cwd[MAX_PATH];
        GetCurrentDirectoryW(MAX_PATH, cwd);

        IShellItem* folder = nullptr;
        if (SUCCEEDED(SHCreateItemFromParsingName(cwd, nullptr, IID_PPV_ARGS(&folder))))
        {
            pfd->SetDefaultFolder(folder);
            pfd->SetFolder(folder);
            folder->Release();
        }

        if (SUCCEEDED(pfd->Show(nullptr)))
        {
            IShellItem* psi = nullptr;
            if (SUCCEEDED(pfd->GetResult(&psi)))
            {
                PWSTR path = nullptr;
                if (SUCCEEDED(psi->GetDisplayName(SIGDN_FILESYSPATH, &path)))
                {
                    // convert wide → std::string
                    int size = WideCharToMultiByte(CP_UTF8, 0, path, -1, nullptr, 0, nullptr, nullptr);
                    std::string utf8(size - 1, 0);
                    WideCharToMultiByte(CP_UTF8, 0, path, -1, utf8.data(), size, nullptr, nullptr);
                    result = utf8;

                    CoTaskMemFree(path);
                }
                psi->Release();
            }
        }

        pfd->Release();
    }

    if (comInitialized)
        CoUninitialize();

    return result;
}

class Row
{
public:
    explicit Row(std::ostream& os)
        : os_(os), first_(true)
    {}

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

inline void WriteExample()
{

    std::filesystem::path dir = "testfolder";

    // Create folder if it doesn't exist
    std::filesystem::create_directories(dir);

    std::ofstream file(dir / "example.csv");
    if (!file.is_open())
        return;

    // Header row
    Row(file)
        .Field("Name")
        .Field("Age")
        .Field("Score")
        .End();

    // Data rows
    Row(file)
        .Field("Alice")
        .Field(30)
        .Field(95.5)
        .End();

    Row(file)
        .Field("Bob, Jr.")   // comma → auto quoted
        .Field(42)
        .Field(88.0)
        .End();

    Row(file)
        .Field("Charlie \"The Champ\"") // quotes → escaped
        .Field(27)
        .Field(91.25)
        .End();
}