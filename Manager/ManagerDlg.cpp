
// ManagerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "winsvc.h"
#include "Manager.h"
#include "ManagerDlg.h"
#include "afxdialogex.h"
#include "json.h"
#include <fstream>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CManagerDlg dialog



CManagerDlg::CManagerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CManagerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CManagerDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_SERVICE, m_ServiceList);
    DDX_Control(pDX, IDC_COMBO_SEARCH, m_Search);
}

BEGIN_MESSAGE_MAP(CManagerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_CBN_SELCHANGE(IDC_COMBO_SEARCH, &CManagerDlg::OnCbnSelchangeComboSearch)
    ON_BN_CLICKED(IDC_BUTTON_SEARCH, &CManagerDlg::OnBnClickedButtonSearch)
    ON_NOTIFY(NM_RCLICK, IDC_LIST_SERVICE, &CManagerDlg::OnNMRClickListService)
    ON_COMMAND(ID_MENU_START, &CManagerDlg::OnMenuStart)
    ON_COMMAND(ID_MENU_STOP, &CManagerDlg::OnMenuStop)
    ON_COMMAND(ID_MENU_RESTART, &CManagerDlg::OnMenuRestart)
    ON_COMMAND(ID_MENU_AUTO, &CManagerDlg::OnMenuAuto)
    ON_COMMAND(ID_MAN, &CManagerDlg::OnMan)
    ON_COMMAND(ID_FORBIDDEN, &CManagerDlg::OnForbidden)
    ON_COMMAND(ID_MENU_OPEN, &CManagerDlg::OnMenuOpen)
    ON_COMMAND(ID_MENU_DRIVER_AUTO, &CManagerDlg::OnMenuDriverAuto)
    ON_COMMAND(ID_MENU_DRIVER_MAN, &CManagerDlg::OnMenuDriverMan)
    ON_MESSAGE(DM_GETDEFID, &OnGetDefID)
    ON_COMMAND(ID_MENU_FORBIDDEN_START, &CManagerDlg::OnMenuForbiddenStart)
    ON_COMMAND(ID_MENU_FORBIDDEN_STOP, &CManagerDlg::OnMenuForbiddenStop)
    ON_BN_CLICKED(IDC_BUTTON_UPDATEALL, &CManagerDlg::OnBnClickedButtonUpdateall)
END_MESSAGE_MAP()

// CManagerDlg message handlers

BOOL CManagerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
    
    InitCtrl();

    LoadJsonData();

    m_SvrDrvManager.GetAllServerInfo(SERVICE_WIN32);
    m_SvrDrvManager.GetAllServerInfo(SERVICE_DRIVER);

    ShowServices(RC_ALL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CManagerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CManagerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CManagerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CManagerDlg::InitCtrl()
{
    //combo控件 0服务 1名称 2驱动 3全部
    m_Search.AddString("服务");
    m_Search.AddString("名称");
    m_Search.AddString("驱动");
    m_Search.AddString("全部");
    m_Search.SetCurSel(3);
    GetDlgItem(IDC_BUTTON_SEARCH)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_EDIT_SERVICE_NAME)->ShowWindow(SW_HIDE);
    //listctrl控件
    m_ServiceList.InsertColumn(0, "序号", LVCFMT_LEFT, 50);
    m_ServiceList.InsertColumn(1, "名称", LVCFMT_LEFT, 150);
    m_ServiceList.InsertColumn(2, "显示名称", LVCFMT_LEFT, 150);
    m_ServiceList.InsertColumn(3, "类型", LVCFMT_LEFT, 40);
    m_ServiceList.InsertColumn(4, "状态", LVCFMT_LEFT, 60);
    m_ServiceList.InsertColumn(5, "启动类型", LVCFMT_LEFT, 100);
    m_ServiceList.InsertColumn(6, "描述信息", LVCFMT_LEFT, 200);
    m_ServiceList.InsertColumn(7, "文件路径", LVCFMT_LEFT, 500);
    m_ServiceList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);
}

