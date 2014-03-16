/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/

#include "stdafx.h"

#include "RemoteControl.h"
#include "Invoker.h"
#include "DlgNewAction.h"
#include "afxdialogex.h"
#include <Psapi.h>
#include <Strsafe.h>

extern CDispatcher g_Dispatcher;
extern CInvoker g_Invoker;

// CDlgNewAction dialog

IMPLEMENT_DYNAMIC(CDlgNewAction, CDialogEx)

CDlgNewAction::CDlgNewAction(CWnd* pParent /*=NULL*/) : CDialogEx(CDlgNewAction::IDD, pParent)
    , m_bGrouping(TRUE), m_actionList(NULL), m_isAdded(false)
{
}

CDlgNewAction::~CDlgNewAction()
{
}

void CDlgNewAction::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_SELECT_APP, m_cmbSelectApp);
    DDX_Check(pDX, IDC_CHECK_GROUP, m_bGrouping);
    DDX_Control(pDX, IDC_COMBO_SELECT_CONTROL, m_cmbSelectControl);
    DDX_Control(pDX, IDC_COMBO_ACTION_TYPE, m_cmbSelectType);
    DDX_Control(pDX, IDC_STATIC_INFO, m_infoBoard);
    DDX_Control(pDX, IDC_HOTKEY_PRESS, m_hotPressKey);
    DDX_Control(pDX, IDC_STATIC_HOT_INFO, m_hotKeyInfo);
    DDX_Control(pDX, IDC_CHECK_GROUP, m_chkGroup);
    DDX_Control(pDX, IDC_BUTTON_TRY, m_btnTryAction);
    DDX_Control(pDX, IDC_BUTTON_APPLY, m_btnAddToList);
    DDX_Control(pDX, IDOK, m_btnSensor);
}

BEGIN_MESSAGE_MAP(CDlgNewAction, CDialogEx)
    ON_BN_CLICKED(IDC_CHECK_STYLE, &CDlgNewAction::OnBnClickedCheckStyle)
    ON_CBN_SELCHANGE(IDC_COMBO_SELECT_APP, &CDlgNewAction::OnCbnSelchangeComboSelectApp)
    ON_BN_CLICKED(IDOK, &CDlgNewAction::OnBnClickedApply)
    ON_BN_CLICKED(IDCANCEL, &CDlgNewAction::OnBnClickedCancel)
    ON_BN_CLICKED(IDC_BUTTON_TRY, &CDlgNewAction::OnBnClickedButtonTry)
    ON_CBN_SELCHANGE(IDC_COMBO_ACTION_TYPE, &CDlgNewAction::OnCbnSelchangeComboActionType)
    ON_CBN_SELCHANGE(IDC_COMBO_SELECT_CONTROL, &CDlgNewAction::OnCbnSelchangeComboSelectControl)
    ON_BN_CLICKED(IDC_BUTTON_APPLY, &CDlgNewAction::OnBnClickedButtonAddToList)
    ON_WM_HELPINFO()
    ON_BN_CLICKED(IDC_CHECK_GROUP, &CDlgNewAction::OnBnClickedCheckGroup)
END_MESSAGE_MAP()

BOOL CDlgNewAction::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    if (m_currentAction.isActual) {
        m_btnAddToList.EnableWindow(FALSE);
        m_btnAddToList.ShowWindow(SW_HIDE);
        m_btnSensor.SetWindowText(L"Next");
        SetWindowText(L"Edit Action");
        m_chkGroup.SetCheck(m_currentAction.isGrouping);
        m_chkGroup.EnableWindow(true);
        m_chkGroup.ShowWindow(SW_SHOW);
        m_bGrouping = m_currentAction.isGrouping;
        FindAvailableApps();
        int sel = m_cmbSelectApp.FindStringExact(0, m_currentAction.windowName);
        m_cmbSelectApp.SetCurSel(max(sel,0));
        OnCbnSelchangeComboSelectApp();
        sel = 0;
        for (int i=0; i<m_cmbSelectType.GetCount(); i++) {
            if (m_cmbSelectType.GetItemData(i) == m_currentAction.type) { sel = i; break; }
        }
        m_cmbSelectType.SetCurSel(sel);
        OnCbnSelchangeComboActionType();
        int type = 0;
        switch (m_currentAction.type) {
        case stActionDescriptor::MY_AT_NAVIGATION:
            type = m_currentAction.navigate_type;
            break;
        case stActionDescriptor::MY_AT_KEYBOARD:
            type = m_currentAction.key_type;
            m_hotPressKey.SetHotKey(m_currentAction.key_code&0xff, (m_currentAction.key_code>>8)&0xff);
            break;
        case stActionDescriptor::MY_AT_PRINT:
            type = m_currentAction.system_type;
            break;
        case stActionDescriptor::MY_AT_BUTTON:
            break;
        }
        sel = 0;
        for (int i=0; i<m_cmbSelectControl.GetCount(); i++) {
            if (m_cmbSelectControl.GetItemData(i) == type) { sel = i; break; }
        }
        m_cmbSelectControl.SetCurSel(sel);
        OnCbnSelchangeComboSelectControl();
        return TRUE;
    }

    m_chkGroup.SetCheck(m_bGrouping);
    m_chkGroup.EnableWindow(true);
    m_btnTryAction.EnableWindow();
    m_currentAction.isActual = false;
    FindAvailableApps();
    return TRUE;
}

