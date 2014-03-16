/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/

#include "stdafx.h"
#include "afxdialogex.h"
#include <Commctrl.h>

#include "invoker.h"
#include "RemoteControl.h"
#include "RemoteControlDlg.h"
#include "DlgNewAction.h"
#include "DlgLinkSensor.h"
#include "DlgTaskInfo.h"
#include "..\sensors\sensor_face_attribute\sensor_face_attribute.hpp"
#include "..\sensors\sensor_gesture_CI\sensor_gesture_CI.hpp"
#include "..\sensors\sensor_voice\sensor_voice.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CDispatcher g_Dispatcher;
extern CInvoker g_Invoker;
#include "..\AUXlib\AUXWidgetManager.h"
extern CAUXWidgetManager g_widgets;

/////////////////
//  CAboutDlg  //
/////////////////
CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


///////////////////
//  CWelcomeDlg  //
///////////////////

IMPLEMENT_DYNAMIC(CWelcomeDlg, CDialogEx)

CWelcomeDlg::CWelcomeDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(CWelcomeDlg::IDD, pParent)
    , m_enable(FALSE)
    , m_page(0)
{
    for (int i=0; i<MAX_HELP_PAGE; i++) {
        m_bitmap[i] = NULL;
    }
}

CWelcomeDlg::~CWelcomeDlg()
{
    for (int i=0; i<MAX_HELP_PAGE; i++) {
        if (m_bitmap[i]) { DeleteObject(m_bitmap[i]); m_bitmap[i] = NULL; }
    }
}

void CWelcomeDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Check(pDX, IDC_CHECK1, m_enable);
    DDX_Control(pDX, IDC_STATIC_WELCOME, m_welcomeImage);
    DDX_Control(pDX, IDC_BUTTON_NEXT, m_btnNext);
    DDX_Control(pDX, IDC_BUTTON_PREV, m_btnPrev);
    DDX_Control(pDX, IDC_STATIC_PAGE, m_staticPage);
    DDX_Control(pDX, IDC_BUTTON1, m_btnHelp);
    DDX_Control(pDX, IDOK, m_btnContinue);
    DDX_Control(pDX, IDC_CHECK1, m_chkAgain);
}

BEGIN_MESSAGE_MAP(CWelcomeDlg, CDialogEx)
    ON_BN_CLICKED(IDOK, &CWelcomeDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDC_BUTTON_PREV, &CWelcomeDlg::OnBnClickedButtonPrev)
    ON_BN_CLICKED(IDC_BUTTON_NEXT, &CWelcomeDlg::OnBnClickedButtonNext)
END_MESSAGE_MAP()

void CWelcomeDlg::OnBnClickedOk()
{
    CDialogEx::OnOK();
    CRemoteControlDlg* dlg = (CRemoteControlDlg*) theApp.m_pMainWnd;
    if (dlg) {
        if (dlg->m_bDisableWelcome != m_enable) {
            dlg->m_bDisableWelcome = m_enable;
            WCHAR name[MAX_PATH];
            ::GetTempPath(MAX_PATH, name);
            CString fname = name;
            fname.Append(L"RemoteControl.ini");
            WritePrivateProfileString( TEXT("HelpScreen"), TEXT("Enable"), m_enable?TEXT("0"):TEXT("1"), fname); 
        }
        dlg->CloseWelcome();
    }
}

BOOL CWelcomeDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    // MAX_HELP_PAGE 4
    m_bitmap[0] = LoadBitmap(theApp.m_hInstance, MAKEINTRESOURCE(IDB_BITMAP_WELCOME));
    m_bitmap[1] = LoadBitmap(theApp.m_hInstance, MAKEINTRESOURCE(IDB_BITMAP_WELCOME2));
    m_bitmap[2] = LoadBitmap(theApp.m_hInstance, MAKEINTRESOURCE(IDB_BITMAP_WELCOME3));
    m_bitmap[3] = LoadBitmap(theApp.m_hInstance, MAKEINTRESOURCE(IDB_BITMAP_WELCOME4));

    SetPage(0);

    return TRUE;  // return TRUE unless you set the focus to a control
}