void CManagerDlg::LoadJsonData()
{
    std::ifstream ifs;
    ifs.open(JSON_FILE_PATH);
    Json::Reader jsonReader;
    Json::Value root;
    if (!jsonReader.parse(ifs, root, false))
        return;

    int arraySize = root.size();
    RCForbiddenInfo forbiddenInfo = { 0 };

    for (int i = 0; i < arraySize; i++)
    {
        memset(&forbiddenInfo, 0, sizeof(forbiddenInfo));
        Json::Value valueRoot = root[i];
        
        strcpy_s(forbiddenInfo.serviceName, 128, valueRoot["serviceName"].asCString());
        forbiddenInfo.startForbidden = valueRoot["bForbiddenStart"].asBool();
        forbiddenInfo.stopForbidden = valueRoot["bForbiddenStop"].asBool();
        m_FobiddenMap.insert(std::make_pair(forbiddenInfo.serviceName, forbiddenInfo));
    }
    ifs.close();
}

void CManagerDlg::WriteJsonData()
{
    std::ofstream ofs;
    ofs.open(JSON_FILE_PATH);
    Json::StyledWriter jsonWriter;
    Json::Value root;

    std::map<std::string, RCForbiddenInfo>::iterator itMap = m_FobiddenMap.begin();
    for (int i = 0; itMap != m_FobiddenMap.end(); ++itMap)
    {
        root[i]["serviceName"] = itMap->second.serviceName;
        root[i]["bForbiddenStart"] = itMap->second.startForbidden;
        root[i]["bForbiddenStop"] = itMap->second.stopForbidden;
        i++;
    }

    ofs << jsonWriter.write(root);
    ofs.close();
}

void CManagerDlg::ShowServices(RClServerSearchType searchType, std::string name)
{
    //传入一个key 。输出一个value
    m_ServiceList.DeleteAllItems();
    std::vector<RCINFO> vecTemp;
    vecTemp = m_SvrDrvManager.GetVecValue(searchType, name);
    size_t vecSize = vecTemp.size();
    
    char buff[8] = { 0 };
    for (int i = 0; i < (int)vecSize; i++)
    {
        _itoa_s(i, buff, 10);
        m_ServiceList.InsertItem(i, buff);
        m_ServiceList.SetItemText(i, 1, vecTemp[i].serverName);
        m_ServiceList.SetItemText(i, 2, vecTemp[i].showName);
        m_ServiceList.SetItemText(i, 3, vecTemp[i].serverType);
        m_ServiceList.SetItemText(i, 4, vecTemp[i].serverStatus);
        m_ServiceList.SetItemText(i, 5, vecTemp[i].startType);
        m_ServiceList.SetItemText(i, 6, vecTemp[i].displayName);
        m_ServiceList.SetItemText(i, 7, vecTemp[i].filePath);
    }
}

void CManagerDlg::OnCbnSelchangeComboSearch()
{
    
    RClServerSearchType sel = (RClServerSearchType)m_Search.GetCurSel();
    if (sel == RC_NAME)
    {
        GetDlgItem(IDC_BUTTON_SEARCH)->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_EDIT_SERVICE_NAME)->ShowWindow(SW_SHOW);
        //m_ServiceList.DeleteAllItems();
        return;
    }
    else
    {
        GetDlgItem(IDC_BUTTON_SEARCH)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_EDIT_SERVICE_NAME)->ShowWindow(SW_HIDE);
    }
    ShowServices(sel);
}


void CManagerDlg::OnBnClickedButtonSearch()
{
    CString buff;
    GetDlgItemText(IDC_EDIT_SERVICE_NAME, buff);
    ShowServices(RC_NAME, buff.GetBuffer(0));
    strcpy_s(m_CurrentItem.searchName, 128, buff.GetBuffer(0));
}