void CDlgNewAction::ReleaseComboBox(CComboBox *cmb)
{
    for (int i=cmb->GetCount()-1; i>=0; i--) {
        DWORD data = (DWORD)cmb->GetItemData(i);
        if (data>100) {
            stActionDescriptor* app = (stActionDescriptor*)cmb->GetItemDataPtr(i);
            if (app) delete app;
            cmb->SetItemDataPtr(i, NULL);
        }
        cmb->DeleteString(i);
    }
    PrintActionInfo(NULL, &m_infoBoard);
    m_hotKeyInfo.ShowWindow(SW_HIDE);
    m_hotPressKey.ShowWindow(SW_HIDE);
}

void CDlgNewAction::FindAvailableApps()
{
    m_currentAction.isActual = false;
    m_cmbSelectType.ShowWindow(SW_HIDE);
    m_cmbSelectControl.ShowWindow(SW_HIDE);
    // clear the appl list
    ReleaseComboBox(&m_cmbSelectApp);
    int ind = 0;
    if (m_bGrouping) {
        ind = m_cmbSelectApp.AddString(L" < select a group name > ");
    } else {
        ind = m_cmbSelectApp.AddString(L" < select an application > ");
    }
    m_currentAction.isGrouping = m_bGrouping;
    m_cmbSelectApp.SetItemDataPtr(ind, NULL);
    m_cmbSelectApp.SetCurSel(0);

    IUIAutomation* automation = g_Invoker.GetAutomation();
    if (!automation) return;

    // find new application
    HWND hWnd = NULL;
    if (m_bGrouping) {
        // add "Any app;" selector
        stActionDescriptor* app = new stActionDescriptor;
        StringCchCopy(app->windowName, MAX_PATH, L"_any_active_application_");
        ind = m_cmbSelectApp.AddString(app->windowName);
        app->specific = stActionDescriptor::MY_ST_ANY_ACTIVE;
        m_cmbSelectApp.SetItemDataPtr(ind, app);
    }

    IUIAutomationElement* root = NULL;
    automation->GetRootElement(&root);
    IUIAutomationElementArray* list=NULL;
    IUIAutomationCondition* cond;
    automation->CreateTrueCondition(&cond);
    if (cond && root) {
        root->FindAll(TreeScope_Children, cond, &list);
    }
    if (cond) cond->Release();
    if (root) root->Release();
    if (!list)  return;
    BSTR winName = SysAllocStringLen(NULL, MAX_PATH);
    BSTR winName2 = SysAllocStringLen(NULL, MAX_PATH);
    BSTR className = SysAllocStringLen(NULL, MAX_PATH);
    int in = 0;
    list->get_Length(&in);
    for (int i=0; i<in; i++) {
        IUIAutomationElement* el = NULL;
        if (SUCCEEDED( list->GetElement(i, &el) ) && el ) {
            el->get_CurrentName(&winName);
            if (SysStringLen(winName)) {
                stActionDescriptor* app = NULL;
                HANDLE hProcess = NULL;
                if (m_bGrouping) {
                    int pid;
                    el->get_CurrentProcessId(&pid);
                    hProcess = OpenProcess( PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, FALSE, pid );
                    if (hProcess) {
                        int len = GetProcessImageFileName(hProcess, winName2, MAX_PATH);
                        if (!len) {
                            CloseHandle(hProcess);
                            continue;
                        }
                    }
                    WCHAR filename[_MAX_FNAME];
                    _wsplitpath_s(winName2, NULL,0, NULL,0, filename,_MAX_FNAME, NULL,0);

                    // prevent double name
                    CString item;
                    bool isExist = false;
                    for (int i=0; i<m_cmbSelectApp.GetCount(); i++) {
                        int len = m_cmbSelectApp.GetLBTextLen(i);
                        m_cmbSelectApp.GetLBText(i, item.GetBuffer(len));
                        if (item.Compare(filename) == 0) {
                            isExist = true;
                            break;
                        }
                    }
                    item.ReleaseBuffer();
                    if (isExist) {
                        if (hProcess)  CloseHandle(hProcess);
                        continue;
                    }

                    app = new stActionDescriptor;
                    app->isGrouping = true;
                    StringCchCopy(app->windowName, _MAX_FNAME, filename);
                    ind = m_cmbSelectApp.AddString(filename);
                } else {
                    app = new stActionDescriptor;
                    StringCchCopy(app->windowName, MAX_PATH, winName);
                    ind = m_cmbSelectApp.AddString(app->windowName);
                }

                el->get_CurrentClassName(&className);
                StringCchCopy(app->className, MAX_PATH, className);
                m_cmbSelectApp.SetItemDataPtr(ind, app);

                if (hProcess)  CloseHandle(hProcess);
            }
        }
    }
    SysFreeString(winName);
    SysFreeString(winName2);
    SysFreeString(className);
    list->Release();
    return;
}

