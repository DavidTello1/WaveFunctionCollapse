#pragma once
#include <windows.h>
#include <Shobjidl.h>

#include <string>

class FileDialog
{
public:
    static std::string OpenFileDialog(LPWSTR pszFilePath, int numTypes, COMDLG_FILTERSPEC* filters) 
    {
        HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

        if (SUCCEEDED(hr))
        {
            IFileOpenDialog* pFileOpen = NULL;

            // Create the FileOpenDialog object.
            hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

            if (SUCCEEDED(hr))
            {
                // Show the Open dialog box.
                pFileOpen->SetFileTypes(numTypes, filters);   //  choose file types to be displayed
                pFileOpen->SetTitle(L"Open File");            //  heading of dialog box
                hr = pFileOpen->Show(NULL);

                // Get the file name from the dialog box.
                if (SUCCEEDED(hr))
                {
                    IShellItem* pItem;
                    hr = pFileOpen->GetResult(&pItem);
                    if (SUCCEEDED(hr))
                    {
                        //PWSTR pszFilePath;
                        //hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                        //// Display the file name to the user.
                        //if (SUCCEEDED(hr))
                        //{
                        //    MessageBoxW(NULL, pszFilePath, L"File Path", MB_OK);
                        //    CoTaskMemFree(pszFilePath);
                        //}
                        //pItem->Release();

                        // -----
                        LPWSTR pTemp;
                        hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pTemp);
                        wcscpy_s(pszFilePath, MAX_PATH, pTemp);

                        if (SUCCEEDED(hr))
                        {
                            CoTaskMemFree(pTemp);
                        }
                        pItem->Release();
                    }
                }
                pFileOpen->Release();
            }
            CoUninitialize();

            // Convert to string
            char path[MAX_PATH];
            sprintf_s(path, "%ws", pszFilePath);

            return std::string(path);
        }

        return std::string("");
    }
};
