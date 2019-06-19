/******************************************************************************
*  版权所有（C）2019，上海二三四五网络科有限公司                                *
*  保留所有权利。                                                             *
******************************************************************************
*  作者 : <王浩>
*  版本 : <v1>
*****************************************************************************/

#ifndef _ProcessOper_
#define _ProcessOper_

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件:
#include <windows.h>

/**
* @brief EasyHookLib的导出接口
*/
EXTERN_C BOOL APIENTRY EasyHookDllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved);

/**
* @brief 初始化，给全局的函数指针赋值，值为windows的原生API
*/
BOOL Init();

/**
* @brief 安装勾子，用我们的函数替换windows api
*/
BOOL InstallHook();

/**
* @brief 脱勾函数
*/
BOOL UnInstallHook();

#endif