void CDlgNewAction::FindAvailableTypes()
{
    m_currentAction.isActual = false;
    m_cmbSelectControl.ShowWindow(SW_HIDE);
    ReleaseComboBox(&m_cmbSelectType);
    int ind = 0;
    if (m_currentAction.specific == stActionDescriptor::MY_ST_ANY_ACTIVE) {
        m_btnTryAction.EnableWindow(FALSE);
        ind = m_cmbSelectType.AddString(L"Keyboard Event");
        m_cmbSelectType.SetItemData(ind, stActionDescriptor::MY_AT_KEYBOARD);
        m_cmbSelectType.SetCurSel(0);
        m_cmbSelectType.EnableWindow(FALSE);
        OnCbnSelchangeComboActionType();
    } else {
        m_btnTryAction.EnableWindow();
        m_cmbSelectType.EnableWindow();
        m_cmbSelectType.AddString(L" < select an action type >");
        m_cmbSelectType.SetCurSel(0);
        ind = m_cmbSelectType.AddString(L"Windows Navigation");
        m_cmbSelectType.SetItemData(ind, stActionDescriptor::MY_AT_NAVIGATION);
        ind = m_cmbSelectType.AddString(L"Keyboard Event");
        m_cmbSelectType.SetItemData(ind, stActionDescriptor::MY_AT_KEYBOARD);
        if (!m_currentAction.isGrouping) {
            ind = m_cmbSelectType.AddString(L"Invoke a Button");
            m_cmbSelectType.SetItemData(ind, stActionDescriptor::MY_AT_BUTTON);
        }
        ind = m_cmbSelectType.AddString(L"Print text");
        m_cmbSelectType.SetItemData(ind, stActionDescriptor::MY_AT_PRINT);
    }
}

void CDlgNewAction::FindAvailableKeyboardEvents()
{
    m_currentAction.isActual = false;
    ReleaseComboBox(&m_cmbSelectControl);
    int ind = m_cmbSelectControl.AddString(L" < select a keyboard event >");
    int aa = (int) (stActionDescriptor::MY_KT_NONE);
    m_cmbSelectControl.SetItemData(ind, aa);
    ind = m_cmbSelectControl.AddString(L"Press Key");
    m_cmbSelectControl.SetItemData(ind, stActionDescriptor::MY_KT_PRESS);
    ind = m_cmbSelectControl.AddString(L"Special:: <Enter>");
    m_cmbSelectControl.SetItemData(ind, stActionDescriptor::MY_KT_ENTER);
    ind = m_cmbSelectControl.AddString(L"Special:: <Escape>");
    m_cmbSelectControl.SetItemData(ind, stActionDescriptor::MY_KT_ESC);
    ind = m_cmbSelectControl.AddString(L"Special:: <Delete>");
    m_cmbSelectControl.SetItemData(ind, stActionDescriptor::MY_KT_DEL);
    ind = m_cmbSelectControl.AddString(L"Special:: <Tab>");
    m_cmbSelectControl.SetItemData(ind, stActionDescriptor::MY_KT_TAB);
    ind = m_cmbSelectControl.AddString(L"Special:: <WIN>");
    m_cmbSelectControl.SetItemData(ind, stActionDescriptor::MY_KT_WIN);
    ind = m_cmbSelectControl.AddString(L"Special:: <Space>");
    m_cmbSelectControl.SetItemData(ind, stActionDescriptor::MY_KT_SPACE);
    ind = m_cmbSelectControl.AddString(L"Special:: <BackSpace>");
    m_cmbSelectControl.SetItemData(ind, stActionDescriptor::MY_KT_BKSPACE);
    m_cmbSelectControl.SetCurSel(0);
}

