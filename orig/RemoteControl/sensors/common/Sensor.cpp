/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/

#include <Windows.h>
#include "Sensor.h"
#include "Thread.h"

#include <Strsafe.h>
#include "util_pipeline.h"

st_eventDescriptor::st_eventDescriptor(int _id, WCHAR* str) {
    id=_id;
    StringCchPrintf(name, MAX_PATH, L"%s",str); 
}

CSensor::CSensor(void)
{
	m_pipeline = NULL;
	m_isEnable = true;
	m_isRun = false;
	OnInit();
}

CSensor::~CSensor(void)
{
	Release();
}

void CSensor::Release(void)
{
	OnStop();
}

BOOL CSensor::GetEvent(unsigned int num, st_eventDescriptor* descr)
{
	if (num>=m_eventList.size()) return false;
	*descr = m_eventList[num];
	return TRUE;
}

BOOL CSensor::GetEventbyId(int id, st_eventDescriptor* descr)
{
    for (unsigned int i=0; i<m_eventList.size(); i++) {
        if (m_eventList.at(i).id == id) {
	        *descr = m_eventList[i];
            return TRUE;
        }
    }
    return FALSE;
}

int  CSensor::GetEventId(unsigned int num)
{
	if (num>=m_eventList.size()) return 0;
	return m_eventList[num].id;
}

int CSensor::AddEvent(WCHAR* eventName)
{
	int id = (int)m_eventList.size();
	if (id>0) id = m_eventList[id-1].id+1;
	m_eventList.push_back(st_eventDescriptor( id, eventName ));
	return id;
}

void CSensor::AddEvent(st_eventDescriptor* event)
{
	m_eventList.push_back(*event);
}

void CSensor::RunThread(void)
{
	m_isRun = true;
	OnStart();
	if (m_pipeline) {
		((UtilPipeline*)m_pipeline)->LoopFrames();
	}
}

void CSensor::Stop() 
{
	__super::Stop();
	OnStop();
	m_isRun = false;
}

void CSensor::Start(BOOL videoControl) 
{
	__super::Start(videoControl);
}
