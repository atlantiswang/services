#include "ProcessOper.h"
#include "..\Public\easyhook.h"
#include "windows.h"

#pragma data_seg("shared")
HHOOK g_hHook = NULL;
#pragma comment(linker,"/SECTION:shared,RWS")

#ifdef _WIN64
    #ifdef _DEBUG
        #pragma comment(lib, "../lib/debug/x64/EasyHook64.lib")
    #else
        #pragma comment(lib, "../lib/release/x64/EasyHook64.lib")
        
    #endif
#else
    #ifdef _DEBUG
        #pragma comment(lib, "../lib/debug/x86/EasyHook32.lib")
    #else
        #pragma comment(lib, "../lib/release/x86/EasyHook32.lib")
    #endif
#endif


LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam)
{
    return CallNextHookEx(g_hHook, code, wParam, lParam);
}

DWORD WINAPI HookThreadProc(LPVOID lpParamter)
{
    InstallHook();
    return 0;
}

void StartHookThread()
{
    DWORD dwThreadID = 0;
    HANDLE hThread = CreateThread(NULL, 0, HookThreadProc, NULL, 0, &dwThreadID);
    CloseHandle(hThread);
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
    )
{
    // 调用 EasyHook 的入口处理函数
    EasyHookDllMain(hModule, ul_reason_for_call, lpReserved);

    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
        {
            Init();

            StartHookThread();
            if (g_hHook == NULL)
            {
                // 通过鼠标勾子来将dll注入到其它GUI程序中
                g_hHook = SetWindowsHookEx(WH_MOUSE, KeyboardProc, hModule, 0);
            }
            break;
        }    
        case DLL_THREAD_ATTACH:
        {
            break;
        }
        case DLL_THREAD_DETACH:
        {
            break;
        }
        case DLL_PROCESS_DETACH:
        {
            UnInstallHook();
            if (g_hHook != NULL)
            {
                g_hHook = NULL;
            }
            break;
        }
    }
    return TRUE;
}

