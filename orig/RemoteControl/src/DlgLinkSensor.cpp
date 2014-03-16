/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/

#include "stdafx.h"
#include "RemoteControl.h"
#include "DlgLinkSensor.h"
#include "Dispatcher.h"

#include "afxdialogex.h"

extern CDispatcher g_Dispatcher;

IMPLEMENT_DYNAMIC(CDlgLinkSensor, CDialogEx)

CDlgLinkSensor::CDlgLinkSensor(stActionDescriptor* descr, CWnd* pParent /*=NULL*/)
    : CDialogEx(CDlgLinkSensor::IDD, pParent)
    , m_enable(FALSE)
    , m_strNewCmd(_T(""))
{
    EnableAutomation();
    m_actionDescr = descr;
    m_sensorInd = 0;
    m_eventInd = 0;
    m_enable = descr->isEnable;
}

CDlgLinkSensor::~CDlgLinkSensor()
{
}

void CDlgLinkSensor::OnFinalRelease()
{
    CDialogEx::OnFinalRelease();
}

void CDlgLinkSensor::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_STATIC_ACTION_INFO, m_actionInfo);
    DDX_Control(pDX, IDC_COMBO_SELECT_SENSOR, m_cmbSelectSensor);
    DDX_Control(pDX, IDC_COMBO_SELECT_EVENT, m_cmbSelectEvent);
    DDX_Check(pDX, IDC_CHECK_ENABLE, m_enable);
    DDX_Control(pDX, IDC_CHECK_ENABLE, m_chkEnable);
    DDX_Control(pDX, IDC_STATIC_NEW_CMD, m_infoNewCmd);
    DDX_Text(pDX, IDC_EDIT_NEW_CMD, m_strNewCmd);
    DDX_Control(pDX, IDC_EDIT_NEW_CMD, m_editNewCmd);
    DDX_Control(pDX, IDC_BUTTON_APPLY, m_btnNewCmd);
}

