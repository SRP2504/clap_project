/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2012 Intel Corporation. All Rights Reserved.

*******************************************************************************/
#include "util_pipeline_object_tracking.h"
#include <windows.h>

bool UtilPipelineObjectTracking::StackableCreate(PXCSession *session) 
{
    if (m_tracking_enabled) {
        pxcStatus sts;
        sts=session->CreateImpl(&m_tracker_mdesc,PXCDCVTracker::CUID,(void**)&m_tracker);
        if (sts<PXC_STATUS_NO_ERROR) return false;
        sts = m_tracker->QueryProfile(&m_tracker_pinfo);//make sure it is initialized
        if (sts<PXC_STATUS_NO_ERROR) return false;
    }
	return UtilPipelineStackable::StackableCreate(session);
}

bool UtilPipelineObjectTracking::InitTracker( const pxcCHAR *filename, int width, int height )
{
    if (m_tracker) 
    {
        m_tracker_pinfo.width = width;
        m_tracker_pinfo.height = height;
        pxcStatus sts=m_tracker->SetProfile(filename, &m_tracker_pinfo);
        if (sts<PXC_STATUS_NO_ERROR) return false;
    }
    return true;
}

bool UtilPipelineObjectTracking::StackableReadSample(UtilCapture *capture,PXCSmartArray<PXCImage> &images,PXCSmartSPArray &sps,pxcU32 isps) 
{
    if (m_tracker && !m_pause) {
        PXCCapture::VideoStream::Images images2;
        capture->MapImages(m_tracker_stream_index,images,images2);
        pxcStatus sts=m_tracker->ProcessImageAsync(images2,sps.ReleaseRef(isps));
		if (sts<PXC_STATUS_NO_ERROR) return false;
    }
    return UtilPipelineStackable::StackableReadSample(capture,images,sps,isps);
}

void UtilPipelineObjectTracking::StackableCleanUp(void) {
	if (m_tracker)	{ 
		m_tracker->Release(); m_tracker=0;      
	}
	UtilPipelineStackable::StackableCleanUp();
}

pxcStatus UtilPipelineObjectTracking::StackableSearchProfiles(UtilCapture *uc, std::vector<PXCCapture::VideoStream::DataDesc*> &vinputs, int vidx, std::vector<PXCCapture::AudioStream::DataDesc*> &ainputs, int aidx) 
{
	if (!m_tracker) return UtilPipelineStackable::StackableSearchProfiles(uc,vinputs,vidx,ainputs,aidx);

	m_tracker_stream_index=vidx;
	
	pxcStatus sts=m_tracker->QueryProfile(&m_tracker_pinfo);
	if (sts == PXC_STATUS_NO_ERROR) 
    {
        if (vidx>=(int)vinputs.size()) vinputs.push_back(&m_tracker_pinfo.inputs);
        else vinputs[vidx]=&m_tracker_pinfo.inputs;

        sts=UtilPipelineStackable::StackableSearchProfiles(uc,vinputs,vidx+1,ainputs,aidx);
        if (sts>=PXC_STATUS_NO_ERROR) return sts;
        if (sts!=PXC_STATUS_ITEM_UNAVAILABLE) return sts;

        sts=uc->LocateStreams(vinputs,ainputs);
        if (sts>=PXC_STATUS_NO_ERROR) return PXC_STATUS_NO_ERROR;
    }

	return PXC_STATUS_PARAM_UNSUPPORTED;
}

void UtilPipelineObjectTracking::EnableTracking(pxcUID iuid)
{
	m_tracking_enabled=true;
	m_tracker_mdesc.iuid=iuid;
}

void UtilPipelineObjectTracking::EnableTracking(pxcCHAR *name) 
{
	m_tracking_enabled=true;
	wcscpy_s<sizeof(m_tracker_mdesc.friendlyName)/sizeof(pxcCHAR)>(m_tracker_mdesc.friendlyName,name);
}

UtilPipelineObjectTracking::UtilPipelineObjectTracking(UtilPipelineStackable *next):UtilPipelineStackable(next) 
{
	memset(&m_tracker_mdesc,0,sizeof(m_tracker_mdesc));
    
	m_tracking_enabled=false;
	m_tracker=0;
	m_pause=false;
}