void CDlgNewAction::FindAvailablePrint()
{
    m_currentAction.isActual = false;
    ReleaseComboBox(&m_cmbSelectControl);
    int ind = m_cmbSelectControl.AddString(L" < select a print type >");
    m_cmbSelectControl.SetItemData(ind, stActionDescriptor::MY_PT_NONE);
    ind = m_cmbSelectControl.AddString(L"Print event name");
    m_cmbSelectControl.SetItemData(ind, stActionDescriptor::MY_PT_PRINT_EVENT);
    ind = m_cmbSelectControl.AddString(L"Print event name and <Return>");
    m_cmbSelectControl.SetItemData(ind, stActionDescriptor::MY_PT_PRINT_EVENT_ENTER);
    ind = m_cmbSelectControl.AddString(L"Print numbers");
    m_cmbSelectControl.SetItemData(ind, stActionDescriptor::MY_PT_PRINT_NUMBER);
    m_cmbSelectControl.SetCurSel(0);
}

void CDlgNewAction::FindAvailableNavigation()
{
    m_currentAction.isActual = false;
    ReleaseComboBox(&m_cmbSelectControl);
    int ind = m_cmbSelectControl.AddString(L" < select a action >");
    m_cmbSelectControl.SetItemData(ind, stActionDescriptor::MY_NT_NONE);
    ind = m_cmbSelectControl.AddString(L"Bring window to top");
    m_cmbSelectControl.SetItemData(ind, stActionDescriptor::MY_NT_TOP);
    ind = m_cmbSelectControl.AddString(L"Minimize");
    m_cmbSelectControl.SetItemData(ind, stActionDescriptor::MY_NT_MIN);
    ind = m_cmbSelectControl.AddString(L"Maximize");
    m_cmbSelectControl.SetItemData(ind, stActionDescriptor::MY_NT_MAX);
    m_cmbSelectControl.SetCurSel(0);
}

void CDlgNewAction::FindAvailableInvoker(PROPERTYID propertyID)
{
    m_currentAction.isActual = false;
    IUIAutomation* automation = g_Invoker.GetAutomation();
    if (!automation) return;

    HWND hWnd = NULL;

    if (m_currentAction.isGrouping) {
        hWnd = g_Dispatcher.GetInstance(m_currentAction.windowName);
    } else {
        hWnd = g_Dispatcher.GetWindow(m_currentAction.className, m_currentAction.windowName);
    }
    if (!IsWindow(hWnd)) return;

    // emulate windows activation
    POINT curentPos;
    ::GetCursorPos(&curentPos);

    IUIAutomationElement* el = NULL;
    automation->ElementFromHandle(hWnd, &el);
    if (!el) {
        ::SetCursorPos(curentPos.x, curentPos.y);
        return;
    }
    
    VARIANT var;
    var.vt = VT_I4; 
    var.lVal = propertyID;
    IUIAutomationCondition* cond = NULL;
    if (SUCCEEDED(automation->CreatePropertyCondition(UIA_ControlTypePropertyId, var, &cond)) && cond) {
        IUIAutomationElementArray* list=NULL;
        if (SUCCEEDED(el->FindAll(TreeScope_Subtree, cond, &list)) && list) {
            int in = 0;
            list->get_Length(&in);
            for (int i=0; i<in; i++) {
                IUIAutomationElement* el = NULL;
                if (SUCCEEDED( list->GetElement(i, &el) ) && el ) {
                    BSTR name = SysAllocStringLen(NULL, MAX_PATH);
                    el->get_CurrentName(&name);
                    if (lstrlen(name)) {
                        stActionDescriptor* app = new stActionDescriptor;
                        *app = m_currentAction;
                        lstrcpyn(app->controlName, name, MAX_PATH);
                        name[lstrlen(name)] = 0;
                        int ind = m_cmbSelectControl.AddString(name);
                        el->get_CurrentAutomationId(&name);
                        if (lstrlen(name)) {
                            // apply AutomationId as data
                            lstrcpyn(app->automationId, name, MAX_PATH);
                            name[lstrlen(name)] = 0;
                        } else {
                            // apply RuntimeId as data
                            SAFEARRAY *sa = NULL;
                            el->GetRuntimeId(&sa);
                            app->rtType = 0;
                            app->rtCounter = sa->cbElements;
                            int* list = (int*)sa->pvData;
                            for (int i=0; i<app->rtCounter; i++) {
                                app->runtimeId[i] = list[i];
                            }
                        }
                        m_cmbSelectControl.SetItemDataPtr(ind, app);
                        m_currentAction = *app;
                    }
                    SysFreeString(name);
                }
            }
            list->Release();
        }
    }
    // return the dialog window
    ::SetCursorPos(curentPos.x, curentPos.y);
}

