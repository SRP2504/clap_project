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

#include "ballRoomScene.h"

//--------------------------------------------------------------------------------------
BallRoomScene::BallRoomScene()
{
};

//--------------------------------------------------------------------------------------
BallRoomScene::~BallRoomScene()
{
    if (allocatedCam)
    {
        SAFE_RELEASE(m_pSceneMainCamera);
    }
};

//--------------------------------------------------------------------------------------
void BallRoomScene::LoadAssets()
{
    if (isLoaded) return;   // insurance - should never happen

    CPUTAssetLibrary *pAssetLibrary   = CPUTAssetLibrary::GetAssetLibrary();
    CPUTOSServices *pServices = CPUTOSServices::GetOSServices();

    // Search a up the dir tree for the media dir, fail if we can't find it
    cString exeDir, mediaDir, upDir;
    mediaDir.clear();
    upDir.clear();
    pServices->GetExecutableDirectory(&exeDir);

    bool mediaDirSet = false;
    for (int upLevel = 0; upLevel < 4; upLevel++)   // try looking up to 3 levels above current working dir
    {
        mediaDir = exeDir + upDir + _L("Media\\envSphere\\");
        if (CPUTSUCCESS(pServices->DoesDirectoryExist(mediaDir)))
        {
            pAssetLibrary->SetMediaDirectoryName(mediaDir);
            mediaDirSet = true;
            break;
        }
        upDir.append(_L("..\\"));
    }

    ASSERT( mediaDirSet, _L("Failed locating environment sphere media"));

    m_pSceneAssetSet = pAssetLibrary->GetAssetSet(_L("envSphere_Camera"));
    ASSERT( m_pSceneAssetSet, _L("Failed loading sphere scene asset set"));
    m_pSceneMainCamera = m_pSceneAssetSet->GetFirstCamera();
    if (!m_pSceneMainCamera)
    {
        m_pSceneMainCamera = new CPUTCamera();  
        m_pSceneMainCamera->SetPosition( 0.0f, 0.0f, 5.0f );
        allocatedCam = true;
    }

    isLoaded = true;
};

//--------------------------------------------------------------------------------------
void BallRoomScene::Initialize()
{
    if (!isLoaded) LoadAssets();
};

//--------------------------------------------------------------------------------------
void BallRoomScene::Update(double elapsed)
{
};

//--------------------------------------------------------------------------------------
void BallRoomScene::Render(CPUTRenderParametersDX &drawParams)
{
    m_pSceneAssetSet->RenderRecursive(drawParams);
};
