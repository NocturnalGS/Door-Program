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
    if (doorlist.HasShaker())
        doorlist.WriteTigerStopCsvs(jobName);
    doorlist.WritePanelCsvs(jobName);
    //doorlist.Print();
    
    double linearfootage = doorlist.GetTotalLinearFootage();
    double bonedetaillinearfootage = doorlist.GetTotalLinearFootageBoneDetail();
    if(linearfootage > 0.1)
    {
        std::cout << "Linear Footage of Rails and Stiles: " << linearfootage << "\n";
    }
    if (bonedetaillinearfootage > 0.1)
    {
        std::cout << "Linear Footage of Bone Detail: " << bonedetaillinearfootage << "\n";
    }

    return 0;
}



