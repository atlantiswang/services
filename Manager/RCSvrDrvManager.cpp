#include "RCSvrDrvManager.h"
#include <stdio.h>

RCSvrDrvManager::RCSvrDrvManager()
{
    
}

RCSvrDrvManager::~RCSvrDrvManager()
{

}

bool RCSvrDrvManager::GetAllServerInfo(DWORD dwServerType)
{
    // 需要放入容器的变量
    RCINFO rcinfo = { 0 };
    // 服务状态结构体变量
    LPENUM_SERVICE_STATUS st;
    st = (LPENUM_SERVICE_STATUS)LocalAlloc(LPTR, 64 * 1024);
    DWORD ret = 0;
    DWORD size = 0;
    SC_HANDLE sc = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    // 枚举所有dwServerType的服务
    EnumServicesStatus(sc, dwServerType, SERVICE_STATE_ALL, (LPENUM_SERVICE_STATUS)st, 1024 * 64, &size, &ret, NULL);
    LPQUERY_SERVICE_CONFIG lsc;
    for (DWORD i = 0; i < ret; i++)
    {
        memset(&rcinfo, 0, sizeof(RCINFO));
        SC_HANDLE ServiceConfige;
        DWORD   nResumeHandle = 0;
        ServiceConfige = OpenService((SC_HANDLE)sc, st[i].lpServiceName, SERVICE_QUERY_CONFIG);
        if (!ServiceConfige)
        {
            printf("OpenService error!/n");
            continue;
        }
        lsc = (LPQUERY_SERVICE_CONFIG)LocalAlloc(LPTR, 8 * 1024);
        QueryServiceConfig(ServiceConfige, lsc, 8 * 1024, &nResumeHandle);
        
        if (lsc->dwStartType == SERVICE_AUTO_START)
        {
            strcpy_s(rcinfo.startType, 32, "Auto Start");
        }
        else if (lsc->dwStartType == SERVICE_DEMAND_START)
        {
            strcpy_s(rcinfo.startType, 32, "Demand Start");
        }
        else if (lsc->dwStartType == SERVICE_DISABLED)
        {
            strcpy_s(rcinfo.startType, 32, "Disabled");
        }
        else if (lsc->dwStartType == SERVICE_BOOT_START)
        {
            strcpy_s(rcinfo.startType, 32, "Boot Start");
        }
        else if (lsc->dwStartType == SERVICE_SYSTEM_START)
        {
            strcpy_s(rcinfo.startType, 32, "System Start");
        }

        if (st[i].ServiceStatus.dwCurrentState != SERVICE_STOPPED)
        {
            strcpy_s(rcinfo.serverStatus, 8, "已启动");
        }
        else
        {
            strcpy_s(rcinfo.serverStatus, 8, "已停止");
        }

        strcpy_s(rcinfo.serverName, 128, st[i].lpServiceName);
        if (dwServerType == SERVICE_WIN32)
        {
            strcpy_s(rcinfo.serverType, 8, "服务");
        }
        if (dwServerType == SERVICE_DRIVER)
        {
            strcpy_s(rcinfo.serverType, 8, "驱动");
        }
        strcpy_s(rcinfo.showName, 128, lsc->lpDisplayName);
        strcpy_s(rcinfo.filePath, MAX_PATH, lsc->lpBinaryPathName);

        DWORD dwNeeded;
        LPSERVICE_DESCRIPTION lpqscBuf2;
        if (QueryServiceConfig2(ServiceConfige, SERVICE_CONFIG_DESCRIPTION, NULL, 0,
            &dwNeeded) == FALSE && GetLastError() == ERROR_INSUFFICIENT_BUFFER)
        {
            lpqscBuf2 = (LPSERVICE_DESCRIPTION)LocalAlloc(LPTR, dwNeeded);
            if (QueryServiceConfig2(ServiceConfige, SERVICE_CONFIG_DESCRIPTION,
                (BYTE*)lpqscBuf2, dwNeeded, &dwNeeded))
            {
                printf(" Description: %s\n", lpqscBuf2->lpDescription);
                if (lpqscBuf2->lpDescription != NULL ||
                    dwServerType != SERVICE_DRIVER)
                {
                    strncpy_s(rcinfo.displayName, 2048, lpqscBuf2->lpDescription, 2040);
                }
            }
            LocalFree(lpqscBuf2);
        }

        m_ServiceVec.push_back(rcinfo);
        
        LocalFree(lsc);
        CloseServiceHandle(ServiceConfige);
//        Sleep(20);
    }
    CloseServiceHandle(sc);
    return true;
}

