/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/

#include <tchar.h>
#include <math.h>

#include "AUXWidget.h"
#include "AUXWidgetManager.h"
#include "pxcsmartptr.h"

extern CAUXWidgetManager g_widgets;

CAUXWidget::CAUXWidget(int row, int col, pxcCHAR *title) 
{
    WNDCLASS wc;
    memset(&wc,0,sizeof(wc));
    wc.lpszClassName=title;
    wc.lpfnWndProc=WindowProc;
    wc.style=CS_HREDRAW|CS_VREDRAW;
    wc.hCursor=LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&wc);
    m_sizeX = 0;
    m_sizeY = 0;
    m_mirrorMode = -1;
    m_hWnd=CreateWindowEx(WS_EX_TOPMOST|WS_EX_TOOLWINDOW, title, title, WS_POPUP, 10, 10, 160, 120,0, 0,0,this);
    m_lastMousePos.x = m_lastMousePos.y = 0;
    m_row = row;
    m_col = col;
    g_widgets.AddWindow(m_hWnd, row, col, title);
    SetPause(FALSE);
    CreateMenu();
}

CAUXWidget::~CAUXWidget(void) 
{
    if (m_hWnd) DestroyWindow(m_hWnd);
    g_widgets.RemoveWindow(m_hWnd);
    DeleteMenu();
}

void CAUXWidget::DoMessageLoop(void) 
{
    for (MSG msg; PeekMessage(&msg, m_hWnd,  0, 0, PM_REMOVE);) {
        DispatchMessage(&msg); 
    } 
}

void CAUXWidget::FixWindow()
{
    GetCursorPos(&m_lastMousePos);
    GetWindowInfo(m_hWnd, &m_lastWinInfo);
}

void CAUXWidget::MoveWindow()
{
    POINT curs;
    GetCursorPos(&curs);
    RECT rect;
    rect.left = m_lastWinInfo.rcWindow.left + curs.x - m_lastMousePos.x;
    rect.top = m_lastWinInfo.rcWindow.top + curs.y - m_lastMousePos.y;
    rect.right = m_lastWinInfo.rcClient.right - m_lastWinInfo.rcClient.left;
    rect.bottom = m_lastWinInfo.rcClient.bottom - m_lastWinInfo.rcClient.top;
    g_widgets.SetUnPinWindowPosition(m_hWnd, rect);
}

LRESULT CAUXWidget::WindowProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam) 
{
    CAUXWidget *pthis=(CAUXWidget*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    switch (uMsg) { 
    case WM_SIZE:
        pthis->m_sizeX = LOWORD(lParam);
        pthis->m_sizeY = HIWORD(lParam);
        break;
    case WM_COMMAND:
        switch (wParam) {
        case COMMAND_PAUSE:
            pthis->SetPause(lParam!=0);
            break;
        case COMMAND_CLOSE:
            if (g_widgets.IsPin(hwnd)) {
                g_widgets.CloseWindow(hwnd);
            } else {
                ShowWindow(hwnd,SW_HIDE);
                g_widgets.CloseWindow(hwnd);
            }
            break;
        case COMMAND_CLOSE_ALL:
            g_widgets.CLoseAllWindows(hwnd);
            break;
        case COMMAND_STOP_TRACKING:
            g_widgets.StopTracking();
            break;
        case COMMAND_SHOW_ALL:
            g_widgets.ShowAllWindows();
            break;
        case COMMAND_FROM_GROUP:
            g_widgets.SetPin(hwnd, FALSE);
            break;
        case COMMAND_IN_GROUP:
            g_widgets.SetPin(hwnd, TRUE);
            break;
        case COMMAND_LAYOUT:
            g_widgets.SwapLayout();
            break;
        case COMMAND_MIRROR:
            pthis->SetMirror(!pthis->IsMirror());
            break;
        }
        break;
    case WM_CREATE:
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)((CREATESTRUCT*)lParam)->lpCreateParams);
        return TRUE; 
    case WM_PAINT:
        if (!pthis->m_bPause) {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            pthis->OnPaint(hdc);

            EndPaint(hwnd, &ps);
        }
        return TRUE;
    case WM_MOUSEMOVE:
        if (wParam&MK_LBUTTON) {
            POINT curs;
            GetCursorPos(&curs);
            int dx = curs.x-pthis->m_lastMousePos.x; 
            int dy = curs.y-pthis->m_lastMousePos.y; 
            if (g_widgets.IsPin(hwnd)) {
                g_widgets.MoveWidgets(dx, dy);
            } else {
                pthis->MoveWindow();
            }
        } else {
            g_widgets.SetPause(FALSE);
            pthis->SetPause(FALSE);
        }
        return TRUE;
    case WM_LBUTTONUP:
        if (g_widgets.IsPin(hwnd)) {
            g_widgets.FixPosition(hwnd);
            g_widgets.SetPause(TRUE);
        } else {
            pthis->FixWindow();
            pthis->SetPause(TRUE);
        }
        g_widgets.SetPause(FALSE);
        pthis->SetPause(FALSE);
        return TRUE;
    case WM_LBUTTONDOWN:
        GetCursorPos(&pthis->m_lastMousePos);
        if (g_widgets.IsPin(hwnd)) {
            g_widgets.FixPosition(hwnd);
            g_widgets.SetPause(TRUE);
        } else {
            pthis->FixWindow();
            pthis->SetPause(TRUE);
        }
        break;
    case WM_RBUTTONUP:
        if (g_widgets.IsPin(hwnd)) {
            g_widgets.FixPosition(hwnd);
        } else {
            pthis->FixWindow();
        }
        break;
    case WM_RBUTTONDOWN:
        GetCursorPos(&pthis->m_lastMousePos);
        if (g_widgets.IsPin(hwnd)) {
            g_widgets.FixPosition(hwnd);
        } else {
            pthis->FixWindow();
        }
        pthis->RunMenu();
        break;
    case WM_CLOSE:
        ShowWindow(hwnd,SW_HIDE);
        return FALSE;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    } 
    return FALSE; 
}

