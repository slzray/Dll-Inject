// DllLoader.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>

int InjectDll(unsigned int pid, const wchar_t* dllPath)
{
	HANDLE hProcess = NULL, hThread = NULL;

    //打开进程
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (hProcess == NULL) {
        std::cout << "OpenProcess error: " << GetLastError() << std::endl;
        return -1;
    }

    int cb = 2 + wcslen(dllPath) * 2;

    //在目标进程中申请一块内存，返回分配内存的首地址
    PWSTR pszLibFileRemote = (PWSTR)VirtualAllocEx(hProcess, NULL, cb, MEM_COMMIT, PAGE_READWRITE);
    if (pszLibFileRemote == NULL) {
        std::cout << "VirtualAllocEx error: " << GetLastError() << std::endl;
        return -2;
    }

    //往该内存写入该DLL字符串
    if (!WriteProcessMemory(hProcess, pszLibFileRemote, (PVOID)dllPath, cb, NULL)){
        std::cout << "WriteProcessMemory error: " << GetLastError() << std::endl;
        return -3;
    }

    //创建远程线程执行LoadLibraryW(pszLibFileRemote)
    hThread = CreateRemoteThread(hProcess, NULL, 0, (PTHREAD_START_ROUTINE)LoadLibraryW, pszLibFileRemote, 0, NULL);
    if (hThread == NULL) {
        std::cout << "CreateRemoteThread error: " << GetLastError() << std::endl;
        return -4;
    }

    WaitForSingleObject(hThread, INFINITE);

    //释放
    VirtualFreeEx(hProcess, pszLibFileRemote, 0, MEM_RELEASE);
    CloseHandle(hThread);
    CloseHandle(hProcess);
    return 0;
}


int wmain(int argc,wchar_t* argv[])
{
    if (argc < 3) {
        std::cout << "Invalid param!" << std::endl;
        return 0;
    }

    InjectDll(wcstol(argv[1], nullptr, 10), argv[2]);

    return 0;
}
