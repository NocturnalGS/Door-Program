#include <iostream>
#include "Windows.h"
#include "Door.h"
#include "CsvUtils.h"
#include "HTML.h"

void WriteExample();
//void GenerateDoorSheet();

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
    std::filesystem::path p(csvPath);
    // filename without extension
    std::string filename = p.stem().string();

    CsvTable doortable = CsvReader::Read(csvPath);
    DoorList doorlist(doortable);
    doorlist.WriteHTMLReport(jobName.c_str());

    // add mid rail and multiple to report drawing
    // Add panel csv!!

    std::string dir = filename;
    doorlist.WriteTigerStopCsvs(dir, jobName);
    doorlist.Print();

    return 0;
}



