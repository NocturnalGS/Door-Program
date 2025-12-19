#include <iostream>
#include "Windows.h"
#include "Door.h"
#include "CsvUtils.h"

void WriteExample();
int main()
{
    char cwd[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, cwd);
    //printf("CWD: %s\n", cwd);
    std::string jobName = extractparentFolderName(cwd);
    //vWriteExample();
    std::string csvPath = CsvFileDialog::Open();
    if (csvPath.empty())
    {
        std::cout << "No file selected\n";
        return 0;
    }

    CsvTable doortable = CsvReader::Read(csvPath);
    DoorList doorlist(doortable);

    //doorlist.WriteTigerStopCsvs(cwd);


    std::string filename = MakeTigerStopFilename(StockGroup::Rail, 2.5, jobName);
    std::cout << filename << "\n";

    return 0;
}

void WriteExample()
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
