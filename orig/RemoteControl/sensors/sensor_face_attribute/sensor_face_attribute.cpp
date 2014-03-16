/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/

#include "util_pipeline.h"
#include "..\..\AUXlib\render_face.h"
#include "sensor_face_attribute.hpp"
#include "..\..\src\invoker.h"

extern CInvoker g_Invoker;

#define MY_SENSOR_ID CSensor::Sensor_FaceAttributes
#define MY_EVENT_FACE_ON  1
#define MY_EVENT_FACE_OFF 2
/* no more supported
#define MY_EVENT_FACE_SMILE_ON 3
#define MY_EVENT_FACE_SMILE_OFF 4
*/

class FaceAttributePipeline: public UtilPipeline
{
public:
    FaceAttributePipeline(bool videoControl):UtilPipeline() 
    {
        m_id = MY_SENSOR_ID;
        m_render = NULL;
        if (videoControl) m_render = new FaceRender(1, 0, L"Image Viewer");
        EnableFaceLocation();
        m_isFace = false;
/* no more supported
        m_isSmile = false;
*/
        m_lastTime[0] = 0;
        m_lastTime[1] = 0;
    }
    virtual void Close(void) 
    {
        __super::Close();
        if (m_render) delete m_render;
        delete this;
    }
    virtual void  OnFaceSetup(PXCFaceAnalysis::ProfileInfo *finfo) 
    {
/* no more supported
        PXCFaceAnalysis* face = QueryFace();
        if (face) {
            PXCFaceAttribute *attribute = face->DynamicCast<PXCFaceAttribute>();
            if (attribute) {
                PXCFaceAttribute::ProfileInfo pinfo;
                attribute->QueryProfile(PXCFaceAttribute::LABEL_EMOTION,&pinfo);
                pinfo.threshold = 50;
                attribute->SetProfile(PXCFaceAttribute::LABEL_EMOTION,&pinfo);
            }
        }
*/
    }

    virtual bool OnNewFrame(void) 
    {
        if (!g_Invoker.IsRunning(m_id)) return false;
        PXCFaceAnalysis* face = QueryFace();
        pxcUID fid = 0;
        if (face) face->QueryFace(0, &fid);

        // check face ON / OFF
        bool isFace = face && fid>0;
        if (m_isFace != isFace) {
            // simple time filter
            if (GetTickCount()-m_lastTime[0] > 500) {
                if (isFace) g_Invoker.ExecuteEvent(m_id, MY_EVENT_FACE_ON); 
                else g_Invoker.ExecuteEvent(m_id, MY_EVENT_FACE_OFF);
                m_isFace = isFace;
                m_lastTime[0] = GetTickCount();
            }
        }

/* no more supported
        // check smile
        bool isSmile = false;
        if (m_isFace && face && fid) {
            PXCFaceAttribute *attribute = face->DynamicCast<PXCFaceAttribute>();
            PXCFaceAttribute::ProfileInfo pinfo;
            attribute->QueryProfile(PXCFaceAttribute::LABEL_EMOTION,&pinfo);
            pxcU32 scores[4];
            if (attribute->QueryData(PXCFaceAttribute::LABEL_EMOTION,fid,scores) >= PXC_STATUS_NO_ERROR) {
                isSmile = scores[PXCFaceAttribute::INDEX_EMOTION_SMILE]>pinfo.threshold;
            }
        }
        if (m_isSmile != isSmile) {
            if (GetTickCount()-m_lastTime[1] > 500) {
                // simple time filter
                if (m_isSmile) {
                    if (!isSmile) g_Invoker.ExecuteEvent(m_id, MY_EVENT_FACE_SMILE_OFF); 
                } else if (isSmile) g_Invoker.ExecuteEvent(m_id, MY_EVENT_FACE_SMILE_ON);
                m_isSmile = isSmile;
                m_lastTime[1] = GetTickCount();
            }
        }
*/

        if (m_render) {
            m_render->ClearData();
            if (face && fid) {
                PXCFaceAnalysis::Detection *faceDetector=face->DynamicCast<PXCFaceAnalysis::Detection>();
                if (faceDetector) {
                    PXCFaceAnalysis::Detection::Data face_data;
                    faceDetector->QueryData(fid, &face_data);
                    m_render->SetDetectionData(&face_data);
                }
            }
            m_render->UpdateFrame(QueryImage(PXCImage::IMAGE_TYPE_COLOR));
        }

        return true;
    }

protected:
    CSensor::SENSOR_ID  m_id;
    FaceRender*     m_render;
    bool m_isFace;
/* no more supported
    bool m_isSmile;
*/
    DWORD m_lastTime[2];
};

///////////////////////

int CSensorFaceAttribute::OnInit()
{
    wsprintf(m_name, L"Face Recognition");
    m_id = MY_SENSOR_ID;

    m_eventList.push_back(st_eventDescriptor( MY_EVENT_FACE_ON,     L"Face ON" ));
    m_eventList.push_back(st_eventDescriptor( MY_EVENT_FACE_OFF,    L"Face OFF" ));
/* no more supported
    m_eventList.push_back(st_eventDescriptor( MY_EVENT_FACE_SMILE_ON,   L"Smile ON" ));
    m_eventList.push_back(st_eventDescriptor( MY_EVENT_FACE_SMILE_OFF,  L"Smile OFF" ));
*/
    return 0;
}

void CSensorFaceAttribute::OnStart()
{
    m_pipeline = new FaceAttributePipeline(this->video);
}

void CSensorFaceAttribute::OnStop()
{
    FaceAttributePipeline* pipeline = (FaceAttributePipeline*)m_pipeline;
}

