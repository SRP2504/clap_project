/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/

#pragma once
#include "Dispatcher.h"
#include "afxwin.h"

class CDlgLinkSensor : public CDialogEx
{
    DECLARE_DYNAMIC(CDlgLinkSensor)

public:
    CDlgLinkSensor(stActionDescriptor* descr, CWnd* pParent = NULL);
    virtual ~CDlgLinkSensor();

    virtual void OnFinalRelease();

    int m_sensorInd;
    int m_eventInd;

    enum { IDD = IDD_DIALOG_LINK_SENSOR };

protected:
    stActionDescriptor* m_actionDescr;
    void PrintActionInfo(stActionDescriptor* descr, CStatic* infoBoard);

    virtual void DoDataExchange(CDataExchange* pDX);

    DECLARE_MESSAGE_MAP()
    DECLARE_DISPATCH_MAP()
    DECLARE_INTERFACE_MAP()
public:
    CStatic m_actionInfo;
    virtual BOOL OnInitDialog();
    afx_msg void OnCbnSelchangeComboSelectSensor();
    CComboBox m_cmbSelectSensor;
    CComboBox m_cmbSelectEvent;
    afx_msg void OnCbnSelchangeComboSelectEvent();
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCheckEnable();
    BOOL m_enable;
    CButton m_chkEnable;
    CStatic m_infoNewCmd;
    CString m_strNewCmd;
    afx_msg void OnBnClickedButtonApply();
    CEdit m_editNewCmd;
    CButton m_btnNewCmd;
};
