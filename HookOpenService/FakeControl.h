/******************************************************************************
*  版权所有（C）2019，上海二三四五网络科有限公司                                *
*  保留所有权利。                                                             *
******************************************************************************
*  作者 : <王浩>
*  版本 : <v1>
*****************************************************************************/

#ifndef __FakeControl_h_
#define __FakeControl_h_

#include <windows.h>
#include <map>

/**
* @brief 从硬盘json文件里读入的信息放到此结构体里
*/
typedef struct RCForbiddenInfo
{
    char serviceName[128]; /// 服务名称
    bool startForbidden; /// 是否禁止开启
    bool stopForbidden; /// 是否禁止停止
}RCFORBIDDENINFO, *PRCFORBIDDENINFO;

/**
* @brief 保存服务打开的句柄，根据句柄的作用与是否使用XXA/XXW函数打开分成了4类
* @note 由于OpenService返回SC_HANDLE后并不知道此HANDLE是用做打开还是停止一个服务用。
* 只能使用多个SC_HANDLE变量来保存。另外也不知道应用程序是用OpenServiceA还是OpenServiceW
* 来打开的服务。只能用多个SC_HANDLE来保存他们
*/
typedef struct RCHandle
{
    SC_HANDLE StartHandleA;
    SC_HANDLE StopHandleA;
    SC_HANDLE StartHandleW;
    SC_HANDLE StopHandleW;
}RCHANDLE, *PRCHANDLE;

/**
* windows api OpenServiceA的函数类型
*/
typedef SC_HANDLE(WINAPI *TRCOpenServiceA)(
    _In_            SC_HANDLE               hSCManager,
    _In_            LPCSTR                lpServiceName,
    _In_            DWORD                   dwDesiredAccess
    );

/**
* windows api OpenServiceW的函数类型
*/
typedef SC_HANDLE(WINAPI *TRCOpenServiceW)(
    _In_            SC_HANDLE               hSCManager,
    _In_            LPCWSTR                lpServiceName,
    _In_            DWORD                   dwDesiredAccess
    );

/**
* windows api StartServiceA的函数类型
*/
typedef BOOL(WINAPI *TRCStartServiceA)(
    _In_            SC_HANDLE            hService,
    _In_            DWORD                dwNumServiceArgs,
    _In_reads_opt_(dwNumServiceArgs)
    LPCSTR             *lpServiceArgVectors
    );

/**
* windows api StartServiceW的函数类型
*/
typedef BOOL(WINAPI *TRCStartServiceW)(
    _In_            SC_HANDLE            hService,
    _In_            DWORD                dwNumServiceArgs,
    _In_reads_opt_(dwNumServiceArgs)
    LPCWSTR             *lpServiceArgVectors
    );

/**
* windows api ControlService的函数类型
*/
typedef BOOL(WINAPI *TRCControlService)(
    _In_        SC_HANDLE           hService,
    _In_        DWORD               dwControl,
    _Out_       LPSERVICE_STATUS    lpServiceStatus
    );

/**
* windows api CloseServiceHandle的函数类型
*/
typedef BOOL(WINAPI *TRCCloseServiceHandle)(
_In_        SC_HANDLE   hSCObject
);

/**
* @brief Hook OpenServiceA的处理函数
* @param phSCManager SM句柄
* @param lpServiceName 要打开的服务的名称
* @param dwDesiredAccess 使用的权限
* @param return 返回句柄。在StartService等函数中使用
*/
SC_HANDLE WINAPI FakeOpenServiceA(
    _In_            SC_HANDLE               hSCManager,
    _In_            LPCSTR                lpServiceName,
    _In_            DWORD                   dwDesiredAccess
    );
/*
* @brief Hook windowsApi(OpenServiceW)
*/
SC_HANDLE WINAPI FakeOpenServiceW(
_In_            SC_HANDLE               hSCManager,
_In_            LPCWSTR                lpServiceName,
_In_            DWORD                   dwDesiredAccess
);

/**
* @brief 启动一个服务的API(hook)
* @param hService OpenService返回的句柄
*/
BOOL WINAPI FakeStartServiceA(
_In_            SC_HANDLE            hService,
_In_            DWORD                dwNumServiceArgs,
_In_reads_opt_(dwNumServiceArgs)
LPCSTR             *lpServiceArgVectors
);

/**
* @brief Hook windowsApi(StartServiceW)
*/
BOOL WINAPI FakeStartServiceW(
_In_            SC_HANDLE            hService,
_In_            DWORD                dwNumServiceArgs,
_In_reads_opt_(dwNumServiceArgs)
LPCWSTR             *lpServiceArgVectors
);

/**
* @brief 停止一个服务时使用的API(hook)
* @param hService OpenService返回的句柄
* @param dwControl控制码用来表示操作的类型（这里只使用停止的控制码）
*/
BOOL WINAPI FakeControlService(
_In_        SC_HANDLE           hService,
_In_        DWORD               dwControl,
_Out_       LPSERVICE_STATUS    lpServiceStatus
);

/**
* @brief 关闭服务句柄的API(hook)
*/
BOOL WINAPI FakeCloseServiceHandle(
    _In_        SC_HANDLE   hSCObject
    );

extern TRCOpenServiceA g_pfnOpenServiceA;
extern TRCOpenServiceW g_pfnOpenServiceW;
extern TRCStartServiceA g_pfnStartServiceA;
extern TRCStartServiceW g_pfnStartServiceW;
extern TRCControlService g_pfnControlService;
extern TRCCloseServiceHandle g_pfnCloseServiceHandle;

extern ULONG hookOpenServiceA_ACLEntries[1];
extern ULONG hookOpenServiceW_ACLEntries[1];
extern ULONG hookStartServiceA_ACLEntries[1];
extern ULONG hookStartServiceW_ACLEntries[1];
extern ULONG hookControlService_ACLEntries[1];
extern ULONG hookCloseServiceHandle_ACLEntries[1];
#endif
