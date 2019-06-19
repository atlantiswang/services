#include "FakeControl.h"
#include "json.h"
#include <fstream>
#include <map>

#define JSON_FILE_PATH "C:\\content.json"

TRCOpenServiceA g_pfnOpenServiceA = NULL;
TRCOpenServiceW g_pfnOpenServiceW = NULL;
TRCStartServiceA g_pfnStartServiceA = NULL;
TRCStartServiceW g_pfnStartServiceW = NULL;
TRCControlService g_pfnControlService = NULL;
TRCCloseServiceHandle g_pfnCloseServiceHandle = NULL;

ULONG hookOpenServiceA_ACLEntries[1] = { 0 };
ULONG hookOpenServiceW_ACLEntries[1] = { 0 };
ULONG hookStartServiceA_ACLEntries[1] = { 0 };
ULONG hookStartServiceW_ACLEntries[1] = { 0 };
ULONG hookControlService_ACLEntries[1] = { 0 };
ULONG hookCloseServiceHandle_ACLEntries[1] = { 0 };

std::map<std::string, RCFORBIDDENINFO> g_JsonDataMap;

std::map<std::string, RCHANDLE> g_Service2RCHandleMap;

std::wstring Ansi2WChar(const char* pszSrc, int nLen)
{
    int nSize = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszSrc, nLen, 0, 0);
    if (nSize <= 0) return NULL;

    WCHAR *pwszDst = new WCHAR[nSize + 1];
    if (NULL == pwszDst) return NULL;

    MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszSrc, nLen, pwszDst, nSize);
    pwszDst[nSize] = 0;

    if (pwszDst[0] == 0xFEFF)
        for (int i = 0; i < nSize; i++)
            pwszDst[i] = pwszDst[i + 1];

    std::wstring wcharString(pwszDst);
    delete[] pwszDst;

    return wcharString;
}

void LoadJsonFile()
{
    g_JsonDataMap.clear();
    std::ifstream ifs;
    ifs.open(JSON_FILE_PATH);
    Json::Reader jsonReader;
    Json::Value root;
    if (!jsonReader.parse(ifs, root, false))
    {
        return;
    }

    int arraySize = root.size();
    RCForbiddenInfo forbiddenInfo = { 0 };

    for (int i = 0; i < arraySize; i++)
    {
        memset(&forbiddenInfo, 0, sizeof(forbiddenInfo));
        Json::Value valueRoot = root[i];

        strcpy_s(forbiddenInfo.serviceName, 128, valueRoot["serviceName"].asCString());
        forbiddenInfo.startForbidden = valueRoot["bForbiddenStart"].asBool();
        forbiddenInfo.stopForbidden = valueRoot["bForbiddenStop"].asBool();
        g_JsonDataMap.insert(std::make_pair(forbiddenInfo.serviceName, forbiddenInfo));
    }
    ifs.close();
}

SC_HANDLE WINAPI FakeOpenServiceA(
    _In_            SC_HANDLE               hSCManager,
    _In_            LPCSTR                lpServiceName,
    _In_            DWORD                   dwDesiredAccess
    )
{
    LoadJsonFile();
    SC_HANDLE openHandleA = g_pfnOpenServiceA(hSCManager, lpServiceName, dwDesiredAccess);
    std::map<std::string, RCFORBIDDENINFO>::iterator itMap = g_JsonDataMap.begin();
    for (; itMap != g_JsonDataMap.end(); ++itMap)
    {
        std::string serviceName = itMap->first;

        if (_strnicmp(serviceName.c_str(), lpServiceName, strlen(lpServiceName)) != 0)
        {
            continue;
        }

        bool startForbidden = itMap->second.startForbidden;
        bool stopForbidden = itMap->second.stopForbidden;
        RCHANDLE handles = { 0, 0, 0, 0 };

        if (startForbidden == true)
        {
            handles.StartHandleA = openHandleA;
        }
        if (stopForbidden == true)
        {
            handles.StopHandleA = openHandleA;
        }
        g_Service2RCHandleMap[serviceName] = handles;
    }
    return openHandleA;
}

