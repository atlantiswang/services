/******************************************************************************
*  版权所有（C）2019，上海二三四五网络科有限公司                                *
*  保留所有权利。                                                             *
******************************************************************************
*  作者 : <王浩>
*  版本 : <v1>
*****************************************************************************/

#ifndef __RCSvrDrvManager_h_
#define __RCSvrDrvManager_h_

#include <vector>
#include <map>
#include <windows.h>

/**
* @brief 在CListCtrl上展示的信息的结构体（除了序号）
*/
typedef struct RCInfo
{
    char        serverName[128];
    char        showName[128];
    char        serverType[8];
    char        startType[32];
    char        serverStatus[8];
    char        displayName[2048];
    char        filePath[MAX_PATH];
}RCINFO, *PRCINFO;

/**
* @brief 对服务进行控制的类型
*/
enum RCControlServer
{
    RC_SERVER_START,
    RC_SERVER_STOP,
    RC_SERVER_RESET
};

/**
* @brief 分类查找服务时的类型
*/
enum RClServerSearchType
{
    RC_SERVICE,
    RC_NAME,
    RC_DRIVER,
    RC_ALL
};

/**
* @brief 服务与驱动操作类
* 包括服务/驱动 的信息的获取、启动状态的修改、启动权限的设置、启动类型的设置等
*/
class RCSvrDrvManager
{
public:
    RCSvrDrvManager();

    ~RCSvrDrvManager();

    /**
    * @brief 获取所有服务/驱动的信息
    * @param[in] dwServerType 说明要获取的是服务还是驱动的信息
    * @return 成功返回true 失败返回false
    */
    bool GetAllServerInfo(DWORD dwServerType);

    /**
    * @brief 获取一个服务/驱动的信息
    * @param[in] serviceName 要获取信息的服务/驱动的名称
    * @param[out] rcinfo 获取的信息通过此引用类型输出
    */
    bool GetServerInfo(std::string serviceName, RCINFO &rcinfo);

    /**
    * @brief 对服务当前的状态进行控制
    * @param[in] serviceName 需要处理的服务的名称
    * @param[in] controlServer 控制的类型 详细见RCControlServer类型
    */
    bool ControlServer(std::string serviceName, RCControlServer controlServer);

    /**
    * @brief 修改服务的启动状态
    * @param[in] serviceName 需要处理的服务的名称
    * @param[in] 需要设置的启动的类型
    */
    bool ChangeStartType(std::string serviceName, DWORD dwServiceType);

    /**
    * @brief 根据参数指定的类型获取服务的集合
    * @param[in] searchType 查询的类型
    * @param[in] searchName 当searchType是以名称筛选时，searchName才生效并传入筛选的名称
    * @return 返回符合条件的服务的集合
    */
    std::vector<RCINFO> GetVecValue(RClServerSearchType searchType, std::string searchName = "");

    /**
    * @brief 获取成员变量容量的大小
    * @return 返回成员变量容量的大小
    */
    size_t GetVecSize();

    /**
    * @brief 对不规则路径进行处理
    * @param[in] pFilePath 传入的需要处理的路径
    * @param[out] pOpenPath 传出的处理过的路径
    */
    void DealFilePath(char *pFilePath, char *pOpenPath);

    /**
    * @brief 根据服务名对一个服务的状态在界面上进行更新
    */
    bool UpdateService(std::string serviceName, RCINFO& rcinfo);

    /**
    * @brief 在界面上更新所有服务的状态
    */
    void UpdateAllServices();
private:

    /**
    * @brief 是否延迟启动服务
    */
    bool ChangeDelayedAutoStart(SC_HANDLE service, BOOL delayed);

    /// 存放服务信息的容器
    std::vector<RCINFO> m_ServiceVec;
};

#endif