void CWelcomeDlg::SetPage(int page)
{
    if (page<0) return;
    if (page>MAX_HELP_PAGE-1) return;
    m_page = page;
    m_btnPrev.EnableWindow(m_page>0);
    m_btnNext.EnableWindow(m_page<MAX_HELP_PAGE-1);
    CString str = L" ";
    str.AppendFormat(L"%i(%i)", m_page+1, MAX_HELP_PAGE);
    m_staticPage.SetWindowText(str);
    m_staticPage.ShowWindow(SW_SHOW);
    m_welcomeImage.SetBitmap(m_bitmap[m_page]);

    RECT rect;
    GetClientRect(&rect);
    m_welcomeImage.SetWindowPos(NULL, 0, 2, rect.right, rect.bottom+1, SWP_SHOWWINDOW);
    m_welcomeImage.Invalidate();
    m_btnHelp.Invalidate();
    m_chkAgain.Invalidate();
    m_btnPrev.Invalidate();
    m_btnNext.Invalidate();
    m_btnContinue.Invalidate();
    m_staticPage.Invalidate();
}

void CWelcomeDlg::OnBnClickedButtonPrev()
{
    SetPage(m_page-1);
}

void CWelcomeDlg::OnBnClickedButtonNext()
{
    SetPage(m_page+1);
}


/////////////////////////
//  CRemoteControlDlg  //
/////////////////////////

CRemoteControlDlg::CRemoteControlDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(CRemoteControlDlg::IDD, pParent)
    , m_isVideoControl(TRUE)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_isSensorsRun = 0;
    m_welcome = NULL;

    WCHAR name[MAX_PATH];
    ::GetTempPath(MAX_PATH, name);
    CString fname = name;
    fname.Append(L"RemoteControl.ini");
    WCHAR str[MAX_PATH];
    if (GetPrivateProfileString( TEXT("HelpScreen"), TEXT("Enable"), TEXT("1"), str, MAX_PATH, fname)) {
        m_bDisableWelcome = str[0] == L'0';
    }
    m_ToolTip = NULL;
}

void CRemoteControlDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_MFCVSLISTBOX_ACTION_LIST, m_cntrlActionList);
    DDX_Control(pDX, IDC_BUTTON_START, m_btnStart);
    DDX_Check(pDX, IDC_CHECK_VIDEO_CONTROL, m_isVideoControl);
    DDX_Control(pDX, IDC_CHECK_VIDEO_CONTROL, m_chkVideoControl);
    DDX_Control(pDX, IDC_CHECK_AUTO, m_btnAutoRun);
}

BEGIN_MESSAGE_MAP(CRemoteControlDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON_START, &CRemoteControlDlg::OnBnClickedButtonStart)
    ON_BN_CLICKED(IDOK, &CRemoteControlDlg::OnBnClickedOk)
    ON_UPDATE_COMMAND_UI(ID_SENSOR_VIDEOCONTROL, &CRemoteControlDlg::OnUpdateSensorVideocontrol)
    ON_COMMAND(ID_FILE_NEW, &CRemoteControlDlg::OnFileNew)
    ON_COMMAND(ID_FILE_LOAD, &CRemoteControlDlg::OnFileLoad)
    ON_COMMAND(ID_FILE_SAVE, &CRemoteControlDlg::OnFileSave)
    ON_COMMAND(ID_SENSORS_SHOWTASKLIST, &CRemoteControlDlg::OnSensorsShowtasklist)
    ON_COMMAND(ID_FILE_ADDFROMFILE, &CRemoteControlDlg::OnFileAddfromfile)
    ON_UPDATE_COMMAND_UI(ID_SELECT_GESTURERECOGNITION, &CRemoteControlDlg::OnUpdateSelectGesturerecognition)
    ON_UPDATE_COMMAND_UI(ID_SELECT_FACERECOGNITION, &CRemoteControlDlg::OnUpdateSelectFacerecognition)
    ON_UPDATE_COMMAND_UI(ID_SELECT_VOICERECOGNITION, &CRemoteControlDlg::OnUpdateSelectVoicerecognition)
    ON_WM_MOVE()
    ON_COMMAND(ID_HELP_ABOUT, &CRemoteControlDlg::OnHelpAbout)
    ON_COMMAND(ID_HELP_HELPSCREEN, &CRemoteControlDlg::OnHelpHelpscreen)
    ON_WM_CLOSE()
