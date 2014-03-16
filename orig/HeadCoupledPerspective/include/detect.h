//--------------------------------------------------------------------------------------
// Copyright 2012 Intel Corporation
// All Rights Reserved
//
// Permission is granted to use, copy, distribute and prepare derivative works of this
// software for any purpose and without fee, provided, that the above copyright notice
// and this statement appear in all copies.  Intel makes no representations about the
// suitability of this software for any purpose.  THIS SOFTWARE IS PROVIDED "AS IS."
// INTEL SPECIFICALLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED, AND ALL LIABILITY,
// INCLUDING CONSEQUENTIAL AND OTHER INDIRECT DAMAGES, FOR THE USE OF THIS SOFTWARE,
// INCLUDING LIABILITY FOR INFRINGEMENT OF ANY PROPRIETARY RIGHTS, AND INCLUDING THE
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  Intel does not
// assume any responsibility for any errors which may appear in this software nor any
// responsibility to update it.
//--------------------------------------------------------------------------------------

#include <Windows.h>
#include <string>

#include "util_pipeline.h"
#include "face_render.h"
#include "hcdUtil.h"

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)         { if((p)){ (p)->Release(); (p)=NULL; } }
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)          { if((p)){ delete (p); (p)=NULL; } }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p)    { if((p)){ delete[](p); (p)=NULL; } }
#endif

class FacePipeline : public UtilPipeline 
{

public:
    FacePipeline(void);
	~FacePipeline(); 

    virtual bool OnNewFrame(void);
    virtual void OnFaceSetup(PXCFaceAnalysis::ProfileInfo *pinfo);
    virtual void OnFaceLandmarkSetup(PXCFaceAnalysis::Landmark::ProfileInfo *pinfo);

    bool GetFaceData(DetectionData &detData, double elapsedTime);
    void EnableCameraWindow(bool enable);

private:
    void UpdateDetectionData(float elapsedTime = 1.0f);

protected:
    static const UINT32                     m_cNumLandmarks = 6;
    PXCFaceAnalysis::Detection::Data        m_FaceData;
    PXCFaceAnalysis::Landmark::LandmarkData m_LandmarkData[m_cNumLandmarks];
    bool            fdValid;                    // valid face data
    bool            ldValid[m_cNumLandmarks];   // valid landmark data 

    DetectionData   m_SnapData, m_SmoothData;
	FaceRender*     m_pFaceRender;
    Timer           m_timer;
};

