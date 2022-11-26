#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <string>
#pragma warning(disable : 4996)

class Injector
{
public:
	HANDLE process;
	HWND hwndproc;

	DWORD GetModule(DWORD pid, const char* name)
	{
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
		MODULEENTRY32 mEntry;
		mEntry.dwSize = sizeof(MODULEENTRY32);
		do
		{
			if (!strcmp(mEntry.szModule, name))
			{
				CloseHandle(snapshot);
				return (DWORD)mEntry.modBaseAddr;
			}
		} while (Module32Next(snapshot, &mEntry));
	}

	DWORD GetProcessByExeName(const char* ExeName)
	{
		DWORD Pid;

		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(PROCESSENTRY32);

		HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
		if (hProcessSnap == INVALID_HANDLE_VALUE)
		{
			MessageBox(NULL, "Error = " + GetLastError(), "Error (GetProcessByExeName)", MB_OK | MB_ICONERROR);
			return false;
		}

		if (Process32First(hProcessSnap, &pe32))
		{
			do
			{
				if (strcmpi(pe32.szExeFile, ExeName) == 0)
				{
					CloseHandle(hProcessSnap);
					return pe32.th32ProcessID;
				}
			} while (Process32Next(hProcessSnap, &pe32));
		}

		CloseHandle(hProcessSnap);
		return 0;
	}

	bool inject(DWORD pid, const char* dll)
	{
		char myDLL[MAX_PATH];
		GetFullPathName(dll, MAX_PATH, myDLL, 0);

		HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, pid);

		LPVOID allocatedMem = VirtualAllocEx(hProcess, NULL, sizeof(myDLL), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		WriteProcessMemory(hProcess, allocatedMem, myDLL, sizeof(myDLL), NULL);

		CreateRemoteThread(hProcess, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibrary, allocatedMem, 0, 0);

		CloseHandle(hProcess);

		return TRUE;
	}

private:
};