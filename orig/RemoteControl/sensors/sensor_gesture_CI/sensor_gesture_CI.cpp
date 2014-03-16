/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/

#include "util_pipeline.h"
#include "..\..\AUXlib\render_gesture.h"
#include "..\..\AUXlib\render_blob.h"
#include "..\..\src\dispatcher.h"
#include "..\..\src\invoker.h"

#include "sensor_gesture_CI.hpp"

extern CInvoker g_Invoker;

#define MY_SENSOR_ID CSensor::Sensor_Gesture_detector
#define MY_EVENT_THUMBUP 1
#define MY_EVENT_THUMBDOWN 2
#define MY_EVENT_PEACE 3
#define MY_EVENT_LEFT 4
#define MY_EVENT_RIGHT 5
#define MY_EVENT_WAVE 6
#define MY_EVENT_HAND_ON 7
#define MY_EVENT_HAND_OFF 8
#define MY_EVENT_UP 9
#define MY_EVENT_DOWN 10
#define MY_EVENT_CIRCLE 11
#define MY_EVENT_BIG5 12

class GestureCIPipeline: public UtilPipeline
{
public:
    GestureCIPipeline(bool videoControl):UtilPipeline() 
    {
        m_id = MY_SENSOR_ID;
        m_render = NULL;
        m_renderB = NULL;
        if (videoControl) {
            m_render = new CAUXGestureRender(1, 1, L"Gesture Viewer");
            m_renderB = new CAUXBlobRender(2, 1, L"Blob Viewer");
        }
        EnableGesture();
        m_isHand = false;
    }
    virtual void Close(void) 
    {
        __super::Close();
        if (m_render) delete m_render;
        if (m_renderB) delete m_renderB;
        delete this;
    }
    virtual void PXCAPI OnGesture(PXCGesture::Gesture *data) 
    {
        if (data->active) { 
            m_gdata=(*data);
            switch (m_gdata.label) {
            case PXCGesture::Gesture::LABEL_HAND_WAVE:
                g_Invoker.ExecuteEvent(m_id, MY_EVENT_WAVE); 
                break;
            case PXCGesture::Gesture::LABEL_NAV_SWIPE_LEFT:
                g_Invoker.ExecuteEvent(m_id, MY_EVENT_LEFT); 
                break;
            case PXCGesture::Gesture::LABEL_NAV_SWIPE_RIGHT:
                g_Invoker.ExecuteEvent(m_id, MY_EVENT_RIGHT); 
                break;
            case PXCGesture::Gesture::LABEL_POSE_THUMB_UP:
                g_Invoker.ExecuteEvent(m_id, MY_EVENT_THUMBUP); 
                break;
            case PXCGesture::Gesture::LABEL_POSE_THUMB_DOWN:
                g_Invoker.ExecuteEvent(m_id, MY_EVENT_THUMBDOWN); 
                break;
            case PXCGesture::Gesture::LABEL_POSE_PEACE:
                g_Invoker.ExecuteEvent(m_id, MY_EVENT_PEACE); 
                break;
            case PXCGesture::Gesture::LABEL_HAND_CIRCLE:
                g_Invoker.ExecuteEvent(m_id, MY_EVENT_CIRCLE); 
                break;
            case PXCGesture::Gesture::LABEL_NAV_SWIPE_UP:
                g_Invoker.ExecuteEvent(m_id, MY_EVENT_UP); 
                break;
            case PXCGesture::Gesture::LABEL_NAV_SWIPE_DOWN:
                g_Invoker.ExecuteEvent(m_id, MY_EVENT_DOWN); 
                break;
            case PXCGesture::Gesture::LABEL_POSE_BIG5:
                g_Invoker.ExecuteEvent(m_id, MY_EVENT_BIG5); 
                break;
            }
        }
    }
    virtual bool OnNewFrame(void) 
    {
        pxcUID user=0;
        if (!g_Invoker.IsRunning(m_id)) return false;
        BOOL isHand = false;
        PXCGesture::GeoNode::Label hand = PXCGesture::GeoNode::LABEL_BODY_HAND_LEFT;
        for (int u=0;;u++) {
            pxcStatus sts= m_gesture->QueryUser(u,&user);
            if (sts<PXC_STATUS_NO_ERROR) break;
            PXCGesture::GeoNode data;
            sts = m_gesture->QueryNodeData(user, hand, &data);
            if (sts==PXC_STATUS_NO_ERROR) { 
                isHand = true;
            } else {
                // try right hand
                hand = PXCGesture::GeoNode::LABEL_BODY_HAND_RIGHT;
                sts = m_gesture->QueryNodeData(user, hand, &data);
                if (sts==PXC_STATUS_NO_ERROR) { 
                    isHand = true;
                    break;
                }
            }
        }
        if (isHand!=m_isHand) {
            if (isHand) g_Invoker.ExecuteEvent(m_id, MY_EVENT_HAND_ON); 
            else g_Invoker.ExecuteEvent(m_id, MY_EVENT_HAND_OFF); 
            m_isHand = isHand;
        }
        
        if (m_render) m_render->UpdateFrame(QueryImage(PXCImage::IMAGE_TYPE_DEPTH), QueryGesture(), &m_gdata);
        if (m_renderB) m_renderB->UpdateFrame(NULL, QueryGesture(), &m_gdata);

        return true;
    }
protected:
    CSensor::SENSOR_ID  m_id;
    CAUXGestureRender*  m_render;
    CAUXBlobRender*     m_renderB;
    PXCGesture::Gesture m_gdata;
    BOOL m_isHand;
};


///////////////////////

int CSensorGestureCI::OnInit()
{
    wsprintf(m_name, L"Gesture Recognition");
    m_id = MY_SENSOR_ID;
    m_eventList.push_back(st_eventDescriptor( MY_EVENT_HAND_ON,     L"Hand ON" ));
    m_eventList.push_back(st_eventDescriptor( MY_EVENT_HAND_OFF,    L"Hand OFF" ));
    m_eventList.push_back(st_eventDescriptor( MY_EVENT_BIG5,        L"Palm (Big5)" ));
    m_eventList.push_back(st_eventDescriptor( MY_EVENT_THUMBUP,     L"Thumb Up" ));
    m_eventList.push_back(st_eventDescriptor( MY_EVENT_THUMBDOWN,   L"Thumb Down" ));
    m_eventList.push_back(st_eventDescriptor( MY_EVENT_PEACE,       L"\"V\" (Peace)" ));
    m_eventList.push_back(st_eventDescriptor( MY_EVENT_LEFT,        L"Left" ));
    m_eventList.push_back(st_eventDescriptor( MY_EVENT_RIGHT,       L"Right" ));
    m_eventList.push_back(st_eventDescriptor( MY_EVENT_UP,          L"Up" ));
    m_eventList.push_back(st_eventDescriptor( MY_EVENT_DOWN,        L"Down" ));
    m_eventList.push_back(st_eventDescriptor( MY_EVENT_CIRCLE,      L"Circle" ));
    m_eventList.push_back(st_eventDescriptor( MY_EVENT_WAVE,        L"Wave" ));

    return 0;
}

void CSensorGestureCI::OnStart()
{
    m_pipeline = new GestureCIPipeline(this->video);
}

void CSensorGestureCI::OnStop()
{
    GestureCIPipeline* pipeline = (GestureCIPipeline*)m_pipeline;
}

