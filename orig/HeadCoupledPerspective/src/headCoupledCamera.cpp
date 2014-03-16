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

//-----------------------------------------------------------------------------
// HDCCameraController is conceptually a standard CPUT FPS camera with a second
// camera positioned directly behind the first.  Mouse/keyboard drive the first
// camera at all times.  When head-coupled camera control is active, the second
// camera is offset from the first camera's axis proportionally to the detected
// head position within the sensor camera's frame.  The scene is rendered from
// the second camera's point of view.
//-----------------------------------------------------------------------------

#include "headCoupledCamera.h"

#define KEY_DOWN(vk) ((GetAsyncKeyState(vk) & 0x8000)?1:0)

//  Translate head motion into keyboard camera controls
//-----------------------------------------------------------------------------
CPUTEventHandledCode HCDCameraController::HandleKeyboardEvent(CPUTKey key)
{
    return CPUT_EVENT_HANDLED;
}

//-----------------------------------------------------------------------------
// Position update from keypress - same as CPUT's FPS controller, plus a position-lock
// Applied to the 'first' camera
void HCDCameraController::Update(float deltaSeconds)
{
    if (!mLocked)
    {
        float speed = mfMoveSpeed * deltaSeconds;
        speed *= KEY_DOWN( VK_LSHIFT ) ? 10.0f : KEY_DOWN( VK_LCONTROL) ? 0.1f : 1.0f;

        float4x4 *pParentMatrix = mpCamera->GetParentMatrix();

        float3 vRight(pParentMatrix->getXAxis());
        float3 vUp(pParentMatrix->getYAxis());
        float3 vLook(pParentMatrix->getZAxis());
        float3 vPositionDelta(0.0f);

        if(CPUTOSServices::GetOSServices()->DoesWindowHaveFocus())
        {
            if(KEY_DOWN('W')) { vPositionDelta +=  vLook *  speed;}
            if(KEY_DOWN('A')) { vPositionDelta += vRight * -speed;}
            if(KEY_DOWN('S')) { vPositionDelta +=  vLook * -speed;}
            if(KEY_DOWN('D')) { vPositionDelta += vRight *  speed;}
            if(KEY_DOWN('E')) { vPositionDelta +=    vUp *  speed;}
            if(KEY_DOWN('Q')) { vPositionDelta +=    vUp * -speed;}
        }

        float x,y,z;
        mpCamera->GetPosition( &x, &y, &z );
        mpCamera->SetPosition( x+vPositionDelta.x, y+vPositionDelta.y, z+vPositionDelta.z );
    }

    mpCamera->Update();
}

//-----------------------------------------------------------------------------
// Sibling of CPUT's FPS controller - same implementation of mouse handling
// Applied to the 'first' camera
CPUTEventHandledCode HCDCameraController::HandleMouseEvent ( int x, int y, int wheel, CPUTMouseState state )
{
    if(state & CPUT_MOUSE_LEFT_DOWN)
    {
        float3 position = mpCamera->GetPosition();

        if(!(mPrevFrameState & CPUT_MOUSE_LEFT_DOWN)) // Mouse was just clicked
        {
            mnPrevFrameX = x;
            mnPrevFrameY = y;
        }

        float nDeltaX = (float)(x-mnPrevFrameX);
        float nDeltaY = (float)(y-mnPrevFrameY);

        float4x4 rotationX = float4x4RotationX(nDeltaY*mfLookSpeed);
        float4x4 rotationY = float4x4RotationY(nDeltaX*mfLookSpeed);

        mpCamera->SetPosition(0.0f, 0.0f, 0.0f); // Rotate about camera center
        float4x4 parent      = *mpCamera->GetParentMatrix();
        float4x4 orientation = rotationX * parent * rotationY;
        orientation.orthonormalize();
        mpCamera->SetParentMatrix( orientation );
        mpCamera->SetPosition( position.x, position.y, position.z ); // Move back to original position
        mpCamera->Update();

        mnPrevFrameX = x;
        mnPrevFrameY = y;
        mPrevFrameState = state;
        return CPUT_EVENT_HANDLED;
    } 
    else
    {
        mPrevFrameState = state;
        return CPUT_EVENT_UNHANDLED;
    }
}

