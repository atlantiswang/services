/******************************************************************************
*  ��Ȩ���У�C��2019���Ϻ�����������������޹�˾                                *
*  ��������Ȩ����                                                             *
******************************************************************************
*  ���� : <����>
*  �汾 : <v1>
*****************************************************************************/

#ifndef __FakeControl_h_
#define __FakeControl_h_

#include <windows.h>
#include <map>

/**
* @brief ��Ӳ��json�ļ���������Ϣ�ŵ��˽ṹ����
*/
typedef struct RCForbiddenInfo
{
    char serviceName[128]; /// ��������
    bool startForbidden; /// �Ƿ��ֹ����
    bool stopForbidden; /// �Ƿ��ֹֹͣ
}RCFORBIDDENINFO, *PRCFORBIDDENINFO;

/**
* @brief �������򿪵ľ�������ݾ�����������Ƿ�ʹ��XXA/XXW�����򿪷ֳ���4��
* @note ����OpenService����SC_HANDLE�󲢲�֪����HANDLE�������򿪻���ֹͣһ�������á�
* ֻ��ʹ�ö��SC_HANDLE���������档����Ҳ��֪��Ӧ�ó�������OpenServiceA����OpenServiceW
* ���򿪵ķ���ֻ���ö��SC_HANDLE����������
*/
typedef struct RCHandle
{
    SC_HANDLE StartHandleA;
    SC_HANDLE StopHandleA;
    SC_HANDLE StartHandleW;
    SC_HANDLE StopHandleW;
}RCHANDLE, *PRCHANDLE;

/**
* windows api OpenServiceA�ĺ�������
*/
typedef SC_HANDLE(WINAPI *TRCOpenServiceA)(
    _In_            SC_HANDLE               hSCManager,
    _In_            LPCSTR                lpServiceName,
    _In_            DWORD                   dwDesiredAccess
    );

/**
* windows api OpenServiceW�ĺ�������
*/
typedef SC_HANDLE(WINAPI *TRCOpenServiceW)(
    _In_            SC_HANDLE               hSCManager,
    _In_            LPCWSTR                lpServiceName,
    _In_            DWORD                   dwDesiredAccess
    );

/**
* windows api StartServiceA�ĺ�������
*/
typedef BOOL(WINAPI *TRCStartServiceA)(
    _In_            SC_HANDLE            hService,
    _In_            DWORD                dwNumServiceArgs,
    _In_reads_opt_(dwNumServiceArgs)
    LPCSTR             *lpServiceArgVectors
    );

/**
* windows api StartServiceW�ĺ�������
*/
typedef BOOL(WINAPI *TRCStartServiceW)(
    _In_            SC_HANDLE            hService,
    _In_            DWORD                dwNumServiceArgs,
    _In_reads_opt_(dwNumServiceArgs)
    LPCWSTR             *lpServiceArgVectors
    );

/**
* windows api ControlService�ĺ�������
*/
typedef BOOL(WINAPI *TRCControlService)(
    _In_        SC_HANDLE           hService,
    _In_        DWORD               dwControl,
    _Out_       LPSERVICE_STATUS    lpServiceStatus
    );

/**
* windows api CloseServiceHandle�ĺ�������
*/
typedef BOOL(WINAPI *TRCCloseServiceHandle)(
_In_        SC_HANDLE   hSCObject
);

/**
* @brief Hook OpenServiceA�Ĵ�����
* @param phSCManager SM���
* @param lpServiceName Ҫ�򿪵ķ��������
* @param dwDesiredAccess ʹ�õ�Ȩ��
* @param return ���ؾ������StartService�Ⱥ�����ʹ��
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
* @brief ����һ�������API(hook)
* @param hService OpenService���صľ��
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
* @brief ֹͣһ������ʱʹ�õ�API(hook)
* @param hService OpenService���صľ��
* @param dwControl������������ʾ���������ͣ�����ֻʹ��ֹͣ�Ŀ����룩
*/
BOOL WINAPI FakeControlService(
_In_        SC_HANDLE           hService,
_In_        DWORD               dwControl,
_Out_       LPSERVICE_STATUS    lpServiceStatus
);

/**
* @brief �رշ�������API(hook)
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
