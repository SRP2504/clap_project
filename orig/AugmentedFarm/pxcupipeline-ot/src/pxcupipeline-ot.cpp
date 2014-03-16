/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2012 Intel Corporation. All Rights Reserved.

*******************************************************************************/
#include <vector>
#include "util_pipeline_face.h"
#include "util_pipeline_gesture.h"
#include "util_pipeline_raw.h"
#include "util_pipeline_voice.h"
#include "util_pipeline_object_tracking.h"
#include "pxcupipeline-ot.h"
#include "ippi.h"
#include "ippcore.h"
#include "pxcprojection.h"
#include "pxcmetadata.h"
#include <tchar.h>

struct _PXCUPipeline_Instance {
	_PXCUPipeline_Instance(pxcCHAR *filename=NULL) : pipeline() {};
	class UtilPipelinePXCU: public UtilPipelineVoice, public UtilPipelineGesture, public UtilPipelineFace, public UtilPipelineObjectTracking, public UtilPipelineRaw {
    public:
        UtilPipelinePXCU(void):UtilPipelineVoice(),UtilPipelineGesture(),UtilPipelineFace(),UtilPipelineObjectTracking(),UtilPipelineRaw(0,0,false,5) { 
			UtilPipelineGesture::m_next=dynamic_cast<UtilPipelineVoice*>(this);
			UtilPipelineObjectTracking::m_next=dynamic_cast<UtilPipelineGesture*>(this);
			UtilPipelineFace::m_next=dynamic_cast<UtilPipelineObjectTracking*>(this);
			UtilPipelineRaw::m_next=dynamic_cast<UtilPipelineFace*>(this);
			m_data.timeStamp=0; 
			memset(&m_vrec_pinfo,0,sizeof(m_vrec_pinfo));
		}
		virtual void OnVoiceRecognitionSetup(PXCVoiceRecognition::ProfileInfo *pinfo) { if (m_vrec_pinfo.language) pinfo->language=m_vrec_pinfo.language; }
	    virtual void PXCAPI OnRecognized(PXCVoiceRecognition::Recognition *data) { m_data=(*data); }
        virtual void PXCAPI OnAlert(PXCVoiceRecognition::Alert *data) {
            m_data.timeStamp=data->timeStamp;
            for (int i=0;i<4;i++) {
                m_data.nBest[i].confidence=0;
                m_data.nBest[i].label= -1;
            }
            switch (data->label) {
            case PXCVoiceRecognition::Alert::LABEL_VOLUME_HIGH:
                wcscpy_s<sizeof(m_data.dictation)/sizeof(pxcCHAR)>(m_data.dictation,L"!VOLUME_HIGH");
                break;
            case PXCVoiceRecognition::Alert::LABEL_VOLUME_LOW:
                wcscpy_s<sizeof(m_data.dictation)/sizeof(pxcCHAR)>(m_data.dictation,L"!VOLUME_LOW"); 
                break;
            case PXCVoiceRecognition::Alert::LABEL_SNR_LOW:
                wcscpy_s<sizeof(m_data.dictation)/sizeof(pxcCHAR)>(m_data.dictation,L"!SNR_LOW");
                break;
            case PXCVoiceRecognition::Alert::LABEL_SPEECH_UNRECOGNIZABLE:
                wcscpy_s<sizeof(m_data.dictation)/sizeof(pxcCHAR)>(m_data.dictation,L"!SPEECH_UNRECOGNIZABLE"); 
                break;
            default:
                wcscpy_s<sizeof(m_data.dictation)/sizeof(pxcCHAR)>(m_data.dictation,L"!UNKNOWN");
            }
        }
        PXCVoiceRecognition::Recognition m_data;
		PXCVoiceRecognition::ProfileInfo m_vrec_pinfo;
    } pipeline;

    std::vector<std::wstring> strings;
	PXCSmartPtr<PXCProjection> projection;
};

#define PXCUPIPELINE_HEADERS
#define PXCUPIPELINE_INSTANCE
#define PXCUPIPELINE_INIT
#define PXCUPIPELINE_PAUSEFRAME
#include "pxcupipeline.cpp"

bool __stdcall PXCUPipeline_Init(PXCUPipeline_Instance instance, PXCUPipeline mode) {
    if (mode&PXCU_PIPELINE_OBJECT_TRACKING)     instance->pipeline.EnableTracking();
    return Init(instance,mode);
}

bool __stdcall PXCUPipeline_InitTracker( PXCUPipeline_Instance instance , pxcCHAR *filename , int width, int height ) {
    if (instance->pipeline.IsDisconnected()) return false;
    PXCDCVTracker *tracker = instance->pipeline.QueryTracker();
    if (!tracker) return false;
    return ( instance->pipeline.InitTracker(filename, width, height) );
}

bool __stdcall PXCUPipeline_QueryTargetData(PXCUPipeline_Instance instance,int targetIndex, PXCDCVTracker::TargetData* targetData)
{
    if (instance->pipeline.IsDisconnected()) return false;
    PXCDCVTracker *tracker = instance->pipeline.QueryTracker();
    if (!tracker) return false;

    return ( tracker->GetTargetData(targetIndex,targetData) >=PXC_STATUS_NO_ERROR ) ;
}

bool __stdcall PXCUPipeline_QueryTargetName(PXCUPipeline_Instance instance,int targetIndex, pxcCHAR* name)
{
    if (instance->pipeline.IsDisconnected()) return false;
    PXCDCVTracker *tracker = instance->pipeline.QueryTracker();
    if (!tracker) return false;
    const pxcCHAR* tmp = tracker->GetTargetName(targetIndex);
    if (tmp==NULL) return false;
	wcscpy_s(name,wcslen(tmp)+1,tmp);
    return true;
}

bool __stdcall PXCUPipeline_QueryTargetType(PXCUPipeline_Instance instance,int targetIndex, PXCDCVTracker::TargetType* targetType)
{
    if (instance->pipeline.IsDisconnected()) return false;
    PXCDCVTracker *tracker = instance->pipeline.QueryTracker();
    if (!tracker) return false;

    *targetType = tracker->GetTargetType(targetIndex);
    return (*targetType != PXCDCVTracker::TARGET_UNDEFINED);
}

bool __stdcall PXCUPipeline_QueryTrackedKeyFrameIndex(PXCUPipeline_Instance instance,int targetIndex, int* frameIndex)
{
    if (instance->pipeline.IsDisconnected()) return false;
    PXCDCVTracker *tracker = instance->pipeline.QueryTracker();
    if (!tracker) return false;
    *frameIndex = tracker->GetTrackedKeyFrameIndex(targetIndex);

    return (*frameIndex != -1);
}

void __stdcall PXCUPipeline_PauseFrame(PXCUPipeline_Instance instance, PXCUPipeline mode, bool pause) {
	PauseFrame(instance,mode,pause);
	if (mode&PXCU_PIPELINE_OBJECT_TRACKING)		instance->pipeline.PauseObjectTracking(pause);
}

