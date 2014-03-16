/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/

#include <tchar.h>
#include <math.h>

#include "..\src\stdafx.h"
#include "..\src\RemoteControl.h"
#include "..\src\RemoteControlDlg.h"
extern CRemoteControlApp theApp;

#include "AUXWidgetManager.h"
#include "pxcsmartptr.h"
CAUXWidgetManager g_widgets;

CAUXWidgetManager::CAUXWidgetManager()
{
    InitializeCriticalSection(&m_CriticalSection);
    m_lastWindow = NULL;
    m_layout = 0;
}

CAUXWidgetManager::~CAUXWidgetManager(void) 
{
    m_widgetList.clear();
    DeleteCriticalSection(&m_CriticalSection);
}

void CAUXWidgetManager::SwapLayout()
{
    ++m_layout%=4;
    Update();
    MoveWidgets(0, 0);
}

void CAUXWidgetManager::StopTracking()
{
    SaveConfiguration();
    CLoseAllWindows(NULL);
    CRemoteControlDlg* dlg = (CRemoteControlDlg*) theApp.m_pMainWnd;
    dlg->OnBnClickedButtonStart();
    dlg->ShowWindow(SW_RESTORE);
}

void CAUXWidgetManager::CLoseAllWindows(HWND hwnd)
{
    EnterCriticalSection(&m_CriticalSection);
    for (int i=0; i<(int)m_widgetList.size(); i++) {
        if (m_widgetList[i].window && (m_widgetList[i].window != hwnd)) {
            ShowWindow(m_widgetList[i].window, SW_HIDE);
            m_widgetList[i].visible = FALSE;
            if (hwnd) m_widgetList[i].active = FALSE;
        }
    }
    Update();
    LeaveCriticalSection(&m_CriticalSection);
}

void CAUXWidgetManager::ShowAllWindows(BOOL reset)
{
    EnterCriticalSection(&m_CriticalSection);
    for (int i=0; i<(int)m_widgetList.size(); i++) {
        if (m_widgetList[i].window) {
            ShowWindow(m_widgetList[i].window, SW_SHOW);
            m_widgetList[i].visible = TRUE;
        }
        m_widgetList[i].active = TRUE;
    }
    if (reset) SaveConfiguration();
    Update();
    LeaveCriticalSection(&m_CriticalSection);
    MoveWidgets(0, 0);
}

void CAUXWidgetManager::RemoveWindow(HWND window)
{
    if (window==NULL) return;
    EnterCriticalSection(&m_CriticalSection);
    for (int i=0; i<(int)m_widgetList.size(); i++) {
        if (m_widgetList[i].window && (m_widgetList[i].window == window)) {
            m_widgetList[i].window = NULL;
            break;
        }
    }
    Update();
    LeaveCriticalSection(&m_CriticalSection);
    MoveWidgets(0, 0);
}

void CAUXWidgetManager::CloseWindow(HWND window)
{
    if (window==NULL) window=m_lastWindow;
    if (window) {
        ShowWindow(window, SW_HIDE);
    }

    EnterCriticalSection(&m_CriticalSection);
    for (int i=0; i<(int)m_widgetList.size(); i++) {
        if (m_widgetList[i].window && (m_widgetList[i].window == window)) {
            m_widgetList[i].visible = FALSE;
            m_widgetList[i].active = FALSE;
            break;
        }
    }
    Update();
    LeaveCriticalSection(&m_CriticalSection);
    MoveWidgets(0, 0);
}

void CAUXWidgetManager::AddWindow(HWND window, int row, int col, wchar_t *title)
{
    int id = -1;
    EnterCriticalSection(&m_CriticalSection);
    // find empty slot
    for (int i=0; i<(int)m_widgetList.size(); i++) {
        if (m_widgetList[i].window==NULL) {
            id = i;
            break;
        }
    }
    if (id<0) {
        id = (int)m_widgetList.size();
        m_widgetList.resize(id+1);
    }
    m_widgetList[id].window = window;
    m_widgetList[id].rect.left = 0;
    m_widgetList[id].rect.bottom = 0;
    m_widgetList[id].pin = TRUE;
    m_widgetList[id].visible = TRUE;
    m_widgetList[id].active = TRUE;
    m_widgetList[id].row = row;
    m_widgetList[id].col = col;
    swprintf_s(m_widgetList[id].title, 256, L"%s", title);
    Update();
    LeaveCriticalSection(&m_CriticalSection);
    FixPosition(window, false);
    LoadConfiguration();
    MoveWidgets(0, 0);
}