void CDlgNewAction::FindAvailableCheckers()
{
    ReleaseComboBox(&m_cmbSelectControl);
    m_cmbSelectControl.AddString(L" < select a check box >");
    m_cmbSelectControl.SetCurSel(0);
    FindAvailableInvoker(UIA_CheckBoxControlTypeId);
}

void CDlgNewAction::FindAvailableButtons()
{
    ReleaseComboBox(&m_cmbSelectControl);
    m_cmbSelectControl.AddString(L" < select a button >");
    m_cmbSelectControl.SetCurSel(0);
    FindAvailableInvoker(UIA_ButtonControlTypeId);
}

void CDlgNewAction::OnBnClickedCheckStyle()
{
    UpdateData();
    FindAvailableApps();
}

void CDlgNewAction::OnCbnSelchangeComboSelectApp()
{
    int ind = m_cmbSelectApp.GetCurSel();
    stActionDescriptor* app = (stActionDescriptor*)m_cmbSelectApp.GetItemDataPtr(ind);
    if (ind) {
        StringCchCopy(m_currentAction.className, MAX_PATH, app->className);
        StringCchCopy(m_currentAction.windowName, MAX_PATH, app->windowName);
        m_currentAction.specific = app->specific;
        FindAvailableTypes();
        m_cmbSelectType.ShowWindow(SW_SHOW);
        m_cmbSelectType.SetFocus();
    } else {
        m_cmbSelectType.ShowWindow(SW_HIDE);
        m_cmbSelectControl.ShowWindow(SW_HIDE);
    }
}

void CDlgNewAction::OnBnClickedCancel()
{
    m_currentAction.isActual = false;
    ReleaseComboBox(&m_cmbSelectControl);
    ReleaseComboBox(&m_cmbSelectType);
    ReleaseComboBox(&m_cmbSelectApp);
    CDialogEx::OnCancel();
}

void CDlgNewAction::OnCbnSelchangeComboActionType()
{
    ReleaseComboBox(&m_cmbSelectControl);
    int type = (int)m_cmbSelectType.GetItemData(m_cmbSelectType.GetCurSel());
    switch(type) {
    case stActionDescriptor::MY_AT_NAVIGATION: // windows navigation
        FindAvailableNavigation();
        break;
    case stActionDescriptor::MY_AT_PRINT: // print event
        FindAvailablePrint();
        break;
    case stActionDescriptor::MY_AT_KEYBOARD: // keyboard event
        FindAvailableKeyboardEvents();
        break;
    case stActionDescriptor::MY_AT_BUTTON: // invoke button
        FindAvailableButtons();
        break;
    default:
        m_cmbSelectControl.ShowWindow(SW_HIDE);
        return;
    }
    m_cmbSelectControl.ShowWindow(SW_SHOW);
    m_cmbSelectControl.SetFocus();
}

