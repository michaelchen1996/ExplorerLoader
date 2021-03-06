// ExplorerInjecter.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#define DLL_PATH TEXT("C:\\Users\\michael\\source\\repos\\ExplorerLoader\\x64\\Debug\\LoadDLL.dll")

BOOL InjectDll(HANDLE hProcess, LPCTSTR szDllPath)
{
	HANDLE hThread = NULL;
	LPVOID pRemoteBuf = NULL;
	DWORD dwBufSize = (DWORD)(_tcslen(szDllPath) + 1) * sizeof(TCHAR);
	LPTHREAD_START_ROUTINE pThreadProc;
	pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwBufSize, MEM_COMMIT, PAGE_READWRITE);
	if (NULL == pRemoteBuf)
	{
		printf("VirtAlloc error\n");
		return FALSE;
	}
	if (!WriteProcessMemory(hProcess, pRemoteBuf, (LPVOID)szDllPath, dwBufSize, NULL))
	{
		printf("WriteMemory error\n");
		return FALSE;
	}
	pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "LoadLibraryW");
	if (NULL == pThreadProc)
	{
		printf("GetAddress error\n");
		return FALSE;
	}
	hThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc, pRemoteBuf, 0, NULL);
	if (NULL == hThread)
	{
		printf("CreateRemoteThread error %d\n", GetLastError());
		return FALSE;
	}
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	CloseHandle(hProcess);
	return TRUE;
}

DWORD GetProcessId(LPCTSTR str)
{
	HANDLE         hProcessSnap = NULL;
	PROCESSENTRY32 pe32 = { 0 };

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hProcessSnap == (HANDLE)-1)
		return -1;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(hProcessSnap, &pe32))
	{
		do
		{
			if (_tcsicmp(str, pe32.szExeFile) == 0)
			{
				CloseHandle(hProcessSnap);
				return pe32.th32ProcessID;
			}
		} while (Process32Next(hProcessSnap, &pe32));
	}
	CloseHandle(hProcessSnap);
	return -1;
}

BOOL EnableDebugPriv()
{
	HANDLE hToken;
	LUID sedebugnameValue;
	TOKEN_PRIVILEGES tkp;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		printf("提权失败。\n");
		return FALSE;
	}

	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &sedebugnameValue))
	{
		CloseHandle(hToken);
		printf("提权失败。\n");
		return FALSE;
	}
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = sedebugnameValue;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof tkp, NULL, NULL))
	{
		printf("提权失败。\n");
		CloseHandle(hToken);
		return FALSE;
	}
	else
	{
		printf("提权成功！\n");
		return TRUE;
	}
}

int main()
{
	EnableDebugPriv();
	DWORD DwPid;
	HANDLE hProcess = NULL;

	DwPid = GetProcessId(TEXT("explorer.exe"));
	if (DwPid < 0)
	{
		printf("GetProcessId Failed");
		return -1;
	}
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, DwPid);
	if (!hProcess)
	{
		printf("OpenProcess Failed");
		return -1;
	}
	printf("Inject start\n");
	InjectDll(hProcess, DLL_PATH);
	system("PAUSE");
	//if (!CreateProcess(TEXT("C:\\Users\\michael\\source\\repos\\ExplorerLoader\\x64\\Debug\\Loadee.exe"),
	//	NULL, NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL))
	//{
	//	printf("CreateProcess error %d\n", GetLastError());
	//	return FALSE;
	//}
    return 0;
}

