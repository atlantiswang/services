/******************************************************************************
*  ��Ȩ���У�C��2019���Ϻ�����������������޹�˾                                *
*  ��������Ȩ����                                                             *
******************************************************************************
*  ���� : <����>
*  �汾 : <v1>
*****************************************************************************/

#ifndef __RCSvrDrvManager_h_
#define __RCSvrDrvManager_h_

#include <vector>
#include <map>
#include <windows.h>

/**
* @brief ��CListCtrl��չʾ����Ϣ�Ľṹ�壨������ţ�
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
* @brief �Է�����п��Ƶ�����
*/
enum RCControlServer
{
    RC_SERVER_START,
    RC_SERVER_STOP,
    RC_SERVER_RESET
};

/**
* @brief ������ҷ���ʱ������
*/
enum RClServerSearchType
{
    RC_SERVICE,
    RC_NAME,
    RC_DRIVER,
    RC_ALL
};

/**
* @brief ����������������
* ��������/���� ����Ϣ�Ļ�ȡ������״̬���޸ġ�����Ȩ�޵����á��������͵����õ�
*/
class RCSvrDrvManager
{
public:
    RCSvrDrvManager();

    ~RCSvrDrvManager();

    /**
    * @brief ��ȡ���з���/��������Ϣ
    * @param[in] dwServerType ˵��Ҫ��ȡ���Ƿ�������������Ϣ
    * @return �ɹ�����true ʧ�ܷ���false
    */
    bool GetAllServerInfo(DWORD dwServerType);

    /**
    * @brief ��ȡһ������/��������Ϣ
    * @param[in] serviceName Ҫ��ȡ��Ϣ�ķ���/����������
    * @param[out] rcinfo ��ȡ����Ϣͨ���������������
    */
    bool GetServerInfo(std::string serviceName, RCINFO &rcinfo);

    /**
    * @brief �Է���ǰ��״̬���п���
    * @param[in] serviceName ��Ҫ����ķ��������
    * @param[in] controlServer ���Ƶ����� ��ϸ��RCControlServer����
    */
    bool ControlServer(std::string serviceName, RCControlServer controlServer);

    /**
    * @brief �޸ķ��������״̬
    * @param[in] serviceName ��Ҫ����ķ��������
    * @param[in] ��Ҫ���õ�����������
    */
    bool ChangeStartType(std::string serviceName, DWORD dwServiceType);

    /**
    * @brief ���ݲ���ָ�������ͻ�ȡ����ļ���
    * @param[in] searchType ��ѯ������
    * @param[in] searchName ��searchType��������ɸѡʱ��searchName����Ч������ɸѡ������
    * @return ���ط��������ķ���ļ���
    */
    std::vector<RCINFO> GetVecValue(RClServerSearchType searchType, std::string searchName = "");

    /**
    * @brief ��ȡ��Ա���������Ĵ�С
    * @return ���س�Ա���������Ĵ�С
    */
    size_t GetVecSize();

    /**
    * @brief �Բ�����·�����д���
    * @param[in] pFilePath �������Ҫ�����·��
    * @param[out] pOpenPath �����Ĵ������·��
    */
    void DealFilePath(char *pFilePath, char *pOpenPath);

    /**
    * @brief ���ݷ�������һ�������״̬�ڽ����Ͻ��и���
    */
    bool UpdateService(std::string serviceName, RCINFO& rcinfo);

    /**
    * @brief �ڽ����ϸ������з����״̬
    */
    void UpdateAllServices();
private:

    /**
    * @brief �Ƿ��ӳ���������
    */
    bool ChangeDelayedAutoStart(SC_HANDLE service, BOOL delayed);

    /// ��ŷ�����Ϣ������
    std::vector<RCINFO> m_ServiceVec;
};

#endif