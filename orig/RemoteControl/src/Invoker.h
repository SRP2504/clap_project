/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/

#pragma once

#include <map>
#include "Dispatcher.h"
#include "..\sensors\common\sensor.h"
#include "..\AUXlib\AUXTextRender.h"

class MyVec2
{
public:
    float   x;
    float   y;  

    MyVec2() {}
    MyVec2( float xx, float yy )  {
        x = xx;
        y = yy;
    };
    float   length() const  {
        return sqrtf(x * x + y * y);
    };
    MyVec2& operator-() {
        x = -x;
        y = -y;         
        return (*this);
    };
    MyVec2 operator+ (const MyVec2& other) const {
        return MyVec2(x + other.x, y + other.y);
    };
    MyVec2 operator- (const MyVec2& other) const {
        return MyVec2(x - other.x, y - other.y);
    };
    MyVec2 operator* (float scale) const {
        return MyVec2(x * scale, y * scale);
    };
    MyVec2 operator/ (float invscale) const {
        float scale = 1 / invscale;
        return MyVec2(x * scale, y * scale);
    };
};

class CInvoker
{
public:
    CInvoker(void);
    ~CInvoker(void);
    
    IUIAutomation* GetAutomation();
    bool StartSensor(CSensor::SENSOR_ID id, BOOL videoControl=1);
    bool StopSensor(CSensor::SENSOR_ID id);
    int  StartAllSensors(BOOL videoControl=1);
    void StopAllSensors();

    bool IsRunning(CSensor::SENSOR_ID id);
    void OnTimer(void);

    int  ExecuteEvent(CSensor::SENSOR_ID sensor_id, int event_id, WCHAR* event_name=NULL);

    BOOL Activate(stActionDescriptor* descr, HWND hWnd);

    int  ExecuteAction(int actionId);
    int  ExecuteAction(stActionDescriptor* descr, BOOL enable=FALSE, WCHAR* event_name=NULL);

    void Lock();
    void UnLock();

protected:
    IUIAutomation* m_automation;
    CRITICAL_SECTION m_mutex;

    CAUXTextRender* m_renderG;
    CAUXTextRender* m_renderF;
    CAUXTextRender* m_renderV;
};