bool RCSvrDrvManager::GetServerInfo(std::string serviceName, RCINFO& rcinfo)
{
    DWORD ret = 0;
    DWORD size = 0;
    SC_HANDLE sc = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    LPQUERY_SERVICE_CONFIG lsc;//服务配置结构体变量

    memset(&rcinfo, 0, sizeof(RCINFO));
    SC_HANDLE ServiceConfige;
    DWORD   nResumeHandle = 0;
    ServiceConfige = OpenService((SC_HANDLE)sc, serviceName.c_str(), SERVICE_ALL_ACCESS | DELETE);
    if (!ServiceConfige)
    {
        printf("OpenService error!/n");
        return false;
    }

    SERVICE_STATUS status;
    
    if (::QueryServiceStatus(ServiceConfige, &status) == FALSE)
    {
        ::CloseServiceHandle(ServiceConfige);
        ::CloseServiceHandle(sc);
        return false;
    }

    lsc = (LPQUERY_SERVICE_CONFIG)LocalAlloc(LPTR, 8 * 1024); //注意分配足够的空间
    QueryServiceConfig(ServiceConfige, lsc, 8 * 1024, &nResumeHandle); //枚举各个服务信息

    if (lsc->dwStartType == SERVICE_AUTO_START)
    {
        strcpy_s(rcinfo.startType, 32, "Auto Start");
    }
    else if (lsc->dwStartType == SERVICE_DEMAND_START)
    {
        strcpy_s(rcinfo.startType, 32, "Demand Start");
    }
    else if (lsc->dwStartType == SERVICE_DISABLED)
    {
        strcpy_s(rcinfo.startType, 32, "Disabled");
    }
    else if (lsc->dwStartType == SERVICE_BOOT_START)
    {
        strcpy_s(rcinfo.startType, 32, "Boot Start");
    }
    else if (lsc->dwStartType == SERVICE_SYSTEM_START)
    {
        strcpy_s(rcinfo.startType, 32, "System Start");
    }
    
    if (status.dwCurrentState != SERVICE_STOPPED)
    {
        strcpy_s(rcinfo.serverStatus, 8, "已启动");
    }
    else
    {
        strcpy_s(rcinfo.serverStatus, 8, "已停止");
    }

    strcpy_s(rcinfo.serverName, 128, serviceName.c_str());

    if (lsc->dwServiceType == SERVICE_FILE_SYSTEM_DRIVER ||
        lsc->dwServiceType == SERVICE_KERNEL_DRIVER)
    {
        strcpy_s(rcinfo.serverType, 8, "驱动");
    }
    else if (lsc->dwServiceType == SERVICE_WIN32_OWN_PROCESS ||
        lsc->dwServiceType == SERVICE_WIN32_SHARE_PROCESS)
    {
        strcpy_s(rcinfo.serverType, 8, "服务");
    }
    
    strcpy_s(rcinfo.showName, 128, lsc->lpDisplayName);
    strcpy_s(rcinfo.filePath, MAX_PATH, lsc->lpBinaryPathName);

    DWORD dwNeeded;
    LPSERVICE_DESCRIPTION lpqscBuf2;
    if (QueryServiceConfig2(ServiceConfige, SERVICE_CONFIG_DESCRIPTION, NULL, 0,
        &dwNeeded) == FALSE && GetLastError() == ERROR_INSUFFICIENT_BUFFER)
    {
        lpqscBuf2 = (LPSERVICE_DESCRIPTION)LocalAlloc(LPTR, dwNeeded);
        if (QueryServiceConfig2(ServiceConfige, SERVICE_CONFIG_DESCRIPTION,
            (BYTE*)lpqscBuf2, dwNeeded, &dwNeeded))
        {
            printf(" Description: %s\n", lpqscBuf2->lpDescription);
            if (lpqscBuf2->lpDescription != NULL)
            {
                strcpy_s(rcinfo.displayName, 2048, lpqscBuf2->lpDescription);
            }
        }
        LocalFree(lpqscBuf2);
    }

    LocalFree(lsc);
    CloseServiceHandle(ServiceConfige);
//    Sleep(20);
    
    CloseServiceHandle(sc);
    return true;
}

