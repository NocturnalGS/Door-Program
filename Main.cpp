#include <iostream>
#include <fstream>
#include "Windows.h"
#include "CsvFileDialog.h"
#include "CsvReader.h"
#include "CsvWriter.h"
#include "Door.h"
#include "CsvUtils.h"


int main()
{
    char cwd[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, cwd);
    printf("CWD: %s\n", cwd);

    std::string csvPath = CsvFileDialog::Open();
    if (csvPath.empty())
    {
        std::cout << "No file selected\n";
        return 0;
    }

    DoorList doorlist;
    CsvTable doortable = CsvReader::Read(csvPath);
    doorlist.ReadCsvTable(doortable);

    std::string jobname = "Nocturnal";
    std::string filename = MakeTigerStopFilename(StockGroup::Rail, 2.5, jobname);

    std::cout << jobname << "\n";
    std::cout << filename << "\n";


    //// Write out using csv::Row
    //std::ofstream file("parts.csv");

    //// Headers
    //{
    //    csv::Row headerRow(file);
    //    for (const auto& h : table.headers)
    //        headerRow.Field(h.c_str());
    //    headerRow.End();
    //}

    //// Data
    //for (const auto& row : table.rows)
    //{
    //    csv::Row out(file);

    //    for (const auto& header : table.headers)
    //    {
    //        out.Field(row[header].c_str());
    //    }

    //    out.End();
    //}

    return 0;
}