SC_HANDLE WINAPI FakeOpenServiceW(
    _In_            SC_HANDLE               hSCManager,
    _In_            LPCWSTR                lpServiceName,
    _In_            DWORD                   dwDesiredAccess
    )
{
    //先加载Json数据
    LoadJsonFile();
    SC_HANDLE openHandleW = g_pfnOpenServiceW(hSCManager, lpServiceName, dwDesiredAccess);
    std::map<std::string, RCFORBIDDENINFO>::iterator itMap = g_JsonDataMap.begin();
    for (; itMap != g_JsonDataMap.end(); ++itMap)
    {
        std::wstring serviceName = Ansi2WChar(itMap->first.c_str(), itMap->first.length());

        if (_wcsnicmp(serviceName.c_str(), lpServiceName, wcslen(lpServiceName)) != 0)
        {
            continue;
        }

        bool startForbidden = itMap->second.startForbidden;
        bool stopForbidden = itMap->second.stopForbidden;
        RCHANDLE handles = { NULL };

        if (startForbidden == true)
        {
            handles.StartHandleW = openHandleW;
        }
        if (stopForbidden == true)
        {
            handles.StopHandleW = openHandleW;
        }
        g_Service2RCHandleMap[itMap->first] = handles;

    }

    return openHandleW;
}

BOOL WINAPI FakeStartServiceA(
    _In_            SC_HANDLE            hService,
    _In_            DWORD                dwNumServiceArgs,
    _In_reads_opt_(dwNumServiceArgs)
    LPCSTR             *lpServiceArgVectors
    )
{
    std::map<std::string, RCHANDLE>::iterator itMap = g_Service2RCHandleMap.begin();
    for (; itMap != g_Service2RCHandleMap.end(); ++itMap)
    {
        if (hService == itMap->second.StartHandleA)
        {
            ::MessageBox(NULL, "CAN'T START", NULL, MB_OK);
            return FALSE;
        }
        if(hService == itMap->second.StartHandleW)
        {
            ::MessageBox(NULL, "CAN'T START", NULL, MB_OK);
            return FALSE;
        }
    }

    return g_pfnStartServiceA(hService, dwNumServiceArgs, lpServiceArgVectors);
}

BOOL WINAPI FakeStartServiceW(
    _In_            SC_HANDLE            hService,
    _In_            DWORD                dwNumServiceArgs,
    _In_reads_opt_(dwNumServiceArgs)
    LPCWSTR             *lpServiceArgVectors
    )
{
    std::map<std::string, RCHANDLE>::iterator itMap = g_Service2RCHandleMap.begin();
    for (; itMap != g_Service2RCHandleMap.end(); ++itMap)
    {
        if (hService == itMap->second.StartHandleA)
        {
            ::MessageBox(NULL, "CAN'T START", NULL, MB_OK);
            return FALSE;
        }
        if (hService == itMap->second.StartHandleW)
        {
            ::MessageBox(NULL, "CAN'T START", NULL, MB_OK);
            return FALSE;
        }
    }
    return g_pfnStartServiceW(hService, dwNumServiceArgs, lpServiceArgVectors);
}

BOOL WINAPI FakeControlService(
    _In_        SC_HANDLE           hService,
    _In_        DWORD               dwControl,
    _Out_       LPSERVICE_STATUS    lpServiceStatus
    )
{
    if (dwControl == SERVICE_CONTROL_STOP)
    {
        std::map<std::string, RCHANDLE>::iterator itMap = g_Service2RCHandleMap.begin();
        for (; itMap != g_Service2RCHandleMap.end(); ++itMap)
        {
            if (hService == itMap->second.StopHandleA)
            {
                ::MessageBox(NULL, "CAN'T STOP", NULL, MB_OK);
                return FALSE;
            }
            if (hService == itMap->second.StopHandleW)
            {
                ::MessageBox(NULL, "CAN'T STOP", NULL, MB_OK);
                return FALSE;
            }
        }
    }
    return g_pfnControlService(hService, dwControl, lpServiceStatus);
}

BOOL WINAPI FakeCloseServiceHandle(
    _In_        SC_HANDLE   hSCObject
    )
{
    std::map<std::string, RCHANDLE>::iterator itMap = g_Service2RCHandleMap.begin();
    for (; itMap != g_Service2RCHandleMap.end(); ++itMap)
    {
        if (itMap->second.StartHandleA == hSCObject)
        {
            itMap->second.StartHandleA = 0;
        }
        if (itMap->second.StartHandleW == hSCObject)
        {
            itMap->second.StartHandleW = 0;
        }
        if (itMap->second.StopHandleA == hSCObject)
        {
            itMap->second.StopHandleA = 0;
        }
        if (itMap->second.StopHandleW == hSCObject)
        {
            itMap->second.StopHandleW = 0;
        }
    }
    return g_pfnCloseServiceHandle(hSCObject);
}