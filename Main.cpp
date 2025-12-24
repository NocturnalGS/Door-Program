#include <iostream>
#include <string>
#include "Windows.h"
#include "Door.h"
#include "CsvUtils.h"

int main()
{
    char cwd[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, cwd);
    std::string jobName = extractparentFolderName(cwd);
    std::cout << jobName << "\n";

    std::string csvPath = CsvFileDialog::Open();
    if (csvPath.empty())
    {
        std::cout << "No file selected\n";
        return 0;
    }

    CsvTable doortable = CsvReader::Read(csvPath);
    DoorList doorlist(doortable);
    doorlist.WriteHTMLReport(jobName.c_str());
    doorlist.WriteTigerStopCsvs(jobName);
    doorlist.WritePanelCsvs(jobName);
    //doorlist.Print();

    return 0;
}