void CAUXWidgetManager::Update()
{
    // set layout
    POINT startPos = {0, 0};
    switch (m_layout) {
    case 0:
    case 1:
        for (int col=0; col<3; col++) {
            if (m_layout==0) startPos.x = 0;
            else startPos.y = 0;
            for (int i=0; i<(int)m_widgetList.size(); i++) {
                if (m_widgetList[i].window && m_widgetList[i].pin  && m_widgetList[i].visible && (m_widgetList[i].col==col)) {
                    for (int j=i; j<(int)m_widgetList.size(); j++) {
                        if (m_widgetList[j].window && m_widgetList[j].pin  && m_widgetList[j].visible && (m_widgetList[j].col==col)) {
                            m_widgetList[j].rect.left = startPos.x;
                            m_widgetList[j].rect.top = startPos.y;
                            if (m_layout==0) startPos.x += 170;
                            else startPos.y += 130;
                        }
                    }
                    if (m_layout==0) startPos.y += 130;
                    else startPos.x += 170;
                    break;
                }
            }
        }
        break;
    case 2:
    case 3:
        for (int i=0; i<(int)m_widgetList.size(); i++) {
            if (m_widgetList[i].window && m_widgetList[i].pin  && m_widgetList[i].visible) {
                m_widgetList[i].rect.left = startPos.x;
                m_widgetList[i].rect.top = startPos.y;
                if (m_layout==2) startPos.y += 130;
                else startPos.x += 170;
            }
        }
        break;
    }
}

void CAUXWidgetManager::FixPosition(HWND window, bool track)
{
    m_lastWindow = window;
    for (int i=0; i<(int)m_widgetList.size(); i++) {
        if (m_widgetList[i].window==window && m_widgetList[i].pin && m_widgetList[i].visible) {
            GetWindowInfo(m_widgetList[i].window, &m_lastWinInfo);
            if (track) {
                m_lastWinInfo.rcWindow.left-=m_widgetList[i].rect.left;
                m_lastWinInfo.rcWindow.top-=m_widgetList[i].rect.top;
                m_lastWinInfo.rcWindow.right-=m_widgetList[i].rect.left;
                m_lastWinInfo.rcWindow.bottom-=m_widgetList[i].rect.top;
            }
            break;
        }
    }
}

void CAUXWidgetManager::SetPause(BOOL pause)
{
    for (int i=0; i<(int)m_widgetList.size(); i++) {
        if (m_widgetList[i].window && m_widgetList[i].pin && m_widgetList[i].visible) {
            PostMessage(m_widgetList[i].window, WM_COMMAND, COMMAND_PAUSE, pause);
        }
    }
}

void CAUXWidgetManager::MoveWidgets(int dX, int dY)
{
    for (int i=0; i<(int)m_widgetList.size(); i++) {
        if (m_widgetList[i].window && m_widgetList[i].pin && m_widgetList[i].visible && m_widgetList[i].pin) {
            SetWindowPos( m_widgetList[i].window, NULL, 
                m_lastWinInfo.rcWindow.left+m_widgetList[i].rect.left+dX, m_lastWinInfo.rcWindow.top+m_widgetList[i].rect.top+dY, 
                m_lastWinInfo.rcClient.right-m_lastWinInfo.rcClient.left, m_lastWinInfo.rcClient.bottom-m_lastWinInfo.rcClient.top, SWP_SHOWWINDOW );
        }
    }
}

BOOL CAUXWidgetManager::IsPin(HWND hwnd)
{
    if (!hwnd) return FALSE;
    for (int i=0; i<(int)m_widgetList.size(); i++) {
        if (m_widgetList[i].window==hwnd) return m_widgetList[i].pin;
    }
    return FALSE;
}

void CAUXWidgetManager::SetPin(HWND hwnd, BOOL pin)
{
    if (!hwnd) return;
    EnterCriticalSection(&m_CriticalSection);
    for (int i=0; i<(int)m_widgetList.size(); i++) {
        if (m_widgetList[i].window==hwnd) {
            m_widgetList[i].pin = pin;
            WINDOWINFO winInfo;
            winInfo.cbSize = sizeof(winInfo);
            GetWindowInfo(hwnd, &winInfo);
            int cx = 160;
            int cy = 120;
            if (m_widgetList[i].pin==FALSE) {
                // set small size
                cx = 80;
                cy = 60;
            }
            ::SetWindowPos(hwnd, NULL, winInfo.rcWindow.left, winInfo.rcWindow.top, cx, cy, SWP_SHOWWINDOW);
            ::PostMessage(hwnd, WM_RBUTTONUP, 0, 0);

            Update();
            MoveWidgets(0, 0);
            break;
        }
    }
    LeaveCriticalSection(&m_CriticalSection);
}

void CAUXWidgetManager::SetUnPinWindowPosition(HWND hwnd, RECT rect)
{
    SetWindowPos(hwnd, NULL, rect.left, rect.top, rect.right, rect.bottom, SWP_SHOWWINDOW);
    for (int i=0; i<(int)m_widgetList.size(); i++) {
        if (m_widgetList[i].window==hwnd) {
            m_widgetList[i].rect = rect;
        }
    }
}

