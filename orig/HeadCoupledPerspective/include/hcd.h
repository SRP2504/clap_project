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

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <d3dx9math.h>
#include <xnamath.h>    // for XMFLOAT
#include <stdio.h>
#include "CPUT_DX11.h"
#include <D3D11.h>      // for D3D11_BUFFER_DESC
#include "CPUTSprite.h"
#include "hcdUtil.h"
#include "detect.h"
#include "headCoupledCamera.h"

enum { MAX_CAMERAS = 16 };

// define some controls
const CPUTControlID ID_MAIN_PANEL = 10;
const CPUTControlID ID_FULLSCREEN_BUTTON = 100;
const CPUTControlID ID_SCENE_DROPDOWN = 101;
const CPUTControlID ID_ESTIMATE_DEPTH_CHECKBOX = 102;
const CPUTControlID ID_INPUT_CAMERA_CHECKBOX = 103;
const CPUTControlID ID_INPUT_ENABLE_CHECKBOX = 104;
const CPUTControlID ID_INPUT_GAIN_XY_SLIDER = 110;
const CPUTControlID ID_TEXTLINES  = 1000;
const CPUTControlID ID_IGNORE_CONTROL_ID = -1;

const UINT NUM_SCENES = 2;

// Scene - abstract, sample contains one or more
//-----------------------------------------------------------------------------
class HCDScene
{
public:
    
    HCDScene() : isLoaded(false), 
                 allocatedCam(false),
                 m_pSceneMainCamera(NULL), 
                 m_pSceneAssetSet(NULL) {};

    virtual ~HCDScene() { SAFE_RELEASE( m_pSceneAssetSet ); };

    virtual void Initialize() = 0;              // pure
    virtual void Update(double elapsed) = 0;    // pure
    virtual void Render( CPUTRenderParametersDX &drawParams ) = 0;  // pure
    virtual bool SetCameraParams(CPUTCamera *pCam) = 0;             // pure

    virtual CPUTCamera* GetCamera() { return m_pSceneMainCamera; };

protected:

    CPUTAssetSet            *m_pSceneAssetSet;
    CPUTCamera              *m_pSceneMainCamera;
    
    bool        isLoaded;
    bool        allocatedCam;
};


// DirectX 11 Sample
//-----------------------------------------------------------------------------
class HCDSample : public CPUT_DX11
{
public:
    HCDSample();
    virtual ~HCDSample();

    // Event handling
    virtual CPUTEventHandledCode HandleKeyboardEvent(CPUTKey key);
    virtual CPUTEventHandledCode HandleMouseEvent(int x, int y, int wheel, CPUTMouseState state);
    virtual void                 HandleCallbackEvent( CPUTEventID Event, CPUTControlID ControlID, CPUTControl* pControl );

    // 'callback' handlers for rendering events.  Derived from CPUT_DX11
    virtual void Create();
    virtual void Render(double elapsed);
    virtual void Update(double elapsed);
    virtual void ResizeWindow(UINT width, UINT height);
    virtual void Shutdown();

private:

    bool SetCPUTDefaults();
    void SetCameraParams();

    // new model set concept
    HCDScene                *m_pScenes[NUM_SCENES];
    UINT                    m_nActiveSceneNumber;
    HCDCameraController     *m_pCameraController;
    float                   m_fElapsedTime;

    CPUTText                *m_pFPSCounter;

public:

    FacePipeline            m_Detector;
};

