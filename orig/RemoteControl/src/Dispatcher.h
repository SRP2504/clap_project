/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/

#pragma once

#include <UIAutomationClient.h>
#include "..\sensors\common\sensor.h"
#include <map>
#include "atlstr.h"

#define MAX_SENSORS 4
#define RC_METRO_STYLE L"Windows.UI.Core.CoreWindow"
#define RC_START_MENU L"ImmersiveLauncher"

struct stActionDescriptor {
    int  isGrouping;
    int  isActual;
    int  specific;
    int  isEnable;
    int  sensorId;
    int  eventId;
    wchar_t className[MAX_PATH];
    wchar_t windowName[MAX_PATH];
    HWND hWnd;
    int  pause;
    enum SPECIFIC_TYPE {
        MY_ST_NORMAL = 0,
        MY_ST_START_MENU,
        MY_ST_WINDOWS8,
        MY_ST_ANY_ACTIVE,
    };
    enum ACTION_TYPE {
        MY_AT_NONE = 0,
        MY_AT_NAVIGATION,
        MY_AT_KEYBOARD,
        MY_AT_BUTTON,
        MY_AT_CHECKBOX,
        MY_AT_MOUSE,
        MY_AT_SYSTEM,
        MY_AT_PRINT
    };
    enum NAVIGATION_TYPE {
        MY_NT_NONE = 0,
        MY_NT_TOP,
        MY_NT_MIN,
        MY_NT_MAX
    };
    enum SYSTEM_TYPE {
        MY_SYS_TASKBAR = 0,
        MY_SYS_SETBAR,
        MY_SYS_SNAP_LEFT,
        MY_SYS_SNAP_RIGHT,
        MY_SYS_NEXT_APP,
    };
    enum KEYBOARD_TYPE {
        MY_KT_NONE = 0,
        MY_KT_PRESS,
        MY_KT_ESC,
        MY_KT_ENTER,
        MY_KT_SPACE,
        MY_KT_TAB,
        MY_KT_WIN,
        MY_KT_ALT_PRESS,
        MY_KT_ALT_RELEASE,
        MY_KT_DEL,
        MY_KT_BKSPACE,
    };
    enum PRINT_TYPE {
        MY_PT_NONE = 0,
        MY_PT_PRINT_EVENT,
        MY_PT_PRINT_EVENT_ENTER,
        MY_PT_PRINT_NUMBER,
    };
    ACTION_TYPE type;
    union {
        struct {
            KEYBOARD_TYPE key_type;
            DWORD key_code;
            wchar_t key_code_name[20];
        };
        NAVIGATION_TYPE navigate_type;
        SYSTEM_TYPE system_type;
        struct {
            wchar_t controlName[MAX_PATH];
            union {
                wchar_t automationId[MAX_PATH];
                struct {
                    int rtType;
                    int rtCounter;
                    int runtimeId[10];
                };
            };
        };
    };
    stActionDescriptor() { memset(this, 0, sizeof(struct stActionDescriptor)); };
    void GetTitle(CString& str, BOOL bName=TRUE, BOOL bAction=TRUE, BOOL bType=TRUE);
    void GetSensorTitle(CString& str, BOOL bMultiString=TRUE);
};

class CDispatcher
{
public:
    CDispatcher(void);
    virtual ~CDispatcher(void);
    void Release();
    BOOL UpdateProcessList();
    
public:
    int  AddAction(const stActionDescriptor* descr);
   stActionDescriptor* GetActionById(int actionId)
   {
      if(actionId<(int)m_actionList.size()) return m_actionList[actionId];
      return 0;
   };
    stActionDescriptor* GetAction(unsigned int num);

    CSensor* GetSensor(int num) { return m_sensorList[num]; };
    CSensor* GetSensorById(CSensor::SENSOR_ID id);

    BOOL LoadActionList(CString* name=NULL, BOOL reset=true);
    BOOL SaveActionList(CString* name=NULL);
    BOOL LoadXML(WCHAR* filename, BOOL reset=true);
    BOOL SaveXML(WCHAR* filename);
    BOOL NewActionList(void);
    void StartFillTaskList();
    int  StopFillTaskList();
    int  AppendTaskToList(int id);
    int  TaskCount() { return (int)m_taskList.size(); };
    stActionDescriptor* GetTask(unsigned int num);
    BOOL m_testConfigIsLoaded;
    BOOL m_wasFileLoad;

    // utilites
    HWND GetInstance(LPCWSTR appName);
    HWND GetWindow(LPCWSTR className, LPCWSTR winName);

protected:
    std::map<int, stActionDescriptor*> m_actionList;
    std::vector<stActionDescriptor*> m_taskList;

    BSTR m_tmpStr1;
    BSTR m_tmpStr2;
    IUIAutomationElementArray* m_processList;

    CSensor* m_sensorList[MAX_SENSORS];
} ;

