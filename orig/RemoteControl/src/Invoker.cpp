/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/

#include "stdafx.h"
#include "Invoker.h"
#include "RemoteControl.h"
#include "RemoteControlDlg.h"

extern CDispatcher g_Dispatcher;
extern CRemoteControlApp theApp;

CInvoker::CInvoker(void)
{
    m_automation = NULL;
    m_renderG = NULL;
    m_renderF = NULL;
    m_renderV = NULL;
    InitializeCriticalSection(&m_mutex);
}

CInvoker::~CInvoker(void)
{
    if (m_automation) m_automation->Release();
    DeleteCriticalSection(&m_mutex);
    if (m_renderG) {
        delete m_renderG;
        m_renderG = NULL;
    }
    if (m_renderF) {
        delete m_renderF;
        m_renderF = NULL;
    }
    if (m_renderV) {
        delete m_renderV;
        m_renderV = NULL;
    }
}

void CInvoker::Lock()
{
    EnterCriticalSection(&m_mutex);
}

void CInvoker::UnLock()
{
    LeaveCriticalSection(&m_mutex);
}

IUIAutomation* CInvoker::GetAutomation()
{
    if (m_automation) return m_automation;
#ifdef RC_WIN7
    CoCreateInstance(__uuidof(CUIAutomation), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_automation));
#else
    CoCreateInstance(__uuidof(CUIAutomation8), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_automation));
#endif
    // try the old version
    CoCreateInstance(__uuidof(CUIAutomation), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_automation));
    if (m_automation) return m_automation;
    return NULL;
}

bool CInvoker::IsRunning(CSensor::SENSOR_ID id)
{
    for (int i=0; i<MAX_SENSORS; i++) {
        CSensor* sensor = g_Dispatcher.GetSensor(i);
        if (sensor && sensor->GetId()==id) {
            return !sensor->stop;
        }
    }
    return false;
}

bool CInvoker::StartSensor(CSensor::SENSOR_ID id, BOOL videoControl)
{
    for (int i=0; i<MAX_SENSORS; i++) {
        CSensor* sensor = g_Dispatcher.GetSensor(i);
        if (sensor && sensor->GetId()==id) {
            sensor->Start(videoControl);
            return !sensor->stop;
        }
    }
    return false;
}

int CInvoker::StartAllSensors(BOOL videoControl)
{
    int count_run_sensors = 0;
    for (int i=0; i<MAX_SENSORS; i++) {
        CSensor* sensor = g_Dispatcher.GetSensor(i);
        if (sensor && sensor->IsEnable()) {
            // if the sensor has task
            for (int j=0; g_Dispatcher.GetTask(j)!=NULL; j++) {
                if (g_Dispatcher.GetTask(j)->sensorId == sensor->GetId()) {
                    sensor->Start(videoControl);
                    if (videoControl) {
                        if (sensor->GetId()==CSensor::Sensor_FaceAttributes) {
                            if (m_renderF) delete m_renderF;
                            m_renderF = new CAUXTextRender(0, 0, L"Face events:");
                            if (m_renderF) m_renderF->SetBackground(LoadBitmap(theApp.m_hInstance, MAKEINTRESOURCE(IDB_BITMAP_WIDGET)));
                        }
                        if (sensor->GetId()==CSensor::Sensor_Gesture_detector) {
                            if (m_renderG) delete m_renderG;
                            m_renderG = new CAUXTextRender(0, 1, L"Gestures:");
                            if (m_renderG) m_renderG->SetBackground(LoadBitmap(theApp.m_hInstance, MAKEINTRESOURCE(IDB_BITMAP_WIDGET)));
                        }
                        if (sensor->GetId()==CSensor::Sensor_Voice_recognition) {
                            if (m_renderV) delete m_renderV;
                            m_renderV = new CAUXTextRender(0, 2, L"Voice commands:");
                            if (m_renderV) m_renderV->SetBackground(LoadBitmap(theApp.m_hInstance, MAKEINTRESOURCE(IDB_BITMAP_WIDGET)));
                        }
                    }
                    count_run_sensors++;
                    break;
                }
            }
        }
    }
    if (count_run_sensors) g_Dispatcher.UpdateProcessList();
    return count_run_sensors;
}

