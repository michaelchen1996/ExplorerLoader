// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"


DWORD WINAPI doCreate(LPVOID p)
{
	STARTUPINFO stStartUpInfo = { sizeof(stStartUpInfo) };
	PROCESS_INFORMATION pProcessInfo;
	ZeroMemory(&stStartUpInfo, sizeof(STARTUPINFO));
	stStartUpInfo.cb = sizeof(STARTUPINFO);
	stStartUpInfo.dwFlags = 0x401;// STARTF_USESHOWWINDOW;
	stStartUpInfo.wShowWindow = 1;
	CreateProcess(TEXT("C:\\Users\\michael\\source\\repos\\ExplorerLoader\\x64\\Debug\\Loadee.exe"),
		NULL,
		NULL,
		NULL,
		FALSE,
		CREATE_NEW_CONSOLE|CREATE_UNICODE_ENVIRONMENT|CREATE_DEFAULT_ERROR_MODE,
		NULL,
		TEXT("C:\\Users\\michael\\source\\repos\\ExplorerLoader\\x64\\Debug\\"),
		&stStartUpInfo,
		&pProcessInfo);
	WaitForSingleObject(pProcessInfo.hProcess, INFINITE);
	CloseHandle(pProcessInfo.hThread);
	CloseHandle(pProcessInfo.hProcess);
	return 0;
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		CreateThread(NULL, 0, doCreate, NULL, 0, NULL);
		break;
    case DLL_THREAD_ATTACH:
		break;
    case DLL_THREAD_DETACH:
		break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
