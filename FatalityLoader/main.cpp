#include "memory.hpp"
Injector inj;
DWORD pid;

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
	
	inj.hwndproc = FindWindowA(0, "Counter-Strike: Global Offensive - Direct3D 9");
	GetWindowThreadProcessId(inj.hwndproc, &pid);
	inj.process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

	if (pid > 0) {
		TerminateProcess(inj.process, 0);
	}
	
	DWORD pid_steam = inj.GetProcessByExeName("steam.exe");
	if (pid_steam > 0) {
		inj.inject(pid_steam, "steam_patcher.dll");
	}
	else {
		ShellExecuteA(NULL, "open", "steam://", NULL, NULL, SW_SHOWNORMAL);
		Sleep(3000);
		inj.inject(pid_steam, "steam_patcher.dll");
	}

	ShellExecuteA(NULL, "open", "steam://rungameid/730", NULL, NULL, SW_SHOWNORMAL);

	while (true) {
		inj.hwndproc = FindWindowA(0, "Counter-Strike: Global Offensive - Direct3D 9");
		GetWindowThreadProcessId(inj.hwndproc, &pid);
		inj.process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

		if (inj.GetModule(pid, "serverbrowser.dll") > 0) {
			inj.inject(pid, "desync.dll");
			exit(1);
		}
	}
}