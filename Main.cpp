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
    std::string jobName = extractparentFolderName(cwd);

    std::string csvPath = CsvFileDialog::Open();
    if (csvPath.empty())
    {
        std::cout << "No file selected\n";
        return 0;
    }

    DoorList doorlist;
    CsvTable doortable = CsvReader::Read(csvPath);
    doorlist.ReadCsvTable(doortable);

    std::string filename = MakeTigerStopFilename(StockGroup::Rail, 2.5, jobName);
    std::cout << filename << "\n";

    return 0;
}