END_MESSAGE_MAP()

// CRemoteControlDlg message handlers
BOOL CRemoteControlDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    if (m_hIcon) {
        SetIcon(m_hIcon, TRUE);     // Set big icon
        SetIcon(m_hIcon, FALSE);        // Set small icon
    }

    // IDM_ABOUTBOX must be in the system command range.
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL) {
        BOOL bNameValid;
        CString strAboutMenu;
        bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
        ASSERT(bNameValid);
        if (!strAboutMenu.IsEmpty()) {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    // update menu Sensors->Snabled Sensecors
    CMenu* menu = GetMenu();
    if (menu) {
        CSensor* sensor;
        sensor = g_Dispatcher.GetSensorById(CSensor::Sensor_FaceAttributes);
        if (sensor) menu->CheckMenuItem(ID_SELECT_FACERECOGNITION, sensor->IsEnable() ? MF_CHECKED : MF_UNCHECKED);
        sensor = g_Dispatcher.GetSensorById(CSensor::Sensor_Gesture_detector);
        if (sensor) menu->CheckMenuItem(ID_SELECT_GESTURERECOGNITION, sensor->IsEnable() ? MF_CHECKED : MF_UNCHECKED);
        sensor = g_Dispatcher.GetSensorById(CSensor::Sensor_Voice_recognition);
        if (sensor) menu->CheckMenuItem(ID_SELECT_VOICERECOGNITION, sensor->IsEnable() ? MF_CHECKED : MF_UNCHECKED);
    }

    // apply tooltips
    if (m_ToolTip==NULL) {
        m_ToolTip= new CToolTipCtrl();
        m_ToolTip->Create(this);
        CWnd* ww = GetDlgItem(IDOK);
        CWnd* w2 = GetDlgItem(IDC_BUTTON_START);
        m_ToolTip->AddTool(GetDlgItem(IDOK),L"Shutdown the Remote Control");
        m_ToolTip->AddTool(GetDlgItem(IDC_BUTTON_START),L"Start/Stop All Enabled Sensors");
        m_ToolTip->Activate(TRUE);
    }

    // check input argument
    int _argc = 0;
    LPWSTR* _argv = CommandLineToArgvW(GetCommandLineW(), &_argc);
    if (_argv && _argc>1) {
        CString name = _argv[1];
        if (g_Dispatcher.LoadActionList(&name)) {
            m_btnAutoRun.SetCheck(TRUE);
            UpdateActionList();
            OnBnClickedButtonStart();
            if (_argv) LocalFree(_argv);
            return TRUE;
        }
    }
    if (_argv) LocalFree(_argv);

    // check autorun
    WCHAR name[MAX_PATH];
    ::GetTempPath(MAX_PATH, name);
    CString fname = name;
    fname.Append(L"RemoteControl.ini");
    WCHAR str[MAX_PATH];
    BOOL autorun = FALSE;
    if (GetPrivateProfileString( TEXT("Autorun"), TEXT("Enable"), NULL, str, MAX_PATH, fname)) {
        autorun = (str[0] == L'1');
    }
    if (autorun && GetPrivateProfileString( TEXT("Autorun"), TEXT("Filename"), NULL, str, MAX_PATH, fname)) {
        CString name = str;
        if (g_Dispatcher.LoadActionList(&name)) {
            m_btnAutoRun.SetCheck(TRUE);
            UpdateActionList();
            OnBnClickedButtonStart();
            return TRUE;
        }
    }

    OpenWelcome();
    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CRemoteControlDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    } else {
        CDialogEx::OnSysCommand(nID, lParam);
    }
}

void CRemoteControlDlg::OnPaint()
{
    if (IsIconic()) {
        CPaintDC dc(this);

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
    } else {

        CDialogEx::OnPaint();
    }
}

