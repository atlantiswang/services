#include "ProcessOper.h"
#include "FakeControl.h"
#include "..\Public\easyhook.h"
#include <Windows.h>
#include <fstream>
#include <stdio.h>

 #pragma comment(lib,"psapi.lib")
 #pragma comment(lib,"Aux_ulib.lib")

#pragma warning(disable:4996)

TRACED_HOOK_HANDLE  g_hHookOpenServiceA = new HOOK_TRACE_INFO();
TRACED_HOOK_HANDLE  g_hHookOpenServiceW = new HOOK_TRACE_INFO();
TRACED_HOOK_HANDLE  g_hHookStartServiceA = new HOOK_TRACE_INFO();
TRACED_HOOK_HANDLE  g_hHookStartServiceW = new HOOK_TRACE_INFO();
TRACED_HOOK_HANDLE  g_hHookControlService = new HOOK_TRACE_INFO();
TRACED_HOOK_HANDLE  g_hHookCloseServiceHandle = new HOOK_TRACE_INFO();

//場宎趙
BOOL Init()
{   
    HMODULE hOpenService = LoadLibrary("Advapi32.dll");
    if (!hOpenService){
        return FALSE;
    }

    g_pfnOpenServiceA = (TRCOpenServiceA)GetProcAddress(hOpenService, "OpenServiceA");
    if (!g_pfnOpenServiceA)
    {
        FreeLibrary(hOpenService);
        return FALSE;
    }

    g_pfnOpenServiceW = (TRCOpenServiceW)GetProcAddress(hOpenService, "OpenServiceW");
    if (!g_pfnOpenServiceW)
    {
        FreeLibrary(hOpenService);
        return FALSE;
    }

    g_pfnStartServiceA = (TRCStartServiceA)GetProcAddress(hOpenService, "StartServiceA");
    if (!g_pfnStartServiceA)
    {
        FreeLibrary(hOpenService);
        return FALSE;
    }

    g_pfnStartServiceW = (TRCStartServiceW)GetProcAddress(hOpenService, "StartServiceW");
    if (!g_pfnStartServiceW)
    {
        FreeLibrary(hOpenService);
        return FALSE;
    }

    g_pfnControlService = (TRCControlService)GetProcAddress(hOpenService, "ControlService");
    if (!g_pfnControlService)
    {
        FreeLibrary(hOpenService);
        return FALSE;
    }

    g_pfnCloseServiceHandle = (TRCCloseServiceHandle)GetProcAddress(hOpenService, "CloseServiceHandle");
    if (!g_pfnCloseServiceHandle)
    {
        FreeLibrary(hOpenService);
        return FALSE;
    }
    
    FreeLibrary(hOpenService);

    return TRUE;
}

BOOL InstallHook()
{
    NTSTATUS status;

    status = LhInstallHook(g_pfnOpenServiceA, FakeOpenServiceA, NULL, g_hHookOpenServiceA);
    if (!SUCCEEDED(status))
    {
        return FALSE;
    }
    status = LhSetExclusiveACL(g_hookOpenServiceAclEntriesA, 1, g_hHookOpenServiceA);
    if (!SUCCEEDED(status))
    {
        return FALSE;
    }

    status = LhInstallHook(g_pfnOpenServiceW, FakeOpenServiceW, NULL, g_hHookOpenServiceW);
    if (!SUCCEEDED(status))
    {
        return FALSE;
    }
    status = LhSetExclusiveACL(g_hookOpenServiceAclEntriesW, 1, g_hHookOpenServiceW);
    if (!SUCCEEDED(status))
    {
        return FALSE;
    }

    status = LhInstallHook(g_pfnStartServiceA, FakeStartServiceA, NULL, g_hHookStartServiceA);
    if (!SUCCEEDED(status))
    {
        return FALSE;
    }
    status = LhSetExclusiveACL(g_hookStartServiceAclEntriesA, 1, g_hHookStartServiceA);
    if (!SUCCEEDED(status))
    {
        return FALSE;
    }

    status = LhInstallHook(g_pfnStartServiceW, FakeStartServiceW, NULL, g_hHookStartServiceW);
    if (!SUCCEEDED(status))
    {
        return FALSE;
    }
    status = LhSetExclusiveACL(g_hookOpenServiceAclEntriesW, 1, g_hHookStartServiceW);
    if (!SUCCEEDED(status))
    {
        return FALSE;
    }

    status = LhInstallHook(g_pfnControlService, FakeControlService, NULL, g_hHookControlService);
    if (!SUCCEEDED(status))
    {
        return FALSE;
    }
    status = LhSetExclusiveACL(g_hookControlServiceAclEntries, 1, g_hHookControlService);
    if (!SUCCEEDED(status))
    {
        return FALSE;
    }

    status = LhInstallHook(g_pfnCloseServiceHandle, FakeCloseServiceHandle, NULL, g_hHookCloseServiceHandle);
    if (!SUCCEEDED(status))
    {
        return FALSE;
    }
    status = LhSetExclusiveACL(g_hookCloseServiceHandleAclEntries, 1, g_hHookCloseServiceHandle);
    if (!SUCCEEDED(status))
    {
        return FALSE;
    }

    return TRUE;
}

//境像滲杅
BOOL Hook()
{
    NTSTATUS status;

    status = LhInstallHook(g_pfnOpenServiceA, FakeOpenServiceA, NULL, g_hHookOpenServiceA);
    if (!SUCCEEDED(status))
    {
        char chMsg[MAX_PATH] = { 0 };
        return FALSE;
    }

    status = LhInstallHook(g_pfnOpenServiceW, FakeOpenServiceW, NULL, g_hHookOpenServiceW);
    if (!SUCCEEDED(status))
    {
        char chMsg[MAX_PATH] = { 0 };
        return FALSE;
    }

    status = LhInstallHook(g_pfnStartServiceA, FakeStartServiceA, NULL, g_hHookStartServiceA);
    if (!SUCCEEDED(status))
    {
        char chMsg[MAX_PATH] = { 0 };
        return FALSE;
    }

    status = LhInstallHook(g_pfnStartServiceW, FakeStartServiceW, NULL, g_hHookStartServiceW);
    if (!SUCCEEDED(status))
    {
        char chMsg[MAX_PATH] = { 0 };
        return FALSE;
    }

    status = LhInstallHook(g_pfnControlService, FakeControlService, NULL, g_hHookControlService);
    if (!SUCCEEDED(status))
    {
        char chMsg[MAX_PATH] = { 0 };
        return FALSE;
    }

    status = LhInstallHook(g_pfnCloseServiceHandle, FakeCloseServiceHandle, NULL, g_hHookCloseServiceHandle);
    if (!SUCCEEDED(status))
    {
        char chMsg[MAX_PATH] = { 0 };
        return FALSE;
    }

    return TRUE;
}

//迕像滲杅
BOOL UnInstallHook()
{
    LhUninstallAllHooks();

    if (g_hHookOpenServiceA)
    {
        LhUninstallHook(g_hHookOpenServiceA);
    }

    if (g_hHookOpenServiceW)
    {
        LhUninstallHook(g_hHookOpenServiceW);
    }

    if (g_hHookStartServiceA)
    {
        LhUninstallHook(g_hHookStartServiceA);
    }

    if (g_hHookStartServiceW)
    {
        LhUninstallHook(g_hHookStartServiceW);
    }

    if (g_hHookControlService)
    {
        LhUninstallHook(g_hHookControlService);
    }

    if (g_hHookCloseServiceHandle)
    {
        LhUninstallHook(g_hHookCloseServiceHandle);
    }

    LhWaitForPendingRemovals();

    return TRUE;
}
