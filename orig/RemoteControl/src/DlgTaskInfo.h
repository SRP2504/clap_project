/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/

#pragma once
#include "afxcmn.h"
#include "afxcview.h"

class CDlgTaskInfo : public CDialogEx
{
    DECLARE_DYNAMIC(CDlgTaskInfo)

public:
    CDlgTaskInfo(CWnd* pParent = NULL);
    virtual ~CDlgTaskInfo();
    void SetSize();
    void CreateSensorTree();
    void CreateApplicationTree();

    virtual void OnFinalRelease();
    virtual BOOL OnInitDialog();

    enum { IDD = IDD_DIALOG_TASKINFO };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);

    DECLARE_MESSAGE_MAP()
    DECLARE_DISPATCH_MAP()
    DECLARE_INTERFACE_MAP()
public:
    afx_msg void OnSize(UINT nType, int cx, int cy);
    CTreeCtrl  m_taskTree;
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    CButton m_btnSens;
    int m_type;
    afx_msg void OnBnClickedRadioApps();
    afx_msg void OnBnClickedRadioSens();
};