void CDlgLinkSensor::PrintActionInfo(stActionDescriptor* descr, CStatic* infoBoard)
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
        str.Append(_T("\n[ Print ]"));
        switch (descr->system_type) {
        case stActionDescriptor::MY_PT_PRINT_EVENT:
            str.Append(_T(" : Event Name"));
            break;
        case stActionDescriptor::MY_PT_PRINT_EVENT_ENTER:
            str.Append(_T(" : Event Name + Return"));
            break;
        case stActionDescriptor::MY_PT_PRINT_NUMBER:
            str.Append(_T(" : Numbers"));
            break;
        }
        break;
    case stActionDescriptor::MY_AT_NAVIGATION:
        str.Append(_T("\n[ Navigation ]"));
        switch (descr->navigate_type) {
        case stActionDescriptor::MY_NT_TOP:
            str.Append(_T(" : Bring window to Top"));
            break;
        case stActionDescriptor::MY_NT_MAX:
            str.Append(_T(" : Maximize window"));
            break;
        case stActionDescriptor::MY_NT_MIN:
            str.Append(_T(" : Minimize window"));
            break;
        }
        break;
    case stActionDescriptor::MY_AT_KEYBOARD:
        str.Append(_T("\n[ Keyboard event ]"));
        switch (descr->key_type) {
        case stActionDescriptor::MY_KT_ESC:
            str.Append(_T(" : <Escape>"));
            break;
        case stActionDescriptor::MY_KT_DEL:
            str.Append(_T(" : <Delete>"));
            break;
        case stActionDescriptor::MY_KT_PRESS:
            {
                str.Append(L"\n : Press key ");
                if (descr->key_code) {
                    WCHAR ss[MAX_PATH];
                    str.Append(L" [ ");
                    str.Append(CHotKeyCtrl::GetKeyName(descr->key_code, TRUE));
                    str.Append(descr->key_code_name);
                    wsprintf(ss, L" ] (0x%X)",descr->key_code, descr->key_code);
                    str.Append(ss);
                }
            }
            break;
        case stActionDescriptor::MY_KT_ENTER:
            str.Append(_T("\n : <Enter>"));
            break;
        case stActionDescriptor::MY_KT_SPACE:
            str.Append(_T("\n : <Space>"));
            break;
        case stActionDescriptor::MY_KT_BKSPACE:
            str.Append(_T("\n : <BackSpace>"));
            break;
        case stActionDescriptor::MY_KT_TAB:
            str.Append(_T("\n : <Tab>"));
            break;
        case stActionDescriptor::MY_KT_WIN:
            str.Append(_T("\n : <WIN key>"));
            break;
        case stActionDescriptor::MY_KT_ALT_PRESS:
            str.Append(_T("\n : <Alt> press"));
            break;
        case stActionDescriptor::MY_KT_ALT_RELEASE:
            str.Append(_T("\n : <Alt> release"));
            break;
        }
        break;
    case stActionDescriptor::MY_AT_BUTTON:
        str.Append(_T("\n[ Invoke Button ]"));
        str.Append(_T("\n + name: "));
        str.Append(descr->controlName);
        if (descr->rtType==0 && descr->rtCounter) {
            WCHAR tmp[MAX_PATH];
            str.Append(_T("\n + rtid: [ "));
            for (int i=0; i<descr->rtCounter; i++) {
                if (i==descr->rtCounter-1) wsprintf(tmp, L"0x%X ]", descr->runtimeId[i]);
                else wsprintf(tmp, L"0x%X,", descr->runtimeId[i]);
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


BEGIN_MESSAGE_MAP(CDlgLinkSensor, CDialogEx)
    ON_WM_CREATE()
    ON_CBN_SELCHANGE(IDC_COMBO_SELECT_SENSOR, &CDlgLinkSensor::OnCbnSelchangeComboSelectSensor)
    ON_CBN_SELCHANGE(IDC_COMBO_SELECT_EVENT, &CDlgLinkSensor::OnCbnSelchangeComboSelectEvent)
    ON_BN_CLICKED(IDOK, &CDlgLinkSensor::OnBnClickedOk)
    ON_BN_CLICKED(IDC_CHECK_ENABLE, &CDlgLinkSensor::OnBnClickedCheckEnable)
    ON_BN_CLICKED(IDC_BUTTON_APPLY, &CDlgLinkSensor::OnBnClickedButtonApply)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CDlgLinkSensor, CDialogEx)
END_DISPATCH_MAP()

static const IID IID_IDlgLinkSensor =
{ 0x41E08779, 0x450D, 0x4B9B, { 0x91, 0x21, 0x84, 0x5, 0xA, 0x46, 0x35, 0xAC } };

BEGIN_INTERFACE_MAP(CDlgLinkSensor, CDialogEx)
    INTERFACE_PART(CDlgLinkSensor, IID_IDlgLinkSensor, Dispatch)
END_INTERFACE_MAP()

BOOL CDlgLinkSensor::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    PrintActionInfo(m_actionDescr, &m_actionInfo);

    m_cmbSelectSensor.AddString(L" < select a sensor >");
    m_cmbSelectSensor.SetCurSel(0);
    m_sensorInd = 0;
    for (int i=0; i<MAX_SENSORS; i++) {
        CSensor* sens = g_Dispatcher.GetSensor(i);
        if (sens) {
            m_cmbSelectSensor.AddString(sens->GetName());
            if (sens->GetId() == m_actionDescr->sensorId) m_sensorInd = i+1;
        }
    }
    m_cmbSelectSensor.SetCurSel(m_sensorInd);
    if (m_sensorInd) {
        OnCbnSelchangeComboSelectSensor();
    }
    m_chkEnable.SetCheck(m_enable);
    return TRUE;
}

void CDlgLinkSensor::OnCbnSelchangeComboSelectSensor()
{
    m_cmbSelectEvent.ShowWindow(SW_HIDE);

    for (int i=m_cmbSelectEvent.GetCount()-1; i>=0; i--) {
        m_cmbSelectEvent.DeleteString(i);
    }

    int sel = m_cmbSelectSensor.GetCurSel();
    if (sel!=m_sensorInd) m_eventInd = 0;
    if (sel) {
        CSensor* sens = g_Dispatcher.GetSensor(sel-1);
        if (sens) {
            m_cmbSelectEvent.ShowWindow(SW_SHOW);
            if (sens->GetId() == CSensor::Sensor_Voice_recognition) {
                m_cmbSelectEvent.AddString(L" < set a voice command >");
                m_cmbSelectEvent.AddString(L" :: create new command ::");
                st_eventDescriptor descr;
                for (int i=0; sens->GetEvent(i, &descr); i++) {
                    m_cmbSelectEvent.AddString(descr.name);
                    if (descr.id == m_actionDescr->eventId) m_eventInd = i+1;
                }
                if (m_eventInd<2) m_cmbSelectEvent.SetCurSel(0);
                else m_cmbSelectEvent.SetCurSel(m_eventInd+1);
            } else {
                m_cmbSelectEvent.AddString(L" < select an event >");
                st_eventDescriptor descr;
                for (int i=0; sens->GetEvent(i, &descr); i++) {
                    m_cmbSelectEvent.AddString(descr.name);
                    if (descr.id == m_actionDescr->eventId) m_eventInd = i+1;
                }
                m_cmbSelectEvent.SetCurSel(m_eventInd);
            }
        }
    }
    m_sensorInd = sel;
}

