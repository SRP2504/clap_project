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
#pragma once

#include "CPUT_DX11.h"
#include "CPUTCamera.h"
#include "CPUTFrustum.h"
#include "hcdUtil.h"
#include "CPUTEventHandler.h"

// Head-coupled camera controller - copied (not derived) from CPUT FPS camera

// Head-controlled camera
//-----------------------------------------------------------------------------
class HCDCameraController : public CPUTCameraController
{
public:
    HCDCameraController() : mXScale(1), mYScale(1), mZScale(1),
                            mCameraXfov(1), mLocked(false) {};
    ~HCDCameraController() { SAFE_RELEASE(mpCamera); };    // Prevent multiple releases

    void Update( float deltaSeconds=0.0f);
    CPUTEventHandledCode HandleKeyboardEvent(CPUTKey key);
    CPUTEventHandledCode HandleMouseEvent(int x, int y, int wheel, CPUTMouseState state);

    void SetHCDCamera(CPUTRenderNode *pCamera); // Wraps SetCamera()
    void SetOffsets(DetectionData &dData, float viewDist, bool estZoom);

    void GetScales(float &x, float &y, float &z);
    void SetScales(float x, float y, float z);

    void SetPhysicalFOV(float fovDegrees);
    void LockPosition(bool lock);

    CPUTCamera *GetOffsetCamera() { return &mOffsetCam; };

private:
    CPUTCamera  mOffsetCam;
    float       mXScale, mYScale, mZScale;
    float       mCameraXfov; // Radians, 57 deg = ~1 radian
    bool        mLocked;     // no WASD movement 
};