void CManagerDlg::OnNMRClickListService(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: Add your control notification handler code here
    *pResult = 0;

    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
    if (pNMListView->iItem != -1)
    {
        //下面的两行代码主要是为了后面的操作为准备的
        //获取列表视图控件中第一个被选择项的位置  
        POSITION m_pstion = m_ServiceList.GetFirstSelectedItemPosition();
        //该函数获取由pos指定的列表项的索引，然后将pos设置为下一个位置的POSITION值
        int index = m_ServiceList.GetNextSelectedItem(m_pstion);

        m_CurrentItem.item = index;

        CString temp = m_ServiceList.GetItemText(index, 1);
        strcpy_s(m_CurrentItem.serverName, 128, temp.GetBuffer(0));
        temp = m_ServiceList.GetItemText(index, 7);
        strcpy_s(m_CurrentItem.filePath, MAX_PATH, temp.GetBuffer(0));
        temp = m_ServiceList.GetItemText(index, 3);
        strcpy_s(m_CurrentItem.serverType, 8, temp.GetBuffer(0));
        temp = m_ServiceList.GetItemText(index, 4);
        strcpy_s(m_CurrentItem.serverStatus, 8, temp.GetBuffer(0));

        DWORD dwPos = GetMessagePos();
        CPoint point(LOWORD(dwPos), HIWORD(dwPos));
        CMenu menu;
        //添加线程操作
        VERIFY(menu.LoadMenu(IDR_MENU_SERVICE));			//这里是我们在1中定义的MENU的文件名称
        CMenu* popup = menu.GetSubMenu(0);
        ASSERT(popup != NULL);
        
        if (strcmp(m_CurrentItem.serverType, "服务") == 0)
        {
            popup->EnableMenuItem(ID_MENU_DRIVER_MAN, MF_DISABLED);
            popup->EnableMenuItem(ID_MENU_DRIVER_AUTO, MF_DISABLED);

            if (strcmp(m_CurrentItem.serverStatus, "已启动") == 0)
            {
                popup->EnableMenuItem(ID_MENU_START, MF_DISABLED);
            }
            
            if (strcmp(m_CurrentItem.serverStatus, "已停止") == 0)
            {
                popup->EnableMenuItem(ID_MENU_STOP, MF_DISABLED);
                popup->EnableMenuItem(ID_MENU_RESTART, MF_DISABLED);
            }

            bool bForbiddenStartState = false;
            bool bForbiddenStopState = false;
            std::map<std::string, RCFORBIDDENINFO>::iterator itMap =
                m_FobiddenMap.find(m_CurrentItem.serverName);
            if (itMap != m_FobiddenMap.end())
            {
                bForbiddenStartState = itMap->second.startForbidden;
                bForbiddenStopState = itMap->second.stopForbidden;
            }
            else
            {
                bForbiddenStartState = false;
                bForbiddenStopState = false;
            }

            //如果是在禁止状态就显示禁止开启解除
            if (bForbiddenStartState == true)
            {
                //popup->SetMenuInfo()
                popup->ModifyMenuA(3, MF_BYPOSITION, ID_MENU_FORBIDDEN_START, "解除禁止启动");
                m_CurrentItem.forbiddenStart = false;
            }
            else
            {
                popup->ModifyMenuA(3, MF_BYPOSITION, ID_MENU_FORBIDDEN_START, "禁止启动");
                m_CurrentItem.forbiddenStart = true;
            }

            if (bForbiddenStopState == true)
            {
                popup->ModifyMenuA(4, MF_BYPOSITION, ID_MENU_FORBIDDEN_STOP, "解除禁止停止");
                m_CurrentItem.forbiddenStop = false;
            }
            else
            {
                popup->ModifyMenuA(4, MF_BYPOSITION, ID_MENU_FORBIDDEN_STOP, "禁止停止");
                m_CurrentItem.forbiddenStop = true;
            }

        }
        else if (strcmp(m_CurrentItem.serverType, "驱动") == 0)
        {
            popup->EnableMenuItem(ID_MENU_START, MF_DISABLED);
            popup->EnableMenuItem(ID_MENU_STOP, MF_DISABLED);
            popup->EnableMenuItem(ID_MENU_RESTART, MF_DISABLED);
            popup->EnableMenuItem(ID_MENU_FORBIDDEN_START, MF_DISABLED);
            popup->EnableMenuItem(ID_MENU_FORBIDDEN_STOP, MF_DISABLED);

//             popup->EnableMenuItem(ID_MENU_AUTO, MF_DISABLED);
//             popup->EnableMenuItem(ID_MENU_AUTO_DELAY, MF_DISABLED);
//             popup->EnableMenuItem(ID_MAN, MF_DISABLED);
//             popup->EnableMenuItem(ID_FORBIDDEN, MF_DISABLED);
        }

        int menuItem = popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
    }

}

