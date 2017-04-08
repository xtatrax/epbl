#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <string>

char* getConfigPath(){
    HKEY hKey;
    LONG result;
    char Data[1024] = "";	// 値を受け取る
    DWORD dwType;		// 値の種類を受け取る
    DWORD dwSize;		// データのサイズを受け取る
    DWORD dwDisposition =0;
    result = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
        TEXT("SOFTWARE\\tatra\\epbl"),
        0,//予約
        KEY_READ,//標準アクセス権）のすべての権利を組み合わせたもの
        &hKey);
    if (result != ERROR_SUCCESS){
        LPTSTR lpBuffer = NULL;
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL, GetLastError(), LANG_USER_DEFAULT,
            (LPTSTR)&lpBuffer, 0, NULL);
        MessageBox(NULL, lpBuffer, "Last Error Message", MB_ICONHAND | MB_OK);
        LocalFree(lpBuffer);
        return NULL;
    }
    //データのサイズを取得
    result = RegQueryValueEx(
        hKey,	// 現在オープンしているキーのハンドル
        "",	// 取得する値の「名前」が入った文字列へのポインタ
        NULL,	// 予約パラメータ。NULLを指定する
        &dwType,	// 値の「種類」を受け取る
        (BYTE*)Data,		// 値の「データ」を受け取る。NULLを指定することも可能だが、データは受け取れない
        &dwSize		// 終端文字'\0'を含んだDataのサイズを取得する
        );
    printf("defailt : %s\n", Data);
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
	CreateProcess(nullptr, str, nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS, nullptr, nullptr, &si, &pi);
	CloseHandle(pi.hThread);
	if (flag)
	{
		WaitForSingleObject(pi.hProcess, INFINITE);
	}
	CloseHandle(pi.hProcess);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	int i;

	char str[64] = {};

	char* s = " ";
	char* exec = "C:\\logonExt\\LogonUI.exe";

	strcat(str, exec);

	for (i = 0; i < __argc; i++)
	{
		strcat(str, s);
		strcat(str, __argv[i]);
	}

	pExec(str, 0);

    std::ifstream ifs(getConfigPath());
    std::string execstr;
    if (ifs.fail())
    {
        MessageBox(
            NULL,
            TEXT("file not found"),
            TEXT("error"),
            MB_OK | MB_ICONWARNING);
        return -1;
    }
    while (std::getline(ifs, execstr))
    {
        pExec((char*)execstr.c_str(), 1);
    }

    return 0;
}
