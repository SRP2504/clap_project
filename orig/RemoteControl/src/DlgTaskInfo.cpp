/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/

#include "stdafx.h"
#include "RemoteControl.h"
#include "DlgTaskInfo.h"
#include "afxdialogex.h"
#include "dispatcher.h"
#include "invoker.h"

extern CDispatcher g_Dispatcher;
extern CInvoker g_Invoker;

// CDlgTaskInfo dialog

IMPLEMENT_DYNAMIC(CDlgTaskInfo, CDialogEx)

CDlgTaskInfo::CDlgTaskInfo(CWnd* pParent /*=NULL*/)
    : CDialogEx(CDlgTaskInfo::IDD, pParent)
    , m_type(0)
{

    EnableAutomation();
}

CDlgTaskInfo::~CDlgTaskInfo()
{
}

void CDlgTaskInfo::OnFinalRelease()
{
    CDialogEx::OnFinalRelease();
}

void CDlgTaskInfo::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TASK_TREE, m_taskTree);
    DDX_Control(pDX, IDC_RADIO_SENS, m_btnSens);
    DDX_Radio(pDX, IDC_RADIO_SENS, m_type);
    DDV_MinMaxInt(pDX, m_type, 0, 1);
}

BEGIN_MESSAGE_MAP(CDlgTaskInfo, CDialogEx)
    ON_WM_SIZE()
    ON_WM_CREATE()
    ON_BN_CLICKED(IDC_RADIO_APPS, &CDlgTaskInfo::OnBnClickedRadioApps)
    ON_BN_CLICKED(IDC_RADIO_SENS, &CDlgTaskInfo::OnBnClickedRadioSens)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CDlgTaskInfo, CDialogEx)
END_DISPATCH_MAP()

static const IID IID_IDlgTaskInfo =
{ 0x2F28FC6D, 0xAC7F, 0x45FA, { 0xA5, 0x15, 0x4, 0x9C, 0xAC, 0xF5, 0x61, 0x13 } };

BEGIN_INTERFACE_MAP(CDlgTaskInfo, CDialogEx)
    INTERFACE_PART(CDlgTaskInfo, IID_IDlgTaskInfo, Dispatch)
END_INTERFACE_MAP()

// CDlgTaskInfo message handlers
void CDlgTaskInfo::SetSize()
{
    RECT rect, rect1;
    m_btnSens.GetClientRect(&rect);
    GetClientRect(&rect1);
    int x = 0;
    int y = rect.bottom;
    int dx = rect1.right - x;
    int dy = rect1.bottom - y;
    m_taskTree.SetWindowPos(NULL, x, y, dx, dy, SWP_SHOWWINDOW);
}

void CDlgTaskInfo::OnSize(UINT nType, int cx, int cy)
{
    CDialogEx::OnSize(nType, cx, cy);
    if (IsWindow(m_taskTree.m_hWnd)) {
        SetSize();
    }
}

int CDlgTaskInfo::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDialogEx::OnCreate(lpCreateStruct) == -1)
        return -1;
    return 0;
}

BOOL CDlgTaskInfo::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    SetSize();
    CreateSensorTree();
    return TRUE;
}

void CDlgTaskInfo::CreateSensorTree()
{
    m_taskTree.DeleteAllItems();

    HTREEITEM root = m_taskTree.InsertItem(L"sensors");
    for (int i=0; i<MAX_SENSORS; i++) {
        CSensor* sensor = g_Dispatcher.GetSensor(i);
        if (sensor) {
            HTREEITEM item = m_taskTree.InsertItem(sensor->GetName(), root);
            st_eventDescriptor event;
            for (int j=0; ; j++) {
                if (!sensor->GetEvent(j, &event)) break;
                HTREEITEM item2 = m_taskTree.InsertItem(event.name, item);
                for (int k=0; k<g_Dispatcher.TaskCount(); k++) {
                    stActionDescriptor* action = g_Dispatcher.GetTask(k);
                    if (action) {
                        if ((action->sensorId==sensor->GetId()) && (action->eventId==event.id)) {
                            CString str;
                            action->GetTitle(str);
                            HTREEITEM item3 = m_taskTree.InsertItem(str, item2);
                            if (action->isEnable) {
                                m_taskTree.SetItemState(item3, TVIS_BOLD, TVIS_BOLD);
                            }
                        }
                    }
                }
                m_taskTree.Expand(item2, TVE_EXPAND );
            }
            m_taskTree.Expand(item, TVE_EXPAND );
        }
    }
    m_taskTree.Expand(root, TVE_EXPAND );
}