// 下面为服务状态的更改 [6/12/2019 wanghaoaq]

void CManagerDlg::OnMenuStart()
{
    // TODO: Add your command handler code here
    if (m_SvrDrvManager.ControlServer(m_CurrentItem.serverName, RC_SERVER_START) == true)
    {
        RCINFO rcinfo = { 0 };
        if (m_SvrDrvManager.UpdateService(m_CurrentItem.serverName, rcinfo) == true)
        {
            m_ServiceList.SetItemText(m_CurrentItem.item, 4, rcinfo.serverStatus);
        }
    }
    else
    {
        AfxMessageBox("启动失败", MB_OK);
    }
}

void CManagerDlg::OnMenuStop()
{
    // TODO: Add your command handler code here
    if(m_SvrDrvManager.ControlServer(m_CurrentItem.serverName, RC_SERVER_STOP) == true)
    {
        RCINFO rcinfo = { 0 };
        if (m_SvrDrvManager.UpdateService(m_CurrentItem.serverName, rcinfo) == true)
        {
            m_ServiceList.SetItemText(m_CurrentItem.item, 4, rcinfo.serverStatus);
        }
    }
    else
    {
        AfxMessageBox("停止失败", MB_OK);
    }
}

void CManagerDlg::OnMenuRestart()
{
    // TODO: Add your command handler code here
    if (m_SvrDrvManager.ControlServer(m_CurrentItem.serverName, RC_SERVER_RESET) == true)
    {
        RCINFO rcinfo = { 0 };
        if (m_SvrDrvManager.UpdateService(m_CurrentItem.serverName, rcinfo) == true)
        {
            m_ServiceList.SetItemText(m_CurrentItem.item, 4, rcinfo.serverStatus);
        }
    }
}

// 下面为启动类型的修改 [6/12/2019 wanghaoaq]
void CManagerDlg::OnMenuAuto()
{
    // TODO: Add your command handler code here
    if (m_SvrDrvManager.ChangeStartType(m_CurrentItem.serverName, SERVICE_AUTO_START))
    {
        RCINFO rcinfo = { 0 };
        if (m_SvrDrvManager.UpdateService(m_CurrentItem.serverName, rcinfo) == true)
        {
            m_ServiceList.SetItemText(m_CurrentItem.item, 5, rcinfo.startType);
        }
    }
}

void CManagerDlg::OnMan()
{
    // TODO: Add your command handler code here
    if (m_SvrDrvManager.ChangeStartType(m_CurrentItem.serverName, SERVICE_DEMAND_START))
    {
        RCINFO rcinfo = { 0 };
        if (m_SvrDrvManager.UpdateService(m_CurrentItem.serverName, rcinfo) == true)
        {
            m_ServiceList.SetItemText(m_CurrentItem.item, 5, rcinfo.startType);
        }
    }
}

void CManagerDlg::OnForbidden()
{
    // TODO: Add your command handler code here
    if (m_SvrDrvManager.ChangeStartType(m_CurrentItem.serverName, SERVICE_DISABLED))
    {
        RCINFO rcinfo = { 0 };
        if (m_SvrDrvManager.UpdateService(m_CurrentItem.serverName, rcinfo) == true)
        {
            m_ServiceList.SetItemText(m_CurrentItem.item, 5, rcinfo.startType);
        }
    }
}

