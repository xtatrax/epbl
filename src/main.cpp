#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <string>
#include <time.h>

void print_log(std::string file, const char *format, ...){
    char tmp[2048];
    va_list arg;
    va_start(arg, format);
    vsprintf(tmp, format, arg);
    va_end(arg);
    time_t now = time(NULL);
    struct tm *pnow = localtime(&now);

    std::ofstream outputfile(file);
    outputfile << pnow->tm_year + 1900 << "/" << pnow->tm_mon + 1 << "/" << pnow->tm_mday <<
    "/" << pnow->tm_hour << ":" << pnow->tm_min << ":" <<  pnow->tm_sec << tmp;
    outputfile.close();
}


std::string getDataPath(){
    HKEY hKey;
    LONG result;
    char Data[1024] = "";	// �l���󂯎��
    DWORD dwType;		// �l�̎�ނ��󂯎��
    DWORD dwSize;		// �f�[�^�̃T�C�Y���󂯎��
    DWORD dwDisposition =0;
    result = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
        TEXT("SOFTWARE\\tatra\\epbl"),
        0,//�\��
        KEY_READ | KEY_WOW64_64KEY,//�A�N�Z�X�� : �ǂݎ���p
        &hKey);
    if (result != ERROR_SUCCESS){
        LPTSTR lpBuffer = NULL;
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL, GetLastError(), LANG_USER_DEFAULT,
            (LPTSTR)&lpBuffer, 0, NULL);
        print_log("c:\epbllog.txt","RegOpenKeyEx Error Message (%d) : %s", result , lpBuffer);
        LocalFree(lpBuffer);
        return NULL;
    }
    //�f�[�^���擾
    result = RegQueryValueEx(
        hKey,           // ���݃I�[�v�����Ă���L�[�̃n���h��
        "",             // �擾����l�́u���O�v��������������ւ̃|�C���^
        NULL,           // �\��p�����[�^�BNULL���w�肷��
        &dwType,        // �l�́u��ށv���󂯎��
        (BYTE*)Data,    // �l�́u�f�[�^�v���󂯎��BNULL���w�肷�邱�Ƃ��\�����A�f�[�^�͎󂯎��Ȃ�
        &dwSize         // �I�[����'\0'���܂�Data�̃T�C�Y���擾����
        );
    //printf("defailt : %s\n", Data);
    /* �I�� */
    RegCloseKey(hKey);

    return Data;
}

void pExec(char* str, int flag)
{
    print_log(getDataPath() + "log.txt", "exec : %s", str);

	PROCESS_INFORMATION pi;
	STARTUPINFO si;
    int ret;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
    ret = CreateProcess(nullptr, str, nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS, nullptr, nullptr, &si, &pi);
    if(ret >= 2){
        LPTSTR lpBuffer = NULL;
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL, GetLastError(), LANG_USER_DEFAULT,
            (LPTSTR)&lpBuffer, 0, NULL);
        print_log(getDataPath() + "log.txt" ,"CreateProcess Error Message (%d) : %s", ret, lpBuffer);
        LocalFree(lpBuffer);
        //CopyFile("c:\\Windows\\system32\\LogonUIOriginal.exe", "c:\\Windows\\system32\\LogonUI.exe", FALSE);
    }
    //�f�[�^���擾

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
	char str[128] = {};

	char* s = " ";
    std::string exec = data + "\\bin\\LogonUI.exe";

	strcat(str, exec.c_str());

	for (int i = 0; i < __argc; i++)
	{
		strcat(str, s);
		strcat(str, __argv[i]);
	}
    pExec(str, 0);


    std::ifstream ifs(data + "\\config\\execlist.conf");
    std::string execstr;
    if (ifs.fail())
    {
        print_log(getDataPath() + "log.txt", "file not found : %s", (data + "\\config\\execlist.conf").c_str());
    }
    while (std::getline(ifs, execstr))
    {
        pExec((char*)execstr.c_str(), 0);
    }

    return 0;
}