bool CInvoker::StopSensor(CSensor::SENSOR_ID id)
{
    for (int i=0; i<MAX_SENSORS; i++) {
        CSensor* sensor = g_Dispatcher.GetSensor(i);
        if (sensor && sensor->GetId()==id) {
            sensor->Stop();
            return true;
        }
    }
    return false;
}

void CInvoker::StopAllSensors()
{
    if (m_renderF) {
        delete m_renderF;
        m_renderF = NULL;
    }
    if (m_renderG) {
        delete m_renderG;
        m_renderG = NULL;
    }
    if (m_renderV) {
        delete m_renderV;
        m_renderV = NULL;
    }
    for (int i=0; i<MAX_SENSORS; i++) {
        CSensor* sensor = g_Dispatcher.GetSensor(i);
        if (sensor) sensor->Stop();
    }
}

int  CInvoker::ExecuteEvent(CSensor::SENSOR_ID sensor_id, int event_id, WCHAR* event_name)
{
    stActionDescriptor* action = NULL;
    Lock();
    int counter = 0;
    for (int i=0; i<1000; i++) {
        action = g_Dispatcher.GetTask(i);
        if (!action) break;
        if (action->sensorId==sensor_id && action->eventId==event_id) {
            ExecuteAction(action, FALSE, event_name);
            counter++;
        }
    }
    UnLock();

    if (m_renderF || m_renderG || m_renderV) {
        CSensor* sensor = g_Dispatcher.GetSensorById(sensor_id);
        if (sensor) {
            st_eventDescriptor descr;
            if (sensor->GetEventbyId(event_id, &descr)) {
                if (m_renderF && (sensor_id==CSensor::Sensor_FaceAttributes)) {
                    m_renderF->UpdateText(descr.name, counter);
                }
                if (m_renderG && (sensor_id==CSensor::Sensor_Gesture_detector)) {
                    m_renderG->UpdateText(descr.name, counter);
                }
                if (m_renderV && (sensor_id==CSensor::Sensor_Voice_recognition)) {
                    m_renderV->UpdateText(descr.name, counter);
                }
            }
        }
    }

    return counter; // return number of executed actions
}

BOOL CInvoker::Activate(stActionDescriptor* descr, HWND hWnd)
{
    if (!IsWindow(hWnd)) return FALSE; // window not exist
    ::BringWindowToTop(hWnd);
    ::SetForegroundWindow(hWnd);
    SwitchToThisWindow(hWnd, TRUE);
    return TRUE;
}


int CInvoker::ExecuteAction(int actionId)
{
    stActionDescriptor* descr = g_Dispatcher.GetActionById(actionId);
    if (descr) return ExecuteAction(descr, TRUE);
    return -1;
}


