/******************************************************************************
*  ��Ȩ���У�C��2019���Ϻ�����������������޹�˾                                *
*  ��������Ȩ����                                                             *
******************************************************************************
*  ���� : <����>
*  �汾 : <v1>
*****************************************************************************/

#ifndef _ProcessOper_
#define _ProcessOper_

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�:
#include <windows.h>

/**
* @brief EasyHookLib�ĵ����ӿ�
*/
EXTERN_C BOOL APIENTRY EasyHookDllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved);

/**
* @brief ��ʼ������ȫ�ֵĺ���ָ�븳ֵ��ֵΪwindows��ԭ��API
*/
BOOL Init();

/**
* @brief ��װ���ӣ������ǵĺ����滻windows api
*/
BOOL InstallHook();

/**
* @brief �ѹ�����
*/
BOOL UnInstallHook();

#endif