HCURSOR CRemoteControlDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CRemoteControlDlg::OnBnClickedButtonStart()
{
    if (m_isSensorsRun) {
        g_widgets.SaveConfiguration();
        g_Invoker.StopAllSensors();
        m_isSensorsRun = 0;
        m_btnStart.SetWindowText(L"Start");
    } else {
        if (g_Invoker.StartAllSensors(m_isVideoControl)) {
            m_btnStart.SetWindowText(L"Stop");
            m_isSensorsRun = 1;
            ShowWindow(SW_MINIMIZE);
        }
    }
}

void CRemoteControlDlg::Release()
{
    if (m_isSensorsRun) {
        g_Invoker.StopAllSensors();
        m_isSensorsRun = 0;
        Sleep(100);
    }
}

void CRemoteControlDlg::OnBnClickedOk()
{
    Release();
    int autorun = m_btnAutoRun.GetCheck();
    WCHAR name[MAX_PATH];
    ::GetTempPath(MAX_PATH, name);
    CString fname = name;
    fname.Append(L"RemoteControl.ini");
    WritePrivateProfileString( TEXT("Autorun"), TEXT("Enable"), autorun?TEXT("1"):TEXT("0"), fname); 
    if (autorun) {
        // save session for an autorun
        CString _name = name;
        _name.Append(L"RemoteControl.xml");
        WritePrivateProfileString( TEXT("Autorun"), TEXT("Filename"), _name, fname); 
        m_cntrlActionList.FillTaskList();
        g_Dispatcher.SaveActionList(&_name);
    }

    g_widgets.SaveConfiguration();
    CDialogEx::OnOK();
}

// temporary fix problem with the voice sensor restart
void StopAudio();

void CRemoteControlDlg::OnClose()
{
    Release();
    StopAudio();
    CDialogEx::OnClose();
}

void CRemoteControlDlg::OnUpdateSensorVideocontrol(CCmdUI *pCmdUI)
{
    CMenu* menu = GetMenu();
    if (menu) {
        BOOL visible = m_isVideoControl;
        switch (menu->CheckMenuItem(ID_SENSOR_VIDEOCONTROL, m_isVideoControl ? MF_UNCHECKED : MF_CHECKED)) {
            case MF_UNCHECKED: m_isVideoControl = 1; break;
            case MF_CHECKED: m_isVideoControl = 0; break;
        }
        if (!visible && m_isVideoControl) {
            g_widgets.ShowAllWindows(TRUE);
        }
    }
}

void CRemoteControlDlg::OnFileNew()
{
    m_cntrlActionList.RemoveAllItems();
    g_Dispatcher.NewActionList();
}

void CRemoteControlDlg::OnFileLoad()
{
    if (g_Dispatcher.LoadActionList()) UpdateActionList();
}

void CRemoteControlDlg::OnFileSave()
{
    m_cntrlActionList.FillTaskList();
    g_Dispatcher.SaveActionList();
}

void CRemoteControlDlg::OnSensorsShowtasklist()
{
    CDlgTaskInfo dialog;
    dialog.DoModal();
}

void CRemoteControlDlg::UpdateActionList()
{
    m_cntrlActionList.RemoveAllItems();
    stActionDescriptor* descr = NULL;
    for (int count=0;;count++) {
        descr = g_Dispatcher.GetAction(count);
        if (!descr) break;
        m_cntrlActionList.AppendAction(descr, count);
    }
    m_cntrlActionList.FillTaskList();
}

void CRemoteControlDlg::OnFileAddfromfile()
{
    if (g_Dispatcher.LoadActionList(NULL, false)) UpdateActionList();
}

void CRemoteControlDlg::OnUpdateSelectGesturerecognition(CCmdUI *pCmdUI)
{
    CMenu* menu = GetMenu();
    if (!menu) return;
    CSensor* sensor = g_Dispatcher.GetSensorById(CSensor::Sensor_Gesture_detector);
    if (!sensor) {
        pCmdUI->SetCheck(0);
        return;
    }
    BOOL enable = !sensor->IsEnable();
    sensor->Enable(enable);
    menu->CheckMenuItem(pCmdUI->m_nID, enable ? MF_CHECKED : MF_UNCHECKED);
    if (sensor->IsRun() && !sensor->IsEnable()) sensor->Stop();
    return;
}