void CAUXWidget::RunMenu()
{
    // pop-up
    DeleteMenu();
    m_hMenu = CreatePopupMenu();
    MENUITEMINFO mii;
    mii.cbSize = sizeof(MENUITEMINFO);
    mii.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_ID; 
    int id = 0;
    mii.wID = COMMAND_CLOSE;
    mii.fType = MFT_STRING;
    mii.dwTypeData = L"Close"; 
    InsertMenuItem(m_hMenu, id++, TRUE, &mii);

    mii.wID = COMMAND_CLOSE_ALL;
    mii.fType = MFT_STRING;
    mii.dwTypeData = L"Close ALL But This"; 
    InsertMenuItem(m_hMenu, id++, TRUE, &mii);

    mii.wID = COMMAND_SHOW_ALL;
    mii.fType = MFT_STRING;
    mii.dwTypeData = L"Show ALL"; 
    InsertMenuItem(m_hMenu, id++, TRUE, &mii);
    
    mii.dwTypeData = L"Pin"; 
    mii.fState = 0;
    mii.fMask |= MIIM_STATE ;
    if (g_widgets.IsPin(m_hWnd)) {
        mii.wID = COMMAND_FROM_GROUP;
        mii.fState = MFS_CHECKED;
    } else {
        mii.wID = COMMAND_IN_GROUP;
        mii.fState = 0;
    }
    InsertMenuItem(m_hMenu, id++, TRUE, &mii);
    mii.fMask &= ~MIIM_STATE;
    mii.fState = 0;

    mii.fType = MFT_SEPARATOR;
    InsertMenuItem(m_hMenu, id++, TRUE, &mii);
    mii.wID = COMMAND_LAYOUT;
    mii.fType = MFT_STRING;
    mii.dwTypeData = L"Layout"; 
    InsertMenuItem(m_hMenu, id++, TRUE, &mii);
    if (m_mirrorMode>=0) {
        mii.wID = COMMAND_MIRROR;
        mii.fMask |= MIIM_STATE ;
        mii.fState = IsMirror() ? MFS_CHECKED : 0;
        mii.fType = MFT_STRING;
        mii.dwTypeData = L"Mirror Image"; 
        InsertMenuItem(m_hMenu, id++, TRUE, &mii);
        mii.fMask &= ~MIIM_STATE ;
    }

    mii.fType = MFT_SEPARATOR;
    InsertMenuItem(m_hMenu, id++, TRUE, &mii);
    mii.wID = COMMAND_STOP_TRACKING;
    mii.fType = MFT_STRING;
    mii.dwTypeData = L"Stop Tracking"; 
    InsertMenuItem(m_hMenu, id++, TRUE, &mii);

    TrackPopupMenu(m_hMenu, 0, m_lastMousePos.x, m_lastMousePos.y, 0, m_hWnd, NULL);
}

void CAUXWidget::DeleteMenu()
{
    if (m_hMenu) {
        DestroyMenu(m_hMenu);
        m_hMenu = NULL;
    }
}