void CDlgNewAction::OnCbnSelchangeComboSelectControl()
{
    m_isAdded = false;
    m_currentAction.isActual = false;
    m_hotKeyInfo.ShowWindow(SW_HIDE);
    m_hotPressKey.ShowWindow(SW_HIDE);

    // fill action descriptor
    int appl = m_cmbSelectApp.GetCurSel();
    int type = (int)m_cmbSelectType.GetItemData(m_cmbSelectType.GetCurSel());
    int cntr = m_cmbSelectControl.GetCurSel();

    // check completeness
    if (appl<1 || type<1 || cntr<1) return;
    stActionDescriptor* app = (stActionDescriptor*)m_cmbSelectApp.GetItemDataPtr(appl);
    lstrcpyn(m_currentAction.windowName, app->windowName, MAX_PATH);
    lstrcpyn(m_currentAction.className, app->className, MAX_PATH);
    
    stActionDescriptor* id = NULL;
    switch (type) {
    case stActionDescriptor::MY_AT_PRINT: // print text
        m_currentAction.type = stActionDescriptor::MY_AT_PRINT;
        m_currentAction.system_type = (stActionDescriptor::SYSTEM_TYPE)m_cmbSelectControl.GetItemData(cntr);
        m_currentAction.isActual = true;
        break;
    case stActionDescriptor::MY_AT_NAVIGATION: // windows navigation
        m_currentAction.type = stActionDescriptor::MY_AT_NAVIGATION;
        m_currentAction.navigate_type = (stActionDescriptor::NAVIGATION_TYPE)m_cmbSelectControl.GetItemData(cntr);
        m_currentAction.isActual = true;
        break;
    case stActionDescriptor::MY_AT_KEYBOARD: // keyboard event
        m_currentAction.type = stActionDescriptor::MY_AT_KEYBOARD;
        m_currentAction.key_type = (stActionDescriptor::KEYBOARD_TYPE)m_cmbSelectControl.GetItemData(cntr);
        if (m_currentAction.key_type == stActionDescriptor::MY_KT_PRESS) {
            m_hotKeyInfo.ShowWindow(SW_SHOW);
            m_hotPressKey.ShowWindow(SW_SHOW);
            m_hotPressKey.SetFocus();
        }
        m_currentAction.key_code = 0;
        m_currentAction.isActual = true;
        break;
    case stActionDescriptor::MY_AT_BUTTON: // invoke button
        m_currentAction.type = stActionDescriptor::MY_AT_BUTTON;
        id = (stActionDescriptor*)m_cmbSelectControl.GetItemDataPtr(cntr);
        lstrcpyn(m_currentAction.controlName, id->controlName, MAX_PATH);
        if (id->rtType) {
            lstrcpyn(m_currentAction.automationId, id->automationId, MAX_PATH);
        } else {
            m_currentAction.rtType = id->rtType;
            m_currentAction.rtCounter = id->rtCounter;
            for (int i=0; i<m_currentAction.rtCounter; i++) {
                m_currentAction.runtimeId[i] = id->runtimeId[i];
            }
        }
        m_currentAction.isActual = true;
        break;
    }
    PrintActionInfo(&m_currentAction, &m_infoBoard);
}