void CRemoteControlDlg::OnUpdateSelectFacerecognition(CCmdUI *pCmdUI)
{
    CMenu* menu = GetMenu();
    if (!menu) return;
    CSensor* sensor = g_Dispatcher.GetSensorById(CSensor::Sensor_FaceAttributes);
    if (!sensor) {
        pCmdUI->SetCheck(0);
        return;
    }
    BOOL enable = !sensor->IsEnable();
    sensor->Enable(enable);
    menu->CheckMenuItem(pCmdUI->m_nID, enable ? MF_CHECKED : MF_UNCHECKED);
    if (sensor->IsRun() && !sensor->IsEnable()) sensor->Stop();
    return;
}

void CRemoteControlDlg::OnUpdateSelectVoicerecognition(CCmdUI *pCmdUI)
{
    CMenu* menu = GetMenu();
    if (!menu) return;
    CSensor* sensor = g_Dispatcher.GetSensorById(CSensor::Sensor_Voice_recognition);
    if (!sensor) {
        pCmdUI->SetCheck(0);
        return;
    }
    BOOL enable = !sensor->IsEnable();
    sensor->Enable(enable);
    menu->CheckMenuItem(pCmdUI->m_nID, enable ? MF_CHECKED : MF_UNCHECKED);
    if (sensor->IsRun() && !sensor->IsEnable()) sensor->Stop();
    return;
}

void CRemoteControlDlg::OpenWelcome(BOOL enable)
{
    if (!enable && m_bDisableWelcome) return;
    if (!m_welcome) {
        m_welcome = new CWelcomeDlg(this);
        m_welcome->m_enable = m_bDisableWelcome;
        if( !m_welcome || FALSE==m_welcome->Create(IDD_WELCOMEBOX,this) ) {
            delete m_welcome;
            m_welcome = NULL;
        }
    }
    if (m_welcome) {
        RECT rect;
        GetClientRect(&rect);
        TITLEBARINFO tb;
        tb.cbSize = sizeof(tb);
        GetTitleBarInfo(&tb);
        rect.top -= tb.rcTitleBar.bottom-tb.rcTitleBar.top-2;
        ClientToScreen(&rect);
        m_welcome->SetWindowPos(NULL, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, SWP_SHOWWINDOW);
    }
}

void CRemoteControlDlg::CloseWelcome()
{
    if (m_welcome) {
        m_welcome->DestroyWindow();
        delete m_welcome;
        m_welcome = NULL;
    }
}

void CRemoteControlDlg::OnMove(int x, int y)
{
    CDialogEx::OnMove(x, y);

    if (m_welcome) {
        RECT rect;
        GetClientRect(&rect);
        TITLEBARINFO tb;
        tb.cbSize = sizeof(tb);
        GetTitleBarInfo(&tb);
        rect.top -= tb.rcTitleBar.bottom-tb.rcTitleBar.top-2;
        ClientToScreen(&rect);
        m_welcome->SetWindowPos(NULL, rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, SWP_SHOWWINDOW);
    }
}

void CRemoteControlDlg::OnHelpAbout()
{
    CAboutDlg dlg;
    dlg.DoModal();
}

void CRemoteControlDlg::OnHelpHelpscreen()
{
    OpenWelcome(TRUE);
}

BOOL CRemoteControlDlg::PreTranslateMessage(MSG* pMsg)
{
    if ((pMsg->message==WM_MOUSEMOVE) && m_ToolTip) m_ToolTip->RelayEvent(pMsg);
    return CDialogEx::PreTranslateMessage(pMsg);
}

///////////////
//  CMyList  //
///////////////

CWnd* CMyList::OnCreateList()
{
    CWnd* wnd = CVSListBox::OnCreateList();
    if (m_pWndList) {
        m_pWndList->SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_ONECLICKACTIVATE|LVS_EX_FLATSB|LVS_EX_CHECKBOXES|LVS_EX_INFOTIP);
        if (m_pToolTip==NULL) {
            if (CTooltipManager::CreateToolTip(m_pToolTip, m_pWndList, AFX_TOOLTIP_TYPE_BUTTON) && m_pToolTip) {
                m_pToolTip->Activate(TRUE);
                m_pToolTip->SetMaxTipWidth(1200);
            }
        }
    }
    AddButton(IDB_BITMAP_EDIT, L"Edit Action", 0, 0, 0);
    AddButton(IDB_BITMAP_MAKE, L"Execute Action", 0, 0, 0);
    return wnd;
}

