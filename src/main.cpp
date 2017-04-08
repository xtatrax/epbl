#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <string>

std::string getDataPath(){
    HKEY hKey;
    LONG result;
    char Data[1024] = "";	// 値を受け取る
    DWORD dwType;		// 値の種類を受け取る
    DWORD dwSize;		// データのサイズを受け取る
    DWORD dwDisposition =0;
    result = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
        TEXT("SOFTWARE\\tatra\\epbl"),
        0,//予約
        KEY_READ | KEY_WOW64_64KEY,//アクセス権 : 読み取り専用
        &hKey);
    if (result != ERROR_SUCCESS){
        LPTSTR lpBuffer = NULL;
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL, GetLastError(), LANG_USER_DEFAULT,
            (LPTSTR)&lpBuffer, 0, NULL);
        MessageBox(NULL, lpBuffer, "RegOpenKeyEx Error Message", MB_ICONHAND | MB_OK);
        LocalFree(lpBuffer);
        return NULL;
    }
    //データを取得
    result = RegQueryValueEx(
        hKey,           // 現在オープンしているキーのハンドル
        "",             // 取得する値の「名前」が入った文字列へのポインタ
        NULL,           // 予約パラメータ。NULLを指定する
        &dwType,        // 値の「種類」を受け取る
        (BYTE*)Data,    // 値の「データ」を受け取る。NULLを指定することも可能だが、データは受け取れない
        &dwSize         // 終端文字'\0'を含んだDataのサイズを取得する
        );
    //printf("defailt : %s\n", Data);
    /* 終了 */
    RegCloseKey(hKey);

    return Data;
}

void pExec(char* str, int flag)
{
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
    if(CreateProcess(nullptr, str, nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS, nullptr, nullptr, &si, &pi)){
        LPTSTR lpBuffer = NULL;
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL, GetLastError(), LANG_USER_DEFAULT,
            (LPTSTR)&lpBuffer, 0, NULL);
        char msg[1024] ="";
        strcat(msg, lpBuffer);
        strcat(msg, "\n");
        strcat(msg, str);
        char cBuff[256] = "";
        GetConsoleTitle(cBuff, 256);
        MessageBox(FindWindow(NULL, cBuff), msg, "CreateProcess Error Message", MB_ICONHAND | MB_OK);
        LocalFree(lpBuffer);
        CopyFile("c:\\Windows\\system32\\LogonUIOriginal.exe", "c:\\Windows\\system32\\LogonUI.exe", FALSE);
    }
    //データを取得

	CloseHandle(pi.hThread);
	if (flag)
	{
		WaitForSingleObject(pi.hProcess, INFINITE);
	}
	CloseHandle(pi.hProcess);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

    std::string data = getDataPath();
	char str[64] = {};

	char* s = " ";
    std::string exec = data + "\\bin\\LogonUI.exe";

	strcat(str, exec.c_str());

	for (int i = 0; i < __argc; i++)
	{
		strcat(str, s);
		strcat(str, __argv[i]);
	}


    std::ifstream ifs(data + "\\config\\execlist.conf");
    std::string execstr;
    if (ifs.fail())
    {
        char cBuff[256] = "";
        GetConsoleTitle(cBuff, 256);

        MessageBox(
            FindWindow(NULL, cBuff),
            TEXT("file not found"),
            TEXT("error"),
            MB_OK | MB_ICONWARNING);
    }
    while (std::getline(ifs, execstr))
    {
        pExec((char*)execstr.c_str(), 1);
    }
    pExec(str, 0);

    return 0;
}
