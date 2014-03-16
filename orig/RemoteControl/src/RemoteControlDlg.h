/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/

#pragma once
#include "afxvslistbox.h"
#include "Dispatcher.h"
#include "DlgNewAction.h"
#include "afxwin.h"


/////////////////
//  CAboutDlg  //
/////////////////
class CAboutDlg : public CDialogEx
{
public:
    CAboutDlg();
    enum { IDD = IDD_ABOUTBOX };
protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    DECLARE_MESSAGE_MAP()
public:
};


///////////////////
//  CWelcomeDlg  //
///////////////////
#define MAX_HELP_PAGE 4
class CWelcomeDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CWelcomeDlg)
public:
    CWelcomeDlg(CWnd* pParent = NULL);
    virtual ~CWelcomeDlg();
    enum { IDD = IDD_WELCOMEBOX };
protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    DECLARE_MESSAGE_MAP()
public:
    void SetPage(int page);

    afx_msg void OnBnClickedOk();
    BOOL m_enable;
    CStatic m_welcomeImage;
    virtual BOOL OnInitDialog();
    CButton m_btnNext;
    CButton m_btnPrev;
    CStatic m_staticPage;
    afx_msg void OnBnClickedButtonPrev();
    afx_msg void OnBnClickedButtonNext();
    int m_page;
    HBITMAP m_bitmap[MAX_HELP_PAGE];
    CButton m_btnHelp;
    CButton m_btnContinue;
    CButton m_chkAgain;
};

///////////////
//  CMyList  //
///////////////
class CMyList : public CVSListBox
{
public:
    CMyList() : m_strToolTip(_T("")), m_pToolTip(NULL) {};
    virtual ~CMyList() {
        CVSListBox::DestroyWindow();
        if (m_pToolTip) {CTooltipManager::DeleteToolTip(m_pToolTip); m_pToolTip=NULL;}
    }
    virtual int AddItem(const CString& strIext, DWORD_PTR dwData = 0, int iIndex = -1);
    virtual BOOL EditItem(int iIndex);
    virtual BOOL RemoveItem(int iIndex);
    virtual CWnd* OnCreateList();
    virtual void OnClickButton(int iButton);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
    int  AppendAction(stActionDescriptor* descr, int id);
    int  EditItem(stActionDescriptor* descr, int id);
    int  LinkSensor(stActionDescriptor* descr);
    void RemoveAllItems();
    int  FillTaskList();
    BOOL UpdateTips(MSG* pMsg);

    CString m_strToolTip;
    CToolTipCtrl* m_pToolTip;
};


/////////////////////////
//  CRemoteControlDlg  //
/////////////////////////
class CRemoteControlDlg : public CDialogEx
{
public:
    CRemoteControlDlg(CWnd* pParent = NULL);
    virtual ~CRemoteControlDlg() {
        if (m_ToolTip) delete m_ToolTip;
        m_ToolTip = NULL;
    }

    enum { IDD = IDD_REMOTECONTROL_DIALOG };
protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    HICON m_hIcon;
    int m_isSensorsRun;
    CWelcomeDlg* m_welcome;

    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()
public:
    void    UpdateActionList();
    void    OpenWelcome(BOOL enable=FALSE);
    void    CloseWelcome();
    void    Release();
    BOOL    m_bDisableWelcome;
    CToolTipCtrl* m_ToolTip;

    CMyList m_cntrlActionList;
    afx_msg void OnBnClickedButtonStart();
    afx_msg void OnBnClickedOk();
    CButton m_btnStart;
    BOOL m_isVideoControl;
    CButton m_chkVideoControl;
    CButton m_btnAutoRun;
    afx_msg void OnUpdateSensorVideocontrol(CCmdUI *pCmdUI);
    afx_msg void OnFileNew();
    afx_msg void OnFileLoad();
    afx_msg void OnFileSave();
    afx_msg void OnSensorsShowtasklist();
    afx_msg void OnFileAddfromfile();
    afx_msg void OnUpdateSelectGesturerecognition(CCmdUI *pCmdUI);
    afx_msg void OnUpdateSelectFacerecognition(CCmdUI *pCmdUI);
    afx_msg void OnUpdateSelectVoicerecognition(CCmdUI *pCmdUI);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnMove(int x, int y);
    afx_msg void OnHelpAbout();
    afx_msg void OnHelpHelpscreen();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnClose();
};
#pragma once