//-----------------------------------------------------------------------------
// CPUTCamera doesn't have an assignment overload - do an explicit copy
void CopyCamera(CPUTCamera *toCam, CPUTCamera *fromCam)
{
    toCam->SetAspectRatio(fromCam->GetAspectRatio());
    toCam->SetFov(fromCam->GetFov());
    toCam->SetNearPlaneDistance(fromCam->GetNearPlaneDistance());
    toCam->SetFarPlaneDistance(fromCam->GetFarPlaneDistance());
    toCam->Update();
}

//-----------------------------------------------------------------------------
void HCDCameraController::SetHCDCamera(CPUTRenderNode *pCamera)
{ 
    SetCamera(pCamera);   // CPUTCameraController::SetCamera(), just assigns pointer & ref-counts

    CopyCamera(&mOffsetCam, (CPUTCamera *) pCamera); 
}

//-----------------------------------------------------------------------------
// Calculate offset position and view parameters of 'second' camera, behind 'first'
void HCDCameraController::SetOffsets(DetectionData &dData, float viewDist, bool estZoom)
{
    // In poor sensing environment we can be here before a valid face detection
    // has occurred.  No harm done, just ignore until there's valid data
    if (0 == dData.imageWidth || 0 == dData.imageHeight)
    {
        _ASSERT(0);
        return;
    }

    // Face horizontal offset
    float midpoint = 0.5f * dData.imageWidth;   // x value at L/R midpoint of image
    float faceMidpoint = dData.faceX;           // x value at center of face
    float dx = (midpoint - faceMidpoint) / (float) dData.imageWidth; // result <-0.5 .. 0.5>

    // Face vertical offset
    midpoint = 0.5f * dData.imageHeight;        // y value at T/B midpoint of image
    faceMidpoint = dData.faceY;                 // y value of face
    float dy = (midpoint - faceMidpoint) / (float) dData.imageHeight;   // <-0.5 .. 0.5>

    float camAspectRatio = (float) dData.imageHeight / (float) dData.imageWidth;

    // Get first camera position data
    float3 camPos = mpCamera->GetPosition();
    float3 camLook = mpCamera->GetLook();
    float3 camUp = mpCamera->GetUp();
    float3 camRight = mpCamera->GetParentMatrix()->getXAxis();  // No GetRight?
    camLook.normalize();
    camUp.normalize();
    camRight.normalize();

    // Position the offset camera behind the first, derive x,y offsets
    float3 offSetPos = camPos-camLook;
    offSetPos += camRight * asin(mCameraXfov * dx * mXScale);
    offSetPos += camUp * asin(mCameraXfov * camAspectRatio * dy * mYScale); 

    // Place the second camera at the offset position, looking at first camera
    mOffsetCam.SetPosition(offSetPos);
    mOffsetCam.LookAt(camPos.x, camPos.y, camPos.z);

    // Zoom level - attempts to preserve solid angle of features as viewer gets
    // closer/further from the display.  i.e. as viewer gets closer, the apparent
    // size of the objects remains the same, while the field of view expands
    if (estZoom)    // Estimate zoom level from scale of face in camera
    {
        float zoomViewDist = viewDist;
        if (dData.zoom > 0.0f) zoomViewDist /= (dData.zoom);
        mOffsetCam.SetFov(2 * atan(0.5f / zoomViewDist));
    }

    mOffsetCam.Update();
};

//-----------------------------------------------------------------------------
// 
void HCDCameraController::GetScales(float &x, float &y, float &z)
{
    x = mXScale;
    y = mYScale;
    z = mZScale;
};

//-----------------------------------------------------------------------------
// 
void HCDCameraController::SetScales(float x, float y, float z)
{
    if (x > 0.0f && x < 10.0f)  // What's a reasonable max?  
        mXScale = x;

    if (y > 0.0f && y < 10.0f)  // What's a reasonable max?  
        mYScale = y;

    if (z > 0.0f && z < 10.0f)  // What's a reasonable max?  
        mZScale = z;
};

//-----------------------------------------------------------------------------
// If actual horizontal field of view of sensor camera is known, set it here
// Not (currently) available from PC-SDK, default setting is 1 Radian (~57 deg).
void HCDCameraController::SetPhysicalFOV(float fovDegrees)
{
    if ((fovDegrees > 0.0f) && (fovDegrees <= 180.0f))
        mCameraXfov = fovDegrees * CONST_PI / 180.0f; // Specified in degrees, stored as radians
};

//-----------------------------------------------------------------------------
// Lock position (not orientation) of first camera (i.e. disable WASD movement)
void HCDCameraController::LockPosition(bool lock)
{
    mLocked = lock;
};
