#include "CsvFileDialog.h"
#include <windows.h>
#include <commdlg.h>

#pragma comment(lib, "Comdlg32.lib")

std::string CsvFileDialog::Open()
{
    char fileName[MAX_PATH] = "";

    char cwd[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, cwd);

    OPENFILENAMEA ofn{};
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFilter =
        "CSV Files (*.csv)\0*.csv\0"
        "All Files (*.*)\0*.*\0";
    ofn.lpstrFile = fileName;
    ofn.lpstrInitialDir = cwd;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
    ofn.lpstrDefExt = "csv";

    if (GetOpenFileNameA(&ofn))
        return fileName;

    return {};
}