bool RCSvrDrvManager::ControlServer(std::string serviceName, RCControlServer controlServer)
{
    SC_HANDLE hSC = ::OpenSCManager(NULL,
        NULL, GENERIC_EXECUTE);
    if (hSC == NULL)
    {
        return false;
    }
    SC_HANDLE hSvc = ::OpenService(hSC, 
        serviceName.c_str(),
        SERVICE_ALL_ACCESS);
    if (hSvc == NULL)
    {
        printf("%d", GetLastError());
        ::CloseServiceHandle(hSC);
        return false;
    }

    SERVICE_STATUS status;
    if (controlServer == RC_SERVER_START)
    {
        // 开启服务
        if (::StartService(hSvc, NULL, NULL) == FALSE)
        {

            ::CloseServiceHandle(hSvc);
            ::CloseServiceHandle(hSC);
            return false;
        }
        while (::QueryServiceStatus(hSvc, &status) == TRUE)
        {
            ::Sleep(status.dwWaitHint);
            if (status.dwCurrentState == SERVICE_RUNNING)
            {

                ::CloseServiceHandle(hSvc);
                ::CloseServiceHandle(hSC);
                return true;
            }
        }
    }
    else if (controlServer == RC_SERVER_STOP)
    {
        // 停止服务
        if (::ControlService(hSvc,
            SERVICE_CONTROL_STOP, &status) == FALSE)
        {
            ::CloseServiceHandle(hSvc);
            ::CloseServiceHandle(hSC);
            return false;
        }
        // 等待服务停止
        while (::QueryServiceStatus(hSvc, &status) == TRUE)
        {
            ::Sleep(status.dwWaitHint);
            if (status.dwCurrentState == SERVICE_STOPPED)
            {

                ::CloseServiceHandle(hSvc);
                ::CloseServiceHandle(hSC);
                return true;
            }
        }
    }
    else if (controlServer == RC_SERVER_RESET)
    {
        // 重启服务，设置成停止再开启
        ControlServer(serviceName, RC_SERVER_STOP);
        ControlServer(serviceName, RC_SERVER_START);
        return true;
    }
    return true;
}

