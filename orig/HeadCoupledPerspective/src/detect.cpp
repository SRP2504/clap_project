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

#include "detect.h"

extern bool g_EnableDepthFromCamera;    // access global from hcd.cpp


static pxcEnum lmLabel[7] = 
{
    PXCFaceAnalysis::Landmark::LABEL_LEFT_EYE_OUTER_CORNER   ,
    PXCFaceAnalysis::Landmark::LABEL_LEFT_EYE_INNER_CORNER   ,
    PXCFaceAnalysis::Landmark::LABEL_RIGHT_EYE_OUTER_CORNER  ,
    PXCFaceAnalysis::Landmark::LABEL_RIGHT_EYE_INNER_CORNER  ,
    PXCFaceAnalysis::Landmark::LABEL_MOUTH_LEFT_CORNER       ,
    PXCFaceAnalysis::Landmark::LABEL_MOUTH_RIGHT_CORNER      ,
    PXCFaceAnalysis::Landmark::LABEL_NOSE_TIP                
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
FacePipeline::FacePipeline(void) :  m_pFaceRender(0)
{
    fdValid = false;
    for (int i = 0; i < m_cNumLandmarks; i++) ldValid[i] = false;

    // Init with some sane face location params - minimize initial jolt
    m_SnapData.faceX = 320.0f;
    m_SnapData.faceY = 240.0f;
    m_SnapData.imageWidth = 640;
    m_SnapData.imageHeight = 480;
    m_SnapData.zoom = 1.0f;
    m_SmoothData = m_SnapData;

    EnableFaceLocation();
    EnableFaceLandmark();
}

//-----------------------------------------------------------------------------
FacePipeline::~FacePipeline() 
{
    SAFE_DELETE(m_pFaceRender);
}

//-----------------------------------------------------------------------------
// Show/Hide camera feedback window
void FacePipeline::EnableCameraWindow(bool enable)
{
    if (enable)
    {
        m_pFaceRender = NULL; 
        m_pFaceRender = new FaceRender(L"Face Detection");
    }
    else
    {
        SAFE_DELETE(m_pFaceRender);
    }
}

//----------------------------------------------------------------------------------
// Process a new camera frame, query the PC-SDK for all available face/landmark data
//---------------------------------------------------------------------------------- 
bool FacePipeline::OnNewFrame(void) 
{
    bool faceDetect = false;

    // PC-SDK defined data structs
    PXCFaceAnalysis::Detection::Data        faceData;
    PXCFaceAnalysis::Landmark::LandmarkData lmData;

    float captureTime = (float) m_timer.GetTotal();

    // Get face, landmark interfaces
    PXCFaceAnalysis *faceAnalyzer = QueryFace();
    PXCFaceAnalysis::Detection *pDetectIfc = faceAnalyzer->DynamicCast<PXCFaceAnalysis::Detection>();
    PXCFaceAnalysis::Landmark  *pLandmarkIfc = faceAnalyzer->DynamicCast<PXCFaceAnalysis::Landmark>();

    // clear feedback window, if enabled
    if (m_pFaceRender) m_pFaceRender->ClearData();

    // Request the first face in the frame (TODO: track a 'primary' face, if multiple are found)
    pxcUID faceID = 0;
    pxcStatus sts = faceAnalyzer->QueryFace(0, &faceID);   // get first face index (increments over time)

    if (PXC_STATUS_NO_ERROR == sts) // Found a face
    {
        // Query face data
        faceDetect = true;
        pxcStatus retCode = pDetectIfc->QueryData(faceID, &faceData);
        if (PXC_STATUS_NO_ERROR == retCode)
        {
            m_FaceData = faceData;  // copy updated face position data
            fdValid = true;         // mark as valid
        } else {
            fdValid = false;
        }
  
        // Query each individual landmark by label, copy all those available
        for (int i = 0; i < m_cNumLandmarks; i++)
        {
            retCode = pLandmarkIfc->QueryLandmarkData(faceID, lmLabel[i], &lmData); // Query by label
            if (PXC_STATUS_NO_ERROR == retCode)
            {
                m_LandmarkData[i] = lmData; // copy updated landmark data
                ldValid[i] = true;          // mark valid
            } else {
                ldValid[i] = false;
            }
        }

        // Draw face and landmark data in feedback window, if enabled
        if (m_pFaceRender) 
        {
            m_pFaceRender->SetDetectionData( &faceData );   
            if (g_EnableDepthFromCamera) m_pFaceRender->SetLandmarkData( pLandmarkIfc, faceID );

            // Update window
            m_pFaceRender->RenderFrame( QueryImage( PXCImage::IMAGE_TYPE_COLOR ) );
        }

        // Measure face location and landmark spacings, and update snapshot data
        UpdateDetectionData(captureTime);   
    }
    else 
    {
        DBGOUT("No face data - capture time %3.1f ms\n", captureTime * 1000.0f);
    }

    m_timer.Reset();
    m_timer.Start();

    return true;
}

//-----------------------------------------------------------------------------
// Initialization callback
void FacePipeline::OnFaceSetup(PXCFaceAnalysis::ProfileInfo *pinfo)
{
    // Modify Analysis Profile info to our liking
    if (pinfo) pinfo->iftracking = true;
}

//-----------------------------------------------------------------------------
// Initialization callback
void FacePipeline::OnFaceLandmarkSetup(PXCFaceAnalysis::Landmark::ProfileInfo *pinfo)
{
    // Modify Landmark profile info to our liking
    if (pinfo) pinfo->labels = PXCFaceAnalysis::Landmark::LABEL_6POINTS;   // All but nose
}

//-----------------------------------------------------------------------------
// Check for new face data from PC-SDK, blend latest snapshot into the smoothed data
//   SnapData has the last face/landmark data found
//   SmoothData has a time-weighted average of recent face/lm data
#define WMERGE(v,n,w) v=(1.0f-(w))*(v)+(w)*(float)(n)   // lerp, new data into existing with weight w
bool FacePipeline::GetFaceData(DetectionData &detData, double elapsedTime)
{
    bool faceDetect = false;

    // Check for new frame, but dont wait if not available
    if ( AcquireFrame(false) )    
    {
        // New frame available - process it for face/landmark data
        faceDetect = OnNewFrame();
        ReleaseFrame();
    }
    else    
    {
        // no frame available - see how long it's been and complain (Debug) if over 100ms
        double detTime = m_timer.GetTotal();
        if ( detTime > 0.1)    // If capture/detection has taken more than 100 ms, something is probably wrong
        {
            DBGOUT("WARN - Face detect latency: %5.1f ms\n", detTime * 1000.0f);
        }
    }
    
    /* Blend most recent data into the smoothed data.  Do it regardless of whether we
     * got a new snapshot of data this time around, so the smoothed data continuously 
     * approaches the most recent snapshot, even as we're waiting for a new snapshot. */

    // Weight based on elapsed time - the longer since we've merged, the more weight
    // to apply to the snapshot data.  Constrain time to 0..1, then derive the weight
    // skewed towards 1 with sqrt(), i.e.
    //      for  30ms, time = .03, weight = sqrt(.03) = .17  -> 17% snap + 83% smoothed
    //      for 300ms, time = .30, weight = sqrt(.30) = .55  -> 55% snap + 45% smoothed
    float weight = (float) sqrt(MAX(0, MIN(elapsedTime, 1.0f)));

    // Some cameras may adjust frame size dynamically - get dimensions every time
    m_SmoothData.imageHeight = m_SnapData.imageHeight;
    m_SmoothData.imageWidth = m_SnapData.imageWidth;
    
    // Do weighted blend of latest face data into smoothed data
    WMERGE(m_SmoothData.faceX, m_SnapData.faceX, weight);
    WMERGE(m_SmoothData.faceY, m_SnapData.faceY, weight);

    // Zoom data is *very* noisy, so reduce weight to force a slow accomodation 
    // TODO: This is still very noisy.  Try a time-weighted history of samples & median-filter them.
    WMERGE(m_SmoothData.zoom, m_SnapData.zoom, (0.2f *weight));

    // Return the smoothed data
    detData = m_SmoothData;   
    
    return faceDetect;
}

//-----------------------------------------------------------------------------
// Extract position data and inter-landmark spacing from new face/lm data
// Calulate face centerpoint and zoom level, update snapshot 
void FacePipeline::UpdateDetectionData (float elapsedTime)
{
    // Statics remember initial measurements of face & landmarks
    static float refFaceDiag = -1.0f;
    static float refTop     = -1.0f;
    static float refBottom  = -1.0f;
    static float refLeft    = -1.0f;
    static float refRight   = -1.0f;
    static float refDiag1   = -1.0f;
    static float refDiag2   = -1.0f;

    // current data
    float faceDim = 0.0f;
    float lmDim = 0.0f;
    int   lmCount = 0;

    // Get camera image dimensions
    pxcU32 width, height;
    QueryImageSize(PXCImage::IMAGE_TYPE_COLOR, width, height); 
    m_SnapData.imageWidth = width;
    m_SnapData.imageHeight = height;

    // Face position / size
    if (fdValid)
    {
        DBGOUT("Face detected, x = %4d, y = %4d, w = %4d, h = %4d, confidence = %4d\n",
               m_FaceData.rectangle.x, m_FaceData.rectangle.y, 
               m_FaceData.rectangle.w, m_FaceData.rectangle.h, m_FaceData.confidence );

        // Set face position in center of face rectangle
        m_SnapData.faceX = m_FaceData.rectangle.x + (0.5f * m_FaceData.rectangle.w);
        m_SnapData.faceY = m_FaceData.rectangle.y + (0.5f * m_FaceData.rectangle.h);
        
        // find diagonal len of face box
        faceDim = sqrt((float) (m_FaceData.rectangle.w * m_FaceData.rectangle.w + 
                                 m_FaceData.rectangle.h * m_FaceData.rectangle.h) );
        
        // If first time through, remember the initial measurement
        if (refFaceDiag < 0.0f) refFaceDiag = faceDim;

        // Record the ratio of current to initial 
        faceDim /= refFaceDiag; 
    }

    // Landmarks postion 
    // Possibly overkill, but taking 6 measurements *should* help smooth jitter

    // Top = outer eye to outer eye
    if (ldValid[0] && ldValid[2])
    {
        float dx = m_LandmarkData[0].position.x - m_LandmarkData[2].position.x;
        float dy = m_LandmarkData[0].position.y - m_LandmarkData[2].position.y;
        float len = (dx*dx + dy*dy);    // lenght^2

        if (refTop < 0.0f) refTop = len;    // Remember 1st measurement

        lmDim += (len / refTop);            // Accumulate ratio of current len vs 1st len
        lmCount++;
    }

    // Bottom = L mouth to R mouth
    if (ldValid[4] && ldValid[5])
    {
        float dx = m_LandmarkData[4].position.x - m_LandmarkData[5].position.x;
        float dy = m_LandmarkData[4].position.y - m_LandmarkData[5].position.y;
        float len = (dx*dx + dy*dy);    // lenght^2

        if (refBottom < 0.0f) refBottom = len;    // Remember 1st

        lmDim += (len / refBottom);               // Accum ratio
        lmCount++;
    }

    // Right = R eye to R mouth
    if (ldValid[2] && ldValid[5])
    {
        float dx = m_LandmarkData[2].position.x - m_LandmarkData[5].position.x;
        float dy = m_LandmarkData[2].position.y - m_LandmarkData[5].position.y;
        float len = (dx*dx + dy*dy);    // lenght^2

        if (refRight < 0.0f) refRight = len;    // Remember 1st

        lmDim += (len / refRight);               // Accum ratio
        lmCount++;
    }

    // Left = L eye to L mouth
    if (ldValid[0] && ldValid[4])
    {
        float dx = m_LandmarkData[0].position.x - m_LandmarkData[4].position.x;
        float dy = m_LandmarkData[0].position.y - m_LandmarkData[4].position.y;
        float len = (dx*dx + dy*dy);    // lenght^2

        if (refLeft < 0.0f) refLeft = len;    // Remember 1st

        lmDim += (len / refLeft);               // Accum ratio
        lmCount++;
    }

    // Diag1 = L eye to R mouth
    if (ldValid[0] && ldValid[5])
    {
        float dx = m_LandmarkData[0].position.x - m_LandmarkData[5].position.x;
        float dy = m_LandmarkData[0].position.y - m_LandmarkData[5].position.y;
        float len = (dx*dx + dy*dy);    // lenght^2

        if (refDiag1 < 0.0f) refDiag1 = len;    // Remember 1st

        lmDim += (len / refDiag1);               // Accum ratio
        lmCount++;
    }

    // Diag2 = R eye to L mouth
    if (ldValid[2] && ldValid[4])
    {
        float dx = m_LandmarkData[2].position.x - m_LandmarkData[4].position.x;
        float dy = m_LandmarkData[2].position.y - m_LandmarkData[4].position.y;
        float len = (dx*dx + dy*dy);    // lenght^2

        if (refDiag2 < 0.0f) refDiag2 = len;    // Remember 1st

        lmDim += (len / refDiag2);               // Accum ratio
        lmCount++;
    }

    // TODO: see if a median filter here improves the jitter
    if (lmCount > 0)
    {
        lmDim /= lmCount;       // Get mean of all lm measurements (squared)
        lmDim = sqrt(lmDim);    // Sqrt to get RMS of measurements
    }

    // Determine a zoom level
    // If we have only face or only lm data - that's the zoom level
    // If we have both, weight the lm data (70%) more than the face data (30%)
    if ((faceDim > 0.0f) && (lmDim > 0.0f))
        m_SnapData.zoom = (0.3f * faceDim + 0.7f * lmDim);
    else if (faceDim > 0.0f)
        m_SnapData.zoom = faceDim;
    else if (lmDim > 0.0f)
        m_SnapData.zoom = lmDim;
    else
        m_SnapData.zoom = 1.0f;
}
