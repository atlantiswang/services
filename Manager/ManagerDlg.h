
// ManagerDlg.h : header file
//

#pragma once
#include "RCSvrDrvManager.h"
#include "afxcmn.h"
#include "afxwin.h"

#define JSON_FILE_PATH "C:\\content.json"

// CManagerDlg dialog
class CManagerDlg : public CDialogEx
{
    typedef struct RCItemInfo
    {
        int         item;
        bool        forbiddenStart;
        bool        forbiddenStop;
        char        searchName[128];
        char        showName[128];
        char        serverName[128];
        char        serverType[8];
        char        startType[8];
        char        serverStatus[8];
        char        displayName[2048];
        char        filePath[MAX_PATH];
    }RCITEMINFO, *PRCITEMINFO;

    typedef struct RCForbiddenInfo
    {
        char serviceName[128];
        bool startForbidden;
        bool stopForbidden;
    }RCFORBIDDENINFO, *PRCFORBIDDENINFO;
// Construction
public:
    CManagerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
    enum { IDD = IDD_MANAGER_DIALOG };

    protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
    HICON m_hIcon;

    // Generated message map functions
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()
private:
    void InitCtrl();

    void LoadJsonData();

    void WriteJsonData();

    void ShowServices(RClServerSearchType searchType, std::string name = "");
private:
    CListCtrl m_ServiceList;

    RCSvrDrvManager m_SvrDrvManager;

    CComboBox m_Search;

    RCITEMINFO m_CurrentItem;

    std::map<std::string, RCFORBIDDENINFO> m_FobiddenMap;
public:
    afx_msg void OnCbnSelchangeComboSearch();
    afx_msg void OnBnClickedButtonSearch();
    afx_msg void OnNMRClickListService(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnMenuStart();
    afx_msg void OnMenuStop();
    afx_msg void OnMenuRestart();
    afx_msg void OnMenuAuto();
    afx_msg void OnMan();
    afx_msg void OnForbidden();
    afx_msg void OnMenuOpen();
    afx_msg void OnMenuDriverAuto();
    afx_msg void OnMenuDriverMan();
    afx_msg LRESULT OnGetDefID(WPARAM wParam, LPARAM lParam);
    afx_msg void OnMenuForbiddenStart();
    afx_msg void OnMenuForbiddenStop();
    afx_msg void OnBnClickedButtonUpdateall();
};
