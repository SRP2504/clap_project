/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/

#pragma once
#include <Windows.h>
#include "pxcimage.h"
#include "atlstr.h"

class CAUXWidget 
{
public:
    CAUXWidget(int row, int col, pxcCHAR *title);
    virtual ~CAUXWidget(void);
    void Release() { delete this; }
    void SetPause(BOOL pause) { m_bPause = pause; };
    void MoveWindow();
    void FixWindow();
    void RunMenu();
    void DeleteMenu();
    BOOL IsMirror() { return m_mirrorMode==1; };
    void SetMirror(BOOL bMirror) { m_mirrorMode = bMirror; };

    HWND m_hWnd;
    HMENU m_hMenu;

protected:
    POINT       m_lastMousePos;
    WINDOWINFO  m_lastWinInfo;
    BOOL        m_bPause;
    int         m_mirrorMode;

    int m_row;
    int m_col;
    int m_sizeX;
    int m_sizeY;

    virtual void OnPaint(HDC hdc) {};
    void DoMessageLoop(void);
    static LRESULT CALLBACK WindowProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam); 
};

