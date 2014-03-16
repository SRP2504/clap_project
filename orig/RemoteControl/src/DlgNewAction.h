/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/

#pragma once
#include "afxwin.h"
#include <UIAutomationClient.h>
#include "afxcmn.h"

#include "Dispatcher.h"
#include "RemoteControlDlg.h"

class CDlgNewAction : public CDialogEx
{
    DECLARE_DYNAMIC(CDlgNewAction)

public:
    CDlgNewAction(CWnd* pParent = NULL);
    virtual ~CDlgNewAction();

    enum { IDD = IDD_DIALOG_NEW_ACTION };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);

    void ReleaseComboBox(CComboBox *cmb);
    void FindAvailableApps();
    void FindAvailableTypes();
    void FindAvailableNavigation();
    void FindAvailableKeyboardEvents();
    void FindAvailableButtons();
    void FindAvailableCheckers();
    void FindAvailableInvoker(PROPERTYID propertyID);
    void FindAvailablePrint();

    void PrintActionInfo(stActionDescriptor* descr, CStatic* infoBoard);

    DECLARE_MESSAGE_MAP()
public:
    CComboBox m_cmbSelectApp;
    CComboBox m_cmbSelectType;
    CComboBox m_cmbSelectControl;
    stActionDescriptor m_currentAction;
    BOOL m_bGrouping;
    class CMyList* m_actionList;

    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedCheckStyle();
    afx_msg void OnCbnSelchangeComboSelectApp();
    afx_msg void OnBnClickedApply();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedButtonTry();
    afx_msg void OnCbnSelchangeComboActionType();
    afx_msg void OnCbnSelchangeComboSelectControl();
    afx_msg void OnBnClickedButtonAddToList();
    CStatic m_infoBoard;
    CHotKeyCtrl m_hotPressKey;
    CStatic m_hotKeyInfo;
    BOOL    m_isAdded;
    afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
    CButton m_chkGroup;
    afx_msg void OnBnClickedCheckGroup();
    CButton m_btnTryAction;
    CButton m_btnAddToList;
    CButton m_btnSensor;
};
