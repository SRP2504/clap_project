/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2012 Intel Corporation. All Rights Reserved.

*******************************************************************************/
#ifndef __UTIL_PIPELINE_OBJECT_TRACKING_H__
#define __UTIL_PIPELINE_OBJECT_TRACKING_H__
#include "util_pipeline_stackable.h"
#include "pxcdcvtracker.h"

class UtilPipelineObjectTracking :protected UtilPipelineStackable {
public:

	UtilPipelineObjectTracking(UtilPipelineStackable *next=0);

    virtual bool  InitTracker( const pxcCHAR *filename ,  int width, int height);
	virtual void  EnableTracking(pxcUID iuid=0);
	virtual void  EnableTracking(pxcCHAR *name);

	virtual void  OnTrackingSetup(PXCDCVTracker::ProfileInfo *pinfo) {}
	virtual void  PauseObjectTracking(bool pause) { m_pause=pause; }
	
	virtual PXCDCVTracker*	QueryTracker(void)	{ return m_tracker; }

protected:

	PXCDCVTracker*					m_tracker;
	PXCDCVTracker::ProfileInfo		m_tracker_pinfo;
	bool							m_tracking_enabled;
	int								m_tracker_stream_index;
	PXCSession::ImplDesc			m_tracker_mdesc;
	bool							m_pause;

	virtual bool      StackableCreate(PXCSession *session);
	virtual pxcStatus StackableSearchProfiles(UtilCapture *uc, std::vector<PXCCapture::VideoStream::DataDesc*> &vinputs, int vidx, std::vector<PXCCapture::AudioStream::DataDesc*> &ainputs, int aidx);
	virtual bool      StackableReadSample(UtilCapture *capture,PXCSmartArray<PXCImage> &images,PXCSmartSPArray &sps,pxcU32 isps);
	virtual void      StackableCleanUp(void);
};

#endif
