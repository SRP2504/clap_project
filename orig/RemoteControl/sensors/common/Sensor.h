/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/

#pragma once
#include <vector>
#include "thread.h"

struct st_eventDescriptor {
    int id;
    WCHAR name[MAX_PATH];
    st_eventDescriptor(int _id, WCHAR* str);
    st_eventDescriptor() {};
};

class CSensor : public Thread
{
public:
    enum SENSOR_ID {
        Sensor_FaceAttributes = 1,
        Sensor_Gesture_detector,
        Sensor_Voice_recognition,
    };
public:
    CSensor();
    virtual ~CSensor();
    virtual void Release();
    virtual void RunThread(void);

    virtual void OnStart() {};
    virtual void OnStop() {};
    virtual int  OnInit() {return 1;};

    virtual void Stop();
    virtual void Start(BOOL videoControl);

    WCHAR* GetName() { return m_name; };
    int AddEvent(WCHAR* eventName);
    void AddEvent(st_eventDescriptor* event);
    
    int  GetEventId(unsigned int num);
    BOOL GetEventbyId(int id, st_eventDescriptor* descr);
    BOOL GetEvent(unsigned int num, st_eventDescriptor* descr);
    SENSOR_ID GetId() { return m_id;};
    BOOL IsRun() {return m_isRun;};
    BOOL IsEnable() {return m_isEnable;};
    BOOL Enable(BOOL enable=true) {BOOL ret=m_isEnable; m_isEnable=enable; return ret;};
    std::vector<st_eventDescriptor> m_eventList;

protected:
    BOOL  m_isEnable;
    BOOL  m_isRun;
    void* m_pipeline;
    WCHAR m_name[MAX_PATH];
    SENSOR_ID m_id;
};

