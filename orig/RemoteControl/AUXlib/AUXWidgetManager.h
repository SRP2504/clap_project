/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/

#pragma once
#include <Windows.h>
#include "AUXStreamRender.h"
#include <vector>

#define COMMAND_CLOSE 1
#define COMMAND_SHOW_ALL 2
#define COMMAND_IN_GROUP 3
#define COMMAND_FROM_GROUP 4
#define COMMAND_PAUSE 5
#define COMMAND_CLOSE_ALL 6
#define COMMAND_STOP_TRACKING 7
#define COMMAND_LAYOUT 8
#define COMMAND_MIRROR 9

class CAUXWidgetManager {
public:
    typedef struct widget_descr_st {
        HWND  window;
        wchar_t title[256];
        RECT  rect;
        BOOL  pin;
        BOOL  visible;
        BOOL  active;
        int   row;
        int   col;

        widget_descr_st() { memset(this, 0, sizeof(widget_descr_st));};

    };
    
    CAUXWidgetManager();
    virtual ~CAUXWidgetManager();

    void AddWindow(HWND window, int row, int col, wchar_t *title);
    void RemoveWindow(HWND window);
    void CloseWindow(HWND window);
    void CLoseAllWindows(HWND hwnd);
    void ShowAllWindows(BOOL reset=FALSE);
    void StopTracking();
    void SwapLayout();
    void SaveConfiguration();
    void LoadConfiguration();
    void UpdateConfiguration(int gx, int gy, WCHAR* widgetName, int pin, int active, int x, int y);

    void FixPosition(HWND window, bool track=true);
    void SetUnPinWindowPosition(HWND hwnd, RECT rect);
    void MoveWidgets(int dX, int dY);
    BOOL IsPin(HWND hwnd);
    void SetPin(HWND hwnd, BOOL pin);
    void SetPause(BOOL pause);
    
    HWND m_lastWindow;
    WINDOWINFO  m_lastWinInfo;
    std::vector<widget_descr_st> m_widgetList;
protected:
    void Update();

    int m_layout;
    CRITICAL_SECTION m_CriticalSection;
};