void CDlgLinkSensor::OnCbnSelchangeComboSelectEvent()
{
    m_infoNewCmd.ShowWindow(SW_HIDE);
    m_editNewCmd.ShowWindow(SW_HIDE);
    m_btnNewCmd.ShowWindow(SW_HIDE);
    int sel = m_cmbSelectSensor.GetCurSel();
    if (sel) {
        CSensor* sens = g_Dispatcher.GetSensor(sel-1);
        if (sens && sens->GetId() == CSensor::Sensor_Voice_recognition) {
            sel = m_cmbSelectEvent.GetCurSel();
            if (sel == 1) {
                m_infoNewCmd.ShowWindow(SW_SHOW);
                m_editNewCmd.ShowWindow(SW_SHOW);
                m_btnNewCmd.ShowWindow(SW_SHOW);
                m_editNewCmd.SetFocus();
                m_editNewCmd.SetWindowText(L"");
                m_enable = false;
                m_chkEnable.SetCheck(m_enable);
                return;
            }
        }
    }

    m_eventInd = m_cmbSelectEvent.GetCurSel()-1;
    m_enable = true;
    m_chkEnable.SetCheck(m_enable);
}


void CDlgLinkSensor::OnBnClickedOk()
{
    // apply results
    m_eventInd = m_cmbSelectEvent.GetCurSel()-1;
    if (m_sensorInd && (m_eventInd>=0)) {
        CSensor* sens = g_Dispatcher.GetSensor(m_sensorInd-1);
        if (sens) {
            m_actionDescr->sensorId = sens->GetId();
            if ( m_actionDescr->sensorId == CSensor::Sensor_Voice_recognition) m_eventInd--;
            m_actionDescr->eventId = sens->GetEventId(m_eventInd);
            m_actionDescr->isEnable = m_enable;
        }
    } else m_actionDescr->isEnable = false;

    CDialogEx::OnOK();
}


void CDlgLinkSensor::OnBnClickedCheckEnable()
{
    m_enable = m_chkEnable.GetCheck();
    m_infoNewCmd.ShowWindow(SW_HIDE);
    m_editNewCmd.ShowWindow(SW_HIDE);
    m_btnNewCmd.ShowWindow(SW_HIDE);
}


void CDlgLinkSensor::OnBnClickedButtonApply()
{
    // create the new voice command
    m_infoNewCmd.ShowWindow(SW_HIDE);
    m_editNewCmd.ShowWindow(SW_HIDE);
    m_btnNewCmd.ShowWindow(SW_HIDE);

    m_editNewCmd.GetWindowText(m_strNewCmd);
    int sel = m_cmbSelectSensor.GetCurSel();
    if (sel) {
        CSensor* sens = g_Dispatcher.GetSensor(sel-1);
        if (sens && sens->GetId() == CSensor::Sensor_Voice_recognition) {
            sel = m_cmbSelectEvent.SelectString(0, m_strNewCmd.GetBuffer());
            if (sel<0) {
                int len = m_strNewCmd.GetLength();
                if ((m_strNewCmd.GetLength()==0) || (m_strNewCmd.GetBuffer()[0]==0) || (m_strNewCmd.GetBuffer()[0]==L' ')) {
                    // don't use blank command
                    m_cmbSelectEvent.SetCurSel(0);
                    m_enable = FALSE;
                    m_chkEnable.SetCheck(0);
                    OnCbnSelchangeComboSelectSensor();
                    return;
                } else {
                    sens->AddEvent(m_strNewCmd.GetBuffer());
                    sel = m_cmbSelectEvent.AddString(m_strNewCmd.GetBuffer());
                    m_cmbSelectEvent.SetCurSel(sel);
                }
            }
            OnCbnSelchangeComboSelectEvent();
        }
    }
}