int CMyList::EditItem(stActionDescriptor* descr, int sel)
{
    // edit actions
    POINT cursorPos;
    GetCursorPos(&cursorPos);
    CDlgNewAction dialog;
    dialog.m_actionList = NULL;
    dialog.m_currentAction = *descr;
    if (dialog.DoModal()==IDOK) {
        if (sel>=0) {
            *descr = dialog.m_currentAction;
            EditItem(sel);
            CString title;
            descr->GetTitle(title);
            SetItemText(sel, title);
            ListView_SetCheckState(m_pWndList->GetSafeHwnd(), sel, descr->isEnable);
        }
    }
    FillTaskList();
    SetCursorPos(cursorPos.x, cursorPos.y);
    return GetCount();
}

void CMyList::OnClickButton(int iButton)
{
    int sel = GetSelItem();
    switch (iButton) {
    case 0: // connect with a sensor
        if (sel>=0) {
           // edit the current task
            int id = (int)GetItemData(sel);
            EditItem(g_Dispatcher.GetActionById(id), sel);
        }
        break;
    case 1: // edit
        if (sel>=0) {
            int id = (int)GetItemData(sel);
            g_Invoker.ExecuteAction(id);
        }
        break;
    default:
        CVSListBox::OnClickButton(iButton);
        break;
    }
}

void CMyList::RemoveAllItems()
{
    for (int i=GetCount(); i>0; --i) {
        RemoveItem(i-1);
    }
    FillTaskList();
}

int CMyList::FillTaskList()
{
    // return task number
    g_Dispatcher.StartFillTaskList();
    for (int i=0; i<GetCount(); i++) {
        g_Dispatcher.AppendTaskToList((int)GetItemData(i));
    }
    int ret = g_Dispatcher.StopFillTaskList();
    UpdateTips(NULL);
    return ret;
}

BOOL CMyList::RemoveItem(int iIndex)
{
    BOOL ret = 0;
    if (iIndex>=0) {
        ret = __super::RemoveItem(iIndex);
        FillTaskList();
    }
    return ret;
}

int CMyList::AddItem(const CString& strIext, DWORD_PTR dwData, int iIndex)
{
    // check enable
    BOOL isCheck = FALSE;
    stActionDescriptor* descr = g_Dispatcher.GetActionById((int)dwData);
    if (descr && descr->isEnable) isCheck = TRUE;

    if (iIndex>=0) {
        int count = __super::AddItem(strIext, dwData, iIndex);
        ListView_SetCheckState(m_pWndList->GetSafeHwnd(), count, isCheck);
        FillTaskList();
        return count;
    }

    // create new actions
    POINT cursorPos;
    GetCursorPos(&cursorPos);
    CDlgNewAction dialog;
    dialog.m_actionList = this;
    int sel=0;
    if (dialog.DoModal()==IDOK && dialog.m_currentAction.isActual) {
        int sel = GetCount()-1;
        if (sel>=0) {
            SelectItem(sel);
            EditItem(sel);
            int id = (int)GetItemData(sel);
            descr = g_Dispatcher.GetActionById(id);
            if (descr) ListView_SetCheckState(m_pWndList->GetSafeHwnd(), sel, descr->isEnable);
        }
    }
    FillTaskList();
    SetCursorPos(cursorPos.x, cursorPos.y);
    return GetCount();
}

int CMyList::AppendAction(stActionDescriptor* descr, int id)
{
    if (!descr->isActual) return GetCount();
    CString title;
    descr->GetTitle(title);
    int item = CVSListBox::AddItem(title, id, GetCount());
    ListView_SetCheckState(m_pWndList->GetSafeHwnd(), item, descr->isEnable);
    return GetCount();
}

