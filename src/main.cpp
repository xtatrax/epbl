#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <string>

char* getConfigPath(){
    HKEY hKey;
    LONG result;
    char Data[1024] = "";	// �l���󂯎��
    DWORD dwType;		// �l�̎�ނ��󂯎��
    DWORD dwSize;		// �f�[�^�̃T�C�Y���󂯎��
    DWORD dwDisposition =0;
    result = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
        TEXT("SOFTWARE\\tatra\\epbl"),
        0,//�\��
        KEY_READ,//�W���A�N�Z�X���j�̂��ׂĂ̌�����g�ݍ��킹������
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
    //�f�[�^�̃T�C�Y���擾
    result = RegQueryValueEx(
        hKey,	// ���݃I�[�v�����Ă���L�[�̃n���h��
        "",	// �擾����l�́u���O�v��������������ւ̃|�C���^
        NULL,	// �\��p�����[�^�BNULL���w�肷��
        &dwType,	// �l�́u��ށv���󂯎��
        (BYTE*)Data,		// �l�́u�f�[�^�v���󂯎��BNULL���w�肷�邱�Ƃ��\�����A�f�[�^�͎󂯎��Ȃ�
        &dwSize		// �I�[����'\0'���܂�Data�̃T�C�Y���擾����
        );
    printf("defailt : %s\n", Data);
    /* �I�� */
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