void CManagerDlg::OnMenuOpen()
{
    // TODO: Add your command handler code here
    char openPath[MAX_PATH] = { 0 };
    m_SvrDrvManager.DealFilePath(m_CurrentItem.filePath, openPath);
    char showPath[MAX_PATH] = { 0 };
    sprintf_s(showPath, "/select, %s", openPath);
    PVOID OldValue = NULL;
    Wow64DisableWow64FsRedirection(&OldValue);
    ShellExecute(NULL, "open", "explorer", showPath, NULL, SW_NORMAL);
    Wow64RevertWow64FsRedirection(OldValue);
}


void CManagerDlg::OnMenuDriverAuto()
{
    // TODO: Add your command handler code here
    if (m_SvrDrvManager.ChangeStartType(m_CurrentItem.serverName, SERVICE_BOOT_START))
    {
        RCINFO rcinfo = { 0 };
        if (m_SvrDrvManager.UpdateService(m_CurrentItem.serverName, rcinfo) == true)
        {
            m_ServiceList.SetItemText(m_CurrentItem.item, 5, rcinfo.startType);
        }
    }
}

void CManagerDlg::OnMenuDriverMan()
{
    // TODO: Add your command handler code here
    if (m_SvrDrvManager.ChangeStartType(m_CurrentItem.serverName, SERVICE_SYSTEM_START))
    {
        RCINFO rcinfo = { 0 };
        if (m_SvrDrvManager.UpdateService(m_CurrentItem.serverName, rcinfo) == true)
        {
            m_ServiceList.SetItemText(m_CurrentItem.item, 5, rcinfo.startType);
        }
    }
}

LRESULT CManagerDlg::OnGetDefID(WPARAM wParam, LPARAM lParam)
{
    return   MAKELONG(0, DC_HASDEFID);
}

void CManagerDlg::OnMenuForbiddenStart()
{

    RCFORBIDDENINFO rcForbiddenInfo = { 0 };
    std::map<std::string, RCFORBIDDENINFO>::iterator itMap =
    m_FobiddenMap.find(m_CurrentItem.serverName);
    //如果此service在map（json文件）里就修改内容
    if (itMap != m_FobiddenMap.end())
    {
        itMap->second.startForbidden = m_CurrentItem.forbiddenStart;
    }
    //如何不在map（json文件）里就插入
    else
    {
        strcpy_s(rcForbiddenInfo.serviceName, 128, m_CurrentItem.serverName);
        
        rcForbiddenInfo.startForbidden = true;
        
        rcForbiddenInfo.stopForbidden = false;
        m_FobiddenMap.insert(std::make_pair(rcForbiddenInfo.serviceName, rcForbiddenInfo));
    }

    WriteJsonData();
}


void CManagerDlg::OnMenuForbiddenStop()
{
    RCFORBIDDENINFO rcForbiddenInfo = { 0 };
    std::map<std::string, RCFORBIDDENINFO>::iterator itMap =
        m_FobiddenMap.find(m_CurrentItem.serverName);
    //如果此service在map（json文件）里就修改内容
    if (itMap != m_FobiddenMap.end())
    {
        itMap->second.stopForbidden = m_CurrentItem.forbiddenStop;
    }
    //如何不在map（json文件）里就插入
    else
    {
        strcpy_s(rcForbiddenInfo.serviceName, 128, m_CurrentItem.serverName);
        rcForbiddenInfo.stopForbidden = m_CurrentItem.forbiddenStop;

        rcForbiddenInfo.startForbidden = false;
        m_FobiddenMap.insert(std::make_pair(rcForbiddenInfo.serviceName, rcForbiddenInfo));
    }

    WriteJsonData();
}


void CManagerDlg::OnBnClickedButtonUpdateall()
{
    // TODO: Add your control notification handler code here
    m_SvrDrvManager.UpdateAllServices();
    ShowServices(RC_ALL);
}