int CMyList::LinkSensor(stActionDescriptor* descr)
{
    POINT cursorPos;
    GetCursorPos(&cursorPos);
    if (!descr) return -1;
    int sel = GetSelItem();
    CDlgLinkSensor dialog(descr);
    if ( dialog.DoModal() == IDOK ) {
        UpdateData();
    }
    ListView_SetCheckState(m_pWndList->GetSafeHwnd(), sel, descr->isEnable);
    FillTaskList();
    SetCursorPos(cursorPos.x, cursorPos.y);
    return 0;
}

BOOL CMyList::EditItem(int iIndex)
{
    if (iIndex<0 || iIndex>=GetCount()) return false;
    int id = (int)GetItemData(iIndex);
    stActionDescriptor* descr = g_Dispatcher.GetActionById(id);
    if (!descr) return false;
    LinkSensor(descr);
    return true;
}

BOOL CMyList::UpdateTips(MSG* pMsg)
{
    if (m_pToolTip==NULL) return FALSE;
    if (pMsg==NULL) {
        m_pToolTip->DelTool(m_pWndList, 1);
        return TRUE;
    }
    TOOLINFO tinfo;
    CPoint point = pMsg->pt;
    m_pWndList->ScreenToClient(&point);
    if (m_pToolTip->HitTest(m_pWndList, point, &tinfo)==FALSE) {
        // reset tips
        m_pToolTip->DelTool(m_pWndList, 1);
        RECT r1, r2;
        GetWindowRect(&r1);
        m_pWndList->GetWindowRect(&r2);
        int shiftY = r2.top - r1.top - 2;
        shiftY = 0;
        CRect itemRect;
        int nBottom = min(m_pWndList->GetItemCount(), m_pWndList->GetTopIndex()+m_pWndList->GetCountPerPage());
        for( int n = m_pWndList->GetTopIndex(); n < nBottom; n++ ) {
            // Get bounding rect of item and check whether point falls in it.
            m_pWndList->GetItemRect( n, &itemRect, LVIR_BOUNDS );
            itemRect.top += shiftY+1;
            itemRect.bottom += shiftY-1;
            itemRect.right -= 20;
            if (itemRect.PtInRect(point)) {
                //SelectItem(n);
                int item = (int)GetItemData(n);
                stActionDescriptor* descr = g_Dispatcher.GetActionById(item);
                if (point.x<itemRect.left+20) {
                    // checkbox
                    itemRect.right = itemRect.left+20;
                    if (descr->isEnable) m_strToolTip = L"Set Disable";
                    else m_strToolTip = L"Set Enable";
                } else {
                    itemRect.left += 20;
                    m_strToolTip = L"Action = ";
                    descr->GetTitle(m_strToolTip);
                    descr->GetSensorTitle(m_strToolTip);
                }
                m_pToolTip->AddTool(m_pWndList, m_strToolTip.GetBuffer(), &itemRect, 1);
                break;
            }
        } 
    }
    m_pToolTip->RelayEvent(pMsg);
    return TRUE;
}

BOOL CMyList::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->hwnd == GetListHwnd()) {
        switch (pMsg->message) {
        case WM_MOUSEMOVE:
            UpdateTips(pMsg);
            break;
        case WM_LBUTTONUP:
            {
                UpdateTips(NULL);
                CPoint ptClick = pMsg->pt;
                m_pWndList->ScreenToClient(&ptClick);
                int iItem = m_pWndList->HitTest(ptClick);
                if (iItem>=0) {
                    int id = (int)m_pWndList->GetItemData(iItem);
                    stActionDescriptor* descr = g_Dispatcher.GetActionById(id);
                    if (descr) {
                        BOOL enable = ListView_GetCheckState(pMsg->hwnd, iItem);
                        if (enable != descr->isEnable) {
                            if (!descr->sensorId) {
                                LinkSensor(descr);
                                ListView_SetCheckState(pMsg->hwnd, iItem, descr->isEnable );
                                m_pWndList->UpdateData();
                                m_pWndList->Invalidate();
                                return TRUE;
                            }
                            descr->isEnable = enable;
                            FillTaskList();
                            return TRUE;
                        }
                    }
                }
            }
            break;
        }
    }
    return CVSListBox::PreTranslateMessage(pMsg);
}
