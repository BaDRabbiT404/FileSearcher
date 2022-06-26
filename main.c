#include <stdio.h>
#include <Windows.h>
#include <malloc.h>
#include <string.h>

#define PATH L"C:\\Users\\1"
#define DEST_PATH  L"C:\\Users\\1\\log.txt"
#define MASK_FOUND L"cool.mp4"
#define SIZE_BUF 260

void WriteData(HANDLE hFile, LPCWSTR path);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    HANDLE hFile = CreateFile(DEST_PATH, GENERIC_WRITE,
        FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    WriteData(hFile, PATH);
    CloseHandle(hFile);
    return 0;
}

void WriteData(HANDLE hFile, LPCWSTR path)
{
    WIN32_FIND_DATA wfd = { 0 };
    SetCurrentDirectory(path);

    HANDLE search = FindFirstFile(MASK_FOUND, &wfd); //1 параметр - маска поиска!
    if (search == INVALID_HANDLE_VALUE)
        return;
    do
    {
        LPWSTR strTmp = (LPWSTR)calloc(SIZE_BUF + 1, sizeof(WCHAR));

        GetCurrentDirectory(SIZE_BUF, strTmp);
        wcscat_s(strTmp, SIZE_BUF, L"\\");
        wcscat_s(strTmp, SIZE_BUF, wfd.cFileName);

        LPWSTR strTmp_ = (LPWSTR)calloc(wcslen(strTmp) + 1, sizeof(WCHAR));
        wcscpy_s(strTmp_, wcslen(strTmp) + 1, strTmp);

        if (wcscmp(wfd.cFileName, L".") &&
            wcscmp(wfd.cFileName, L".."))
        {//Тут форматирование для записи в файл
            wcscat_s(strTmp, SIZE_BUF, L"\t\t\t");
            if (((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) &&
                (!(wfd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)))
            {
                wcscat_s(strTmp, SIZE_BUF, L"DIR");
                wcscat_s(strTmp, SIZE_BUF, L"\r\n");
                DWORD iCount = 0;
                WriteFile(hFile, strTmp, wcslen(strTmp) * sizeof(WCHAR), &iCount, NULL);
                WriteData(hFile, strTmp_);
                SetCurrentDirectory(path); //<- не забыть!!!!
            }
            else
            {
                wcscat_s(strTmp, SIZE_BUF, L"FILE");
                wcscat_s(strTmp, SIZE_BUF, L"\r\n");
                DWORD iCount = 0;
                WriteFile(hFile, strTmp, wcslen(strTmp) * sizeof(WCHAR), &iCount, NULL);
            }
        }
        free(strTmp);
        free(strTmp_);
        //. и .. - не записывать
    } while (FindNextFile(search, &wfd));

    FindClose(search);
}