bool RCSvrDrvManager::ChangeStartType(std::string serviceName, DWORD dwServiceType)
{
    SC_HANDLE schSCManager;
    SC_HANDLE schService;

    schSCManager = OpenSCManager(
        NULL,
        NULL,
        SC_MANAGER_ALL_ACCESS);

    if (NULL == schSCManager)
    {
        return FALSE;
    }

    schService = OpenService(
        schSCManager,
        serviceName.c_str(),
        SERVICE_CHANGE_CONFIG | SERVICE_START);

    if (schService == NULL)
    {
        CloseServiceHandle(schSCManager);
        return FALSE;
    }
    if (dwServiceType == SERVICE_BOOT_START)
    {
        BOOL b = ChangeServiceConfig(schService, SERVICE_NO_CHANGE, SERVICE_BOOT_START, SERVICE_NO_CHANGE, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        if (!b)
            printf("ChangeServiceConfig failed (%d)/n", GetLastError());
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
    }
    else if (dwServiceType == SERVICE_SYSTEM_START)
    {
        BOOL b = ChangeServiceConfig(schService, SERVICE_NO_CHANGE, SERVICE_SYSTEM_START, SERVICE_NO_CHANGE, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        if (!b)
            printf("ChangeServiceConfig failed (%d)/n", GetLastError());
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
    }
    else if (dwServiceType == SERVICE_AUTO_START)
    {
        BOOL b = ChangeServiceConfig(schService, SERVICE_NO_CHANGE, SERVICE_AUTO_START, SERVICE_NO_CHANGE, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        if (!b)
            printf("ChangeServiceConfig failed (%d)/n", GetLastError());
        ChangeDelayedAutoStart(schService, FALSE);
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
    }
    else if (dwServiceType == SERVICE_DEMAND_START)
    {
        BOOL b = ChangeServiceConfig(schService, SERVICE_NO_CHANGE, SERVICE_DEMAND_START, SERVICE_NO_CHANGE, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        if (!b)
            printf("ChangeServiceConfig failed (%d)/n", GetLastError());
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
    }
    else if (dwServiceType == SERVICE_DISABLED)
    {
        BOOL bReturn = ChangeServiceConfig(schService, SERVICE_NO_CHANGE, SERVICE_DISABLED, SERVICE_NO_CHANGE, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        if (!bReturn)
            printf("ChangeServiceConfig failed (%d)/n", GetLastError());
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
    }
    if (schSCManager != NULL)
        CloseServiceHandle(schSCManager);
    if (schService != NULL)
        CloseServiceHandle(schService);
    return true;
}


std::vector<RCINFO> RCSvrDrvManager::GetVecValue(RClServerSearchType searchType, std::string searchName)
{
    std::vector<RCINFO> tempVec;
    if (searchType == RC_SERVICE)
    {
        for (std::vector<RCINFO>::iterator it = m_ServiceVec.begin(); it != m_ServiceVec.end(); ++it)
        {
            if (strcmp(it->serverType, "服务") == 0)
            {
                tempVec.push_back(*it);
            }
        }
    }
    else if (searchType == RC_NAME)
    {
        for (std::vector<RCINFO>::iterator it = m_ServiceVec.begin(); it != m_ServiceVec.end(); ++it)
        {
            if (searchName.compare(it->serverName) == 0 ||
                _stricmp(searchName.c_str(), it->serverName) == 0)
            {
                tempVec.push_back(*it);
                return tempVec;
            }
        }
    }
    else if (searchType == RC_DRIVER)
    {
        for (std::vector<RCINFO>::iterator it = m_ServiceVec.begin(); it != m_ServiceVec.end(); ++it)
        {
            if (strcmp(it->serverType, "驱动") == 0)
            {
                tempVec.push_back(*it);
            }
        }
    }
    else if (searchType == RC_ALL)
    {
        tempVec = m_ServiceVec;
    }

    return tempVec;
}

size_t RCSvrDrvManager::GetVecSize()
{
    return m_ServiceVec.size();
}

void RCSvrDrvManager::DealFilePath(char *pFilePath, char *pOutPath)
{
    if (strlen(pFilePath) == 0)
    {
        return ;
    }

    strcpy_s(pOutPath, MAX_PATH, pFilePath);

    if (_strnicmp(pOutPath, "\\systemroot\\", 12) == 0)
    {
        char systemDir[128] = {0};
        GetSystemDirectory(systemDir, 128);
        *strrchr(systemDir, '\\') = 0;
        sprintf_s(pOutPath, MAX_PATH,"%s\\%s", systemDir, pFilePath + 12);
    }

    if (_strnicmp(pOutPath, "system32", 8) == 0)
    {
        char systemDir[128] = { 0 };
        GetSystemDirectory(systemDir, 128);
        sprintf_s(pOutPath, MAX_PATH, "%s\\%s", systemDir, pFilePath + 12);
    }

    //先去掉双引号
    if (pOutPath[0] == '\"' && pOutPath[strlen(pOutPath) - 1] == '\"')
    {
        std::string temp = pOutPath + 1;
        temp.pop_back();
        strcpy_s(pOutPath, MAX_PATH, temp.c_str());
    }

    char *pSpace = strstr(pOutPath, " -");
    if (pSpace != NULL)
    {
        *pSpace = 0;
    }
    pSpace = strstr(pOutPath, " /");
    if (pSpace != NULL)
    {
        *pSpace = 0;
    }
}

bool RCSvrDrvManager::UpdateService(std::string serviceName, RCINFO& rcinfo)
{
    std::vector<RCINFO>::iterator it = m_ServiceVec.begin();
    for (; it != m_ServiceVec.end(); ++it)
    {
        if (serviceName.compare(it->serverName) == 0)
        {
            if(GetServerInfo(serviceName.c_str(), rcinfo) == true)
            {
                memcpy_s(&*it, sizeof(RCINFO), &rcinfo, sizeof(RCINFO));
                return true;
                break;
            }
        }
    }
    return false;
}

void RCSvrDrvManager::UpdateAllServices()
{
    m_ServiceVec.clear();
    GetAllServerInfo(SERVICE_WIN32);
    GetAllServerInfo(SERVICE_DRIVER);
}

bool RCSvrDrvManager::ChangeDelayedAutoStart(
    SC_HANDLE service, BOOL delayed)
{
    SERVICE_DELAYED_AUTO_START_INFO info = { delayed };

    return 0 != ::ChangeServiceConfig2(
        service,
        SERVICE_CONFIG_DELAYED_AUTO_START_INFO,
        &info);
}