int CInvoker::ExecuteAction(stActionDescriptor* descr, BOOL enable, WCHAR* event_name)
{
    if (!descr || !descr->isActual) return -1;
    if (!descr->isEnable && !enable) return 0;
    POINT currentPos;
    ::GetCursorPos(&currentPos);
    HWND top = NULL;

    if (enable) g_Dispatcher.UpdateProcessList();

    HWND _hWnd = descr->hWnd;
    if (descr->specific != stActionDescriptor::MY_ST_ANY_ACTIVE) {
        if (_hWnd==NULL) return 0;
        if (enable) top = GetForegroundWindow();
    }

    // force focus 
    if (enable && _hWnd) Activate(descr, _hWnd);
    switch (descr->type) {
    case stActionDescriptor::MY_AT_NAVIGATION:
        switch (descr->navigate_type) {
        case stActionDescriptor::MY_NT_TOP:
            ::AllowSetForegroundWindow(GetCurrentProcessId());
            ::BringWindowToTop(_hWnd);
            ::SetForegroundWindow(_hWnd);
            break;
        case stActionDescriptor::MY_NT_MAX:
            ::ShowWindow(_hWnd, SW_SHOWMAXIMIZED);
            break;
        case stActionDescriptor::MY_NT_MIN:
            ::ShowWindow(_hWnd, SW_SHOWMINIMIZED);
            break;
        }
        break;
    case stActionDescriptor::MY_AT_PRINT:
        if (_hWnd && (::GetForegroundWindow() != _hWnd)) break;
        {
            WCHAR* str = event_name;
            st_eventDescriptor evnt;
            if (str==NULL) {
                CSensor* sensor = g_Dispatcher.GetSensorById(CSensor::SENSOR_ID(descr->sensorId));
                if (!sensor) break;
                if(FALSE==sensor->GetEventbyId(descr->eventId, &evnt)) break;
                str = evnt.name;
            }
            if (descr->system_type == stActionDescriptor::MY_PT_PRINT_NUMBER) {
                struct { 
                    WCHAR* command; 
                    WCHAR symb;
                } grammar[17] = { L"one",L'1', L"two",L'2', L"three",L'3', L"four",L'4', L"five",L'5', L"six",L'6',
                                  L"seven",L'7', L"eight",L'8', L"eight",L'8', L"nine",L'9', L"zero",L'0', L"null",L'0',
                                  L"plus",L'+', L"minus",L'-', L"equal",L'=', L"divide",L'/', L"multiply",L'*' };
                // check digits
                int num=0;
                for (num=0; num<MAX_PATH; num++) {
                    if (str[num]==0) break;
                    int digit = str[num]-L'0';
                    if (digit<0||digit>9) break;
                    SHORT key = VkKeyScan(str[num]);
                    keybd_event(0x00ff&key, 0, 0, 0);
                    keybd_event(0x00ff&key, 0, KEYEVENTF_KEYUP, 0);
                }
                if (num==0) {
                    // check commands
                    for (int i=0; i<17; i++) {
                        if (_wcsicmp(str,grammar[i].command)==0) {
                            SHORT key = VkKeyScan(grammar[i].symb);
                            if (0xff00&key) keybd_event(VK_SHIFT, 0, 0, 0);
                            keybd_event(0x00ff&key, 0, 0, 0);
                            keybd_event(0x00ff&key, 0, KEYEVENTF_KEYUP, 0);
                            if (0xff00&key) keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
                            break;
                        }
                    }
                }
            } else {
                for (int i=0; i<MAX_PATH; i++) {
                    if (str[i]==0) break;
                    SHORT key = VkKeyScan(str[i]);
                    if (0xff00&key) keybd_event(VK_SHIFT, 0, 0, 0);
                    keybd_event(0x00ff&key, 0, 0, 0);
                    keybd_event(0x00ff&key, 0, KEYEVENTF_KEYUP, 0);
                    if (0xff00&key) keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
                }
                if (descr->system_type == stActionDescriptor::MY_PT_PRINT_EVENT_ENTER) {
                    keybd_event(VK_RETURN, 0, 0, 0);
                    keybd_event(VK_RETURN, 0, KEYEVENTF_KEYUP, 0);
                }
            }
        }
        break;
    case stActionDescriptor::MY_AT_KEYBOARD:
        // check focus
        if (_hWnd && (::GetForegroundWindow() != _hWnd)) break;
        switch (descr->key_type) {
        case stActionDescriptor::MY_KT_ESC:
            keybd_event(VK_ESCAPE, 0, 0, 0);
            keybd_event(VK_ESCAPE, 0, KEYEVENTF_KEYUP, 0);
            break;
        case stActionDescriptor::MY_KT_DEL:
            keybd_event(VK_DELETE, 0, 0, 0);
            keybd_event(VK_DELETE, 0, KEYEVENTF_KEYUP, 0);
            break;
        case stActionDescriptor::MY_KT_PRESS:
            {
                BYTE extKey = 0;
                if (descr->key_code & 0xff00) {
                    // extended key
                    switch(descr->key_code>>8 & (~HOTKEYF_EXT)) {
                    case HOTKEYF_SHIFT: extKey = VK_SHIFT; break;
                    case HOTKEYF_CONTROL: extKey = VK_CONTROL; break;
                    case HOTKEYF_ALT: extKey = VK_MENU; break;
                    }
                }

                if (extKey) { keybd_event(extKey, 0, 0, 0); Sleep(50); }
                keybd_event(descr->key_code & 0xff, 0, 0, 0);
                keybd_event(descr->key_code & 0xff, 0, KEYEVENTF_KEYUP, 0);
                if (extKey) keybd_event(extKey, 0, KEYEVENTF_KEYUP, 0);
                break;
            }
        case stActionDescriptor::MY_KT_ENTER:
            keybd_event(VK_RETURN, 0, 0, 0);
            keybd_event(VK_RETURN, 0, KEYEVENTF_KEYUP, 0);
            break;
        case stActionDescriptor::MY_KT_SPACE:
            keybd_event(VK_SPACE, 0, 0, 0);
            keybd_event(VK_SPACE, 0, KEYEVENTF_KEYUP, 0);
            break;
        case stActionDescriptor::MY_KT_BKSPACE:
            keybd_event(VK_BACK, 0, 0, 0);
            keybd_event(VK_BACK, 0, KEYEVENTF_KEYUP, 0);
            break;
        case stActionDescriptor::MY_KT_WIN:
            keybd_event(VK_LWIN, 0, 0, 0);  
            keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0);
            break;
        case stActionDescriptor::MY_KT_ALT_PRESS:
            keybd_event(VK_LMENU, 0, 0, 0);
            break;
        case stActionDescriptor::MY_KT_TAB:
            keybd_event(VK_LMENU, 0, 0, 0);
            break;
        case stActionDescriptor::MY_KT_ALT_RELEASE:
            keybd_event(VK_LMENU, 0, KEYEVENTF_KEYUP, 0);
            break;
        }
        break;
    case stActionDescriptor::MY_AT_BUTTON:
        {
            IUIAutomation* automation = GetAutomation();
            if (!automation) break;
            IUIAutomationElement* el = NULL;
            automation->ElementFromHandle(_hWnd, &el);
            if (!el) break;
            if (descr->rtType==0 && descr->rtCounter) {
                if (1) {
                    // use name
                    VARIANT var;
                    var.vt = VT_BSTR;
                    var.bstrVal = SysAllocString(descr->controlName);
                    IUIAutomationCondition* cond = NULL;
                    if (SUCCEEDED(automation->CreatePropertyCondition(UIA_NamePropertyId, var, &cond)) && cond) {
                        IUIAutomationElement* btn = NULL;
                        if (SUCCEEDED(el->FindFirst(TreeScope_Subtree, cond, &btn))) {
                            if (btn) {
                                // invoke
                                IUIAutomationInvokePattern*  inv;
                                btn->GetCurrentPatternAs(UIA_InvokePatternId, IID_PPV_ARGS(&inv));
                                if (inv) {
                                    inv->Invoke();
                                    inv->Release();
                                }
                            }
                        }
                        cond->Release();
                    }
                } else {
                    // use runtimeId
                    VARIANT var;
                    var.vt = VT_I4|VT_ARRAY;
                    SAFEARRAY* sa=SafeArrayCreateVector(VT_I4, 0, descr->rtCounter);
                    for (int i=0; i<descr->rtCounter; i++) {
                        ( (int*)(sa->pvData) )[i] = descr->runtimeId[i];
                    }
                    var.parray = sa;
                    IUIAutomationCondition* cond = NULL;
                    if (SUCCEEDED(automation->CreatePropertyCondition(UIA_RuntimeIdPropertyId, var, &cond)) && cond) {
                        IUIAutomationElement* btn = NULL;
                        if (SUCCEEDED(el->FindFirst(TreeScope_Subtree, cond, &btn))) {
                            if (btn) {
                                // invoke
                                IUIAutomationInvokePattern*  inv;
                                btn->GetCurrentPatternAs(UIA_InvokePatternId, IID_PPV_ARGS(&inv));
                                if (inv) { 
                                    inv->Invoke();
                                    inv->Release();
                                }
                            }
                        }
                        cond->Release();
                    }
                    SafeArrayDestroy(sa);
                }
            } else {
                //use automationId
                VARIANT var;
                var.vt = VT_BSTR;   
                var.bstrVal = SysAllocString(descr->automationId);

                IUIAutomationCondition* cond = NULL;
                if (SUCCEEDED(automation->CreatePropertyCondition(UIA_AutomationIdPropertyId, var, &cond)) && cond) {
                    IUIAutomationElement* btn = NULL;
                    if (SUCCEEDED(el->FindFirst(TreeScope_Subtree, cond, &btn))) {
                        if (btn) {
                            // invoke
                            IUIAutomationInvokePattern*  inv;
                            btn->GetCurrentPatternAs(UIA_InvokePatternId, IID_PPV_ARGS(&inv));
                            if (inv) { 
                                inv->Invoke();
                                inv->Release();
                            }
                        }
                    }
                    cond->Release();
                }
                SysFreeString(var.bstrVal);
            }
        }
        break;
    }

    if (top) ::SetForegroundWindow(top);
    if (descr->pause) Sleep(descr->pause);
    return 2;
}
