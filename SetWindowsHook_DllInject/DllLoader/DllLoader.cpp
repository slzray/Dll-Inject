#include <Windows.h>
#include <iostream>

using namespace std;

int wmain(int argc, wchar_t* argv[]) {
	
	if (argc < 3) {
		cout << "Invalid param!" << endl;
		return 0;
	}

	const wchar_t* dllPath = argv[1];
	const wchar_t* windowName = argv[2];

	//获取窗口句柄
	HWND hWnd = FindWindowW(windowName, NULL);
	if (hWnd == NULL) {
		cout << "Could not find target window." << endl;
		return EXIT_FAILURE;
	}

	// 获取窗口线程ID
	DWORD pid = NULL;
	DWORD tid = GetWindowThreadProcessId(hWnd, &pid);
	if (tid == NULL) {
		cout << "Could not get thread ID of the target window." << endl;
		return EXIT_FAILURE;
	}

	// 加载dll
	HMODULE dll = LoadLibraryEx(dllPath, NULL, DONT_RESOLVE_DLL_REFERENCES);
	if (dll == NULL) {
		cout << "The DLL could not be found." << endl;
		return EXIT_FAILURE;
	}

	// 获取导出函数地址
	HOOKPROC addr = (HOOKPROC)GetProcAddress(dll, "NextHook"); 
	if (addr == NULL) {
		cout << "The function was not found." << endl;
		return EXIT_FAILURE;
	}

	// 设置消息钩子
	HHOOK handle = SetWindowsHookEx(WH_GETMESSAGE, addr, dll, tid); 
	if (handle == NULL) {
		cout << "Couldn't set the hook with SetWindowsHookEx." << endl;
		return EXIT_FAILURE;
	}

	// 触发钩子
	PostThreadMessage(tid, WM_NULL, NULL, NULL);

	Sleep(1);
	
	//卸载钩子
	BOOL unhook = UnhookWindowsHookEx(handle);
	if (unhook == FALSE) {
		cout << "Could not remove the hook." << endl;
		return EXIT_FAILURE;
	}

	cout << "Inject successful!" << endl;
	return EXIT_SUCCESS;
}