void CDlgNewAction::PrintActionInfo(stActionDescriptor* descr, CStatic* infoBoard)
{
    if (!infoBoard) return; //TODO

    if (!descr) {
        infoBoard->SetWindowText(_T(""));
        return;
    }

    // check the current action
    if (!descr->isActual) {
        infoBoard->SetWindowText(_T("* incomplete action *"));
        return;
    }

    CString str;
    if (descr->isGrouping) {
        str.Append(_T("Application: "));
    } else {
        str.Append(_T("Window: "));
    }
    str.Append(descr->windowName);
    str.Append(_T("\nClass: "));
    str.Append(descr->className);
    switch (descr->type) {
    case stActionDescriptor::MY_AT_PRINT:
        str.Append(_T("\nPrint text:"));
        switch (descr->system_type) {
        case stActionDescriptor::MY_PT_PRINT_EVENT:
            str.Append(_T("\n + Event Name"));
            break;
        case stActionDescriptor::MY_PT_PRINT_EVENT_ENTER:
            str.Append(_T("\n + Event Name + Return"));
            break;
        case stActionDescriptor::MY_PT_PRINT_NUMBER:
            str.Append(_T("\n + Numbers"));
            break;
        }
        break;
    case stActionDescriptor::MY_AT_NAVIGATION:
        str.Append(_T("\nAction: Navigation"));
        switch (descr->navigate_type) {
        case stActionDescriptor::MY_NT_TOP:
            str.Append(_T("\n + Bring window to Top"));
            break;
        case stActionDescriptor::MY_NT_MAX:
            str.Append(_T("\n + Maximize window"));
            break;
        case stActionDescriptor::MY_NT_MIN:
            str.Append(_T("\n + Minimize window"));
            break;
        }
        break;
    case stActionDescriptor::MY_AT_KEYBOARD:
        str.Append(_T("\nAction: Keyboard event"));
        switch (descr->key_type) {
        case stActionDescriptor::MY_KT_ESC:
            str.Append(_T("\n + <Escape>"));
            break;
        case stActionDescriptor::MY_KT_DEL:
            str.Append(_T("\n + <Delete>"));
            break;
        case stActionDescriptor::MY_KT_PRESS:
            {
                str.Append(L"\n + Press key ");
                descr->key_code = m_hotPressKey.GetHotKey();
                if (descr->key_code) {
                    WCHAR ss[MAX_PATH];
                    str.Append(L" [ ");
                    str.Append(m_hotPressKey.GetHotKeyName());
                    StringCchPrintf(ss, MAX_PATH, L" ] (0x%X)",descr->key_code, descr->key_code);
                    str.Append(ss);
                    // apply key name
                    StringCchPrintf(descr->key_code_name, 20, L"%s", m_hotPressKey.GetHotKeyName());
                }
            }
            break;
        case stActionDescriptor::MY_KT_ENTER:
            str.Append(_T("\n + <Enter>"));
            break;
        case stActionDescriptor::MY_KT_SPACE:
            str.Append(_T("\n + <Space>"));
            break;
        case stActionDescriptor::MY_KT_BKSPACE:
            str.Append(_T("\n + <BackSpace>"));
            break;
        case stActionDescriptor::MY_KT_TAB:
            str.Append(_T("\n + <Tab>"));
            break;
        case stActionDescriptor::MY_KT_WIN:
            str.Append(_T("\n + <WIN key>"));
            break;
        case stActionDescriptor::MY_KT_ALT_PRESS:
            str.Append(_T("\n + <Alt> press"));
            break;
        case stActionDescriptor::MY_KT_ALT_RELEASE:
            str.Append(_T("\n + <Alt> release"));
            break;
        }
        break;
    case stActionDescriptor::MY_AT_BUTTON:
        str.Append(_T("\nAction: Invoke Button"));
        str.Append(_T("\n + name: "));
        str.Append(descr->controlName);
        if (descr->rtType==0 && descr->rtCounter) {
            WCHAR tmp[MAX_PATH];
            str.Append(_T("\n + rtid: [ "));
            for (int i=0; i<descr->rtCounter; i++) {
                if (i==descr->rtCounter-1) {
                    StringCchPrintf(tmp, MAX_PATH, L"0x%X ]", descr->runtimeId[i]);
                } else {
                    StringCchPrintf(tmp, MAX_PATH, L"0x%X,", descr->runtimeId[i]);
                }
                str.Append(tmp);
            }
        } else {
            str.Append(_T("\n + auid: "));
            str.Append(descr->automationId);
        }
        break;
    }
    infoBoard->SetWindowText(str);
}

void CDlgNewAction::OnBnClickedButtonTry()
{
    PrintActionInfo(&m_currentAction, &m_infoBoard);
    g_Invoker.ExecuteAction(&m_currentAction, TRUE);
}


void CDlgNewAction::OnBnClickedButtonAddToList()
{
    // fill action
    PrintActionInfo(&m_currentAction, &m_infoBoard);
    if (m_actionList && m_currentAction.isActual ) {
        int id = g_Dispatcher.AddAction(&m_currentAction);
        m_actionList->AppendAction(&m_currentAction, id);
        m_isAdded = true;
    }
}


void CDlgNewAction::OnBnClickedApply()
{
    // fill action
    PrintActionInfo(&m_currentAction, &m_infoBoard);
    if (m_currentAction.isActual ) {
        if (m_actionList) {
            if (!m_isAdded) {
                int id = g_Dispatcher.AddAction(&m_currentAction);
                m_actionList->AppendAction(&m_currentAction, id);
            }
        }
        ReleaseComboBox(&m_cmbSelectControl);
        ReleaseComboBox(&m_cmbSelectType);
        ReleaseComboBox(&m_cmbSelectApp);
        CDialogEx::OnOK();
    }
}


BOOL CDlgNewAction::OnHelpInfo(HELPINFO* pHelpInfo)
{
    return TRUE; // disable due to catch F1 key 
}


void CDlgNewAction::OnBnClickedCheckGroup()
{
    UpdateData();
    FindAvailableApps();
}