void CDlgTaskInfo::CreateApplicationTree()
{
    m_taskTree.DeleteAllItems();

    HTREEITEM root = m_taskTree.InsertItem(L"applications");

    // enumerate all applications
    std::vector<CString> nameList;
    for (int i=0; i<g_Dispatcher.TaskCount(); i++) {
        stActionDescriptor* action = g_Dispatcher.GetTask(i);
        if (action) {
            CString name;
            action->GetTitle(name, 1, 0, 0);
            for (int j=0; j<(int)nameList.size(); j++) {
                if (name.Compare(nameList.at(j))==0) {
                    action = NULL;
                    break;
                }
            }
            if (action) nameList.push_back(name);
        }
    }

    // fill tree
    HTREEITEM item;
    for (int i=0; i<(int)nameList.size(); i++) {
        item = m_taskTree.InsertItem(nameList.at(i), root);
        // enumerate all types
        std::vector<int> typeList;
        for (int k=0; k<g_Dispatcher.TaskCount(); k++) {
            stActionDescriptor* action = g_Dispatcher.GetTask(k);
            if (!action) continue;
            CString name;
            action->GetTitle(name, 1, 0, 0);
            if (name.Compare(nameList.at(i))==0) {
                for (int j=0; j<(int)typeList.size(); j++) {
                    if ((int)action->type == typeList.at(j)) {
                        action = NULL;
                        break;
                    }
                }
                if (action) typeList.push_back((int)action->type);
            }
        }
        HTREEITEM item2 = NULL;
        for (int j=0; j<(int)typeList.size(); j++) {
            CString name;
            switch (typeList.at(j)) {
            case stActionDescriptor::MY_AT_NONE:        continue;
            case stActionDescriptor::MY_AT_NAVIGATION:  name = L"Navigation";       break;
            case stActionDescriptor::MY_AT_KEYBOARD:    name = L"Keyboard event";   break;
            case stActionDescriptor::MY_AT_PRINT:       name = L"Print";            break;
            case stActionDescriptor::MY_AT_BUTTON:      name = L"Push button";      break;
            }
            item2 = m_taskTree.InsertItem(name, item);
            if (!item2) continue;
            for (int k=0; k<g_Dispatcher.TaskCount(); k++) {
                stActionDescriptor* action = g_Dispatcher.GetTask(k);
                if (!action) continue;
                CString name;
                action->GetTitle(name, 1, 0, 0);
                if (name.Compare(nameList.at(i))==0) {
                    if ((int)action->type == typeList.at(j)) {
                        CString title;
                        action->GetTitle(title, 0, 0, 1);
                        // check existance
                        HTREEITEM item3 = m_taskTree.GetChildItem(item2);
                        while (item3 != NULL) {
                            if (title.Compare(m_taskTree.GetItemText(item3))==0) break;
                            item3 = m_taskTree.GetNextItem(item3, TVGN_NEXT);
                        }
                        if (item3==NULL) {
                            // create new
                            item3 = m_taskTree.InsertItem(title, item2);
                        }
                        CString sensorTitle;
                        action->GetSensorTitle(sensorTitle, 0);
                        HTREEITEM item4 = m_taskTree.InsertItem(sensorTitle, item3);
                        if (action->isEnable) {
                            m_taskTree.SetItemState(item4, TVIS_BOLD, TVIS_BOLD);
                        }
                        m_taskTree.Expand(item3, TVE_EXPAND );
                    }
                }
            }
            if (item2) m_taskTree.Expand(item2, TVE_EXPAND );
        }
        if (item) m_taskTree.Expand(item, TVE_EXPAND );
    }
    m_taskTree.Expand(root, TVE_EXPAND );
}

void CDlgTaskInfo::OnBnClickedRadioSens()
{
    CreateSensorTree();
}

void CDlgTaskInfo::OnBnClickedRadioApps()
{
    CreateApplicationTree();
}
