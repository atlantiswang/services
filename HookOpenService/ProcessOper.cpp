#include "ProcessOper.h"
#include "FakeControl.h"
#include "..\Public\easyhook.h"
#include <Windows.h>
#include <fstream>
#include <stdio.h>

 #pragma comment(lib,"psapi.lib")
 #pragma comment(lib,"Aux_ulib.lib")

#pragma warning(disable:4996)

TRACED_HOOK_HANDLE  hHookOpenServiceA = new HOOK_TRACE_INFO();
TRACED_HOOK_HANDLE  hHookOpenServiceW = new HOOK_TRACE_INFO();
TRACED_HOOK_HANDLE  hHookStartServiceA = new HOOK_TRACE_INFO();
TRACED_HOOK_HANDLE  hHookStartServiceW = new HOOK_TRACE_INFO();
TRACED_HOOK_HANDLE  hHookControlService = new HOOK_TRACE_INFO();
TRACED_HOOK_HANDLE  hHookCloseServiceHandle = new HOOK_TRACE_INFO();

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

    status = LhInstallHook(g_pfnOpenServiceA, FakeOpenServiceA, NULL, hHookOpenServiceA);
    if (!SUCCEEDED(status))
    {
        return FALSE;
    }
    status = LhSetExclusiveACL(hookOpenServiceA_ACLEntries, 1, hHookOpenServiceA);
    if (!SUCCEEDED(status))
    {
        return FALSE;
    }

    status = LhInstallHook(g_pfnOpenServiceW, FakeOpenServiceW, NULL, hHookOpenServiceW);
    if (!SUCCEEDED(status))
    {
        return FALSE;
    }
    status = LhSetExclusiveACL(hookOpenServiceW_ACLEntries, 1, hHookOpenServiceW);
    if (!SUCCEEDED(status))
    {
        return FALSE;
    }

    status = LhInstallHook(g_pfnStartServiceA, FakeStartServiceA, NULL, hHookStartServiceA);
    if (!SUCCEEDED(status))
    {
        return FALSE;
    }
    status = LhSetExclusiveACL(hookStartServiceA_ACLEntries, 1, hHookStartServiceA);
    if (!SUCCEEDED(status))
    {
        return FALSE;
    }

    status = LhInstallHook(g_pfnStartServiceW, FakeStartServiceW, NULL, hHookStartServiceW);
    if (!SUCCEEDED(status))
    {
        return FALSE;
    }
    status = LhSetExclusiveACL(hookOpenServiceW_ACLEntries, 1, hHookStartServiceW);
    if (!SUCCEEDED(status))
    {
        return FALSE;
    }

    status = LhInstallHook(g_pfnControlService, FakeControlService, NULL, hHookControlService);
    if (!SUCCEEDED(status))
    {
        return FALSE;
    }
    status = LhSetExclusiveACL(hookControlService_ACLEntries, 1, hHookControlService);
    if (!SUCCEEDED(status))
    {
        return FALSE;
    }

    status = LhInstallHook(g_pfnCloseServiceHandle, FakeCloseServiceHandle, NULL, hHookCloseServiceHandle);
    if (!SUCCEEDED(status))
    {
        return FALSE;
    }
    status = LhSetExclusiveACL(hookCloseServiceHandle_ACLEntries, 1, hHookCloseServiceHandle);
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

    status = LhInstallHook(g_pfnOpenServiceA, FakeOpenServiceA, NULL, hHookOpenServiceA);
    if (!SUCCEEDED(status))
    {
        char chMsg[MAX_PATH] = { 0 };
        return FALSE;
    }

    status = LhInstallHook(g_pfnOpenServiceW, FakeOpenServiceW, NULL, hHookOpenServiceW);
    if (!SUCCEEDED(status))
    {
        char chMsg[MAX_PATH] = { 0 };
        return FALSE;
    }

    status = LhInstallHook(g_pfnStartServiceA, FakeStartServiceA, NULL, hHookStartServiceA);
    if (!SUCCEEDED(status))
    {
        char chMsg[MAX_PATH] = { 0 };
        return FALSE;
    }

    status = LhInstallHook(g_pfnStartServiceW, FakeStartServiceW, NULL, hHookStartServiceW);
    if (!SUCCEEDED(status))
    {
        char chMsg[MAX_PATH] = { 0 };
        return FALSE;
    }

    status = LhInstallHook(g_pfnControlService, FakeControlService, NULL, hHookControlService);
    if (!SUCCEEDED(status))
    {
        char chMsg[MAX_PATH] = { 0 };
        return FALSE;
    }

    status = LhInstallHook(g_pfnCloseServiceHandle, FakeCloseServiceHandle, NULL, hHookCloseServiceHandle);
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

    if (hHookOpenServiceA)
    {
        LhUninstallHook(hHookOpenServiceA);
    }

    if (hHookOpenServiceW)
    {
        LhUninstallHook(hHookOpenServiceW);
    }

    if (hHookStartServiceA)
    {
        LhUninstallHook(hHookStartServiceA);
    }

    if (hHookStartServiceW)
    {
        LhUninstallHook(hHookStartServiceW);
    }

    if (hHookControlService)
    {
        LhUninstallHook(hHookControlService);
    }

    if (hHookCloseServiceHandle)
    {
        LhUninstallHook(hHookCloseServiceHandle);
    }

    LhWaitForPendingRemovals();

    return TRUE;
}