void CAUXWidgetManager::UpdateConfiguration(int gx, int gy, WCHAR* widgetName, int pin, int active, int x, int y )
{
    CString str;
    WCHAR name[MAX_PATH];
    ::GetTempPath(MAX_PATH, name);
    CString fname = name;
    fname.Append(L"RemoteControl.ini");
    if (gx>=0) {
        //WritePrivateProfileString(TEXT("Widgets"), NULL, NULL, fname);
        str = L"";
        str.AppendFormat(L"%i,%i,%i,%i", 0,0,160,120);
        int cX = 160;//m_lastWinInfo.rcWindow.right-m_lastWinInfo.rcWindow.left;
        int cY = 120;//m_lastWinInfo.rcWindow.bottom-m_lastWinInfo.rcWindow.top;
        str.AppendFormat(L",%i,%i,%i,%i", gx,gy,cX,cY);
        str.AppendFormat(L",%i", m_layout);
        WritePrivateProfileString( TEXT("Widgets"), TEXT("_main_"), str, fname); 
    } else {
        str = L"";
        if (pin) str.AppendFormat(L"%i,%i,%i,%i", x, y, 160, 120);
        else     str.AppendFormat(L"%i,%i,%i,%i", x, y, 80, 60);
        str.AppendFormat(L",%i", pin);
        str.AppendFormat(L",%i", active);
        WritePrivateProfileString( TEXT("Widgets"), widgetName, str, fname); 
    }
}

void CAUXWidgetManager::SaveConfiguration()
{
    if ((m_lastWinInfo.rcClient.left==m_lastWinInfo.rcClient.right) || (m_lastWinInfo.rcClient.top==m_lastWinInfo.rcWindow.bottom)) return;

    CString str;
    WCHAR name[MAX_PATH];
    ::GetTempPath(MAX_PATH, name);
    CString fname = name;
    fname.Append(L"RemoteControl.ini");
    str = L"";
    str.AppendFormat(L"%i,%i,%i,%i", m_lastWinInfo.rcClient.left,m_lastWinInfo.rcClient.top,m_lastWinInfo.rcClient.right, m_lastWinInfo.rcClient.bottom);
    str.AppendFormat(L",%i,%i,%i,%i", m_lastWinInfo.rcWindow.left,m_lastWinInfo.rcWindow.top,m_lastWinInfo.rcWindow.right, m_lastWinInfo.rcWindow.bottom);
    str.AppendFormat(L",%i", m_layout);
    WritePrivateProfileString( TEXT("Widgets"), TEXT("_main_"), str, fname); 
    for (int i=0; i<(int)m_widgetList.size(); i++) {
        if (m_widgetList[i].title[0]) {
            str = L"";
            int cx = 80;
            int cy = 60;
            if (m_widgetList[i].pin) { cx = 160; cy=120; }
            str.AppendFormat(L"%i,%i,%i,%i", m_widgetList[i].rect.left, m_widgetList[i].rect.top, cx, cy);
            str.AppendFormat(L",%i", m_widgetList[i].pin);
            str.AppendFormat(L",%i", m_widgetList[i].active);
            WritePrivateProfileString( TEXT("Widgets"), m_widgetList[i].title, str, fname);
            Sleep(100);
        }
    }
}

void CAUXWidgetManager::LoadConfiguration()
{
    WCHAR str[1024];
    int _p[10];
    ::GetTempPath(MAX_PATH, str);
    CString fname = str;
    fname.Append(L"RemoteControl.ini");
    if (GetPrivateProfileString( TEXT("Widgets"), TEXT("_main_"), NULL, str, 1024, fname)) {
        if (swscanf_s(str, L"%i,%i,%i,%i,%i,%i,%i,%i,%i",&_p[0], &_p[1],&_p[2],&_p[3],&_p[4], &_p[5],&_p[6],&_p[7],&_p[8])==9) {
            if ((_p[0]==_p[2]) || (_p[1]==_p[3])) return;
            m_lastWinInfo.rcClient.left   = _p[0];
            m_lastWinInfo.rcClient.top    = _p[1];
            m_lastWinInfo.rcClient.right  = _p[2];
            m_lastWinInfo.rcClient.bottom = _p[3];
            m_lastWinInfo.rcWindow.left   = _p[4];
            m_lastWinInfo.rcWindow.top    = _p[5];
            m_lastWinInfo.rcWindow.right  = _p[6];
            m_lastWinInfo.rcWindow.bottom = _p[7];
            m_layout = _p[8];
        }
    }
    for (int i=0; i<(int)m_widgetList.size(); i++) {
        if (GetPrivateProfileString( TEXT("Widgets"), m_widgetList[i].title, NULL, str, MAX_PATH, fname)) {
            // parse configuration
            if (swscanf_s(str, L"%i,%i,%i,%i,%i,%i", &_p[0],&_p[1],&_p[2],&_p[3],&_p[4],&_p[5])==6) {
                m_widgetList[i].rect.left = _p[0];
                m_widgetList[i].rect.top = _p[1];
                m_widgetList[i].rect.right = _p[2];
                m_widgetList[i].rect.bottom = _p[3];
                m_widgetList[i].pin = _p[4];
                if (_p[5]==0) {
                    m_widgetList[i].visible = FALSE;
                    m_widgetList[i].active = FALSE;
                    if (m_widgetList[i].window) ShowWindow(m_widgetList[i].window, SW_HIDE);
                }
            }
        }
        if (m_widgetList[i].window && m_widgetList[i].visible) {
            if (m_widgetList[i].pin==FALSE) SetUnPinWindowPosition(m_widgetList[i].window, m_widgetList[i].rect);
            else ShowWindow(m_widgetList[i].window, SW_SHOW);
        }
    }
}


