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
#include "hcd.h"
#include "CPUTRenderTarget.h"
#include "ballRoomScene.h"
#include "pipeRoomScene.h"

// Globals
float   g_viewDistance = 1.0f;  // distance to viewer, measured in (screen_width) units
bool    g_EnableDepthFromCamera = false;
bool    g_EnableInputCamera = false;
bool    g_ShowInputCamera = false;
bool    g_FullScreenMode = false;

bool    g_SensorOK = true;     // To allow the sample to run, even if no camera sensor detected

CPUTCheckbox* g_pEstimateDepthCheckbox = NULL;
CPUTCheckbox* g_pShowInputCheckbox = NULL;
CPUTCheckbox* g_pEnableCameraCheckbox = NULL;

float   g_XYScale = 1.0f;
float   g_ZScale = 1.0f;

// Diagnostic text
CPUTText *diagfps = NULL;

// Application entry point.  Execution begins here.
//-----------------------------------------------------------------------------
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

#if defined(DEBUG) | defined(_DEBUG) | defined (_DBGOUT_ON_RELEASE)

    // tell VS to report leaks at any exit of the program (including new)
    #ifndef DBG_NEW
        #define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
        #define new DBG_NEW
    #endif
    _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );


    // Open a console for debug text
    AllocConsole();
    HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
    int hCrt = _open_osfhandle((long) handle_out, _O_TEXT);
    FILE* hf_out = _fdopen(hCrt, "w");
    setvbuf(hf_out, NULL, _IONBF, 1);
    *stdout = *hf_out;

    DBGOUT(" Opening Debug console...\n\n");

#endif

    CPUTResult result=CPUT_SUCCESS;
    int returnCode = 0;

    CPUTOSServices *pServices = CPUTOSServices::GetOSServices();

    // create an instance of my sample
    HCDSample* pSample = new HCDSample();

    // Initialize the system and give it the base CPUT resource directory (location of GUI images/etc)
        // Initialize CPUT and give it the base CPUT resource directory (location of GUI images/etc)
    if (CPUTFAILED( pSample->CPUTInitialize() ))
    {
        pServices->OpenMessageBox(_L("Error"), _L("Failed to initialize CPUT framework, exiting"));
        SAFE_DELETE(pSample);
        exit(-1);
    }

    // window parameters
    CPUTWindowCreationParams params;
    params.windowWidth      = 1280; 
    params.windowHeight     = 1024;
    params.startFullscreen  = false;

    // device parameters
    params.deviceParams.refreshRate         = 60;
    params.deviceParams.swapChainBufferCount= 1;
    params.deviceParams.swapChainFormat     = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    params.deviceParams.swapChainUsage      = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    // Parse the command line, override defaults above if specified
    cString AssetFilename;
    cString CommandLine(lpCmdLine);
    pSample->CPUTParseCommandLine(CommandLine, &params, &AssetFilename);       

    // create the window and device context
    result = pSample->CPUTCreateWindowAndContext(_L("Head Coupled Display"), params);
    ASSERT( CPUTSUCCESS(result), _L("CPUT Error creating window and context.") );

    // Initialize the face detector
    bool ok = pSample->m_Detector.Init();
    
    if (!ok)
    {
        MessageBox(0, _L("Error initializing camera sensor - face tracking disabled."), 0, MB_OK);
        g_SensorOK = false;
        if (g_pEnableCameraCheckbox) g_pEnableCameraCheckbox->SetEnable(false);
    }

    // start the main message loop
    returnCode = pSample->CPUTMessageLoop();

    // Exit main loop - clean up
    if (g_SensorOK) pSample->m_Detector.Close();

    pSample->DeviceShutdown();
    delete pSample;

    return returnCode;
}

// Handle OnCreation events
//-----------------------------------------------------------------------------
void HCDSample::Create()
{
    // Load the scene data
    m_pScenes[0] = new PipeRoomScene;
    m_pScenes[1] = new BallRoomScene;    

    if (!SetCPUTDefaults())
    {
        CPUT_DX11::Shutdown();  // Flag unsuccessful Create();      //TODO: this doesn't exit cleanly - figure out why
        return;
    }

    // Create GUI controls
    CPUTGuiControllerDX11*  pGUI = CPUTGetGuiController();
    CPUTButton* pButton = NULL;
    pGUI->CreateButton(_L("Fullscreen"), ID_FULLSCREEN_BUTTON, ID_MAIN_PANEL, &pButton);

    // NOTE - be sure to update NUM_SCENES constant when adding scenes
    CPUTDropdown* pDropdown = NULL;
    pGUI->CreateDropdown(_L("Pipe Room"), ID_SCENE_DROPDOWN, ID_MAIN_PANEL, &pDropdown);
    pDropdown->AddSelectionItem(_L("Environment Window"));
    m_nActiveSceneNumber = 0;   // Pipe Room
    pDropdown->SetSelectedItem(m_nActiveSceneNumber+1);  // (dropdown is 1-based)
    
    CPUTCheckbox* pCheckbox = NULL;
    pGUI->CreateCheckbox(_L("Enable Camera Control"), ID_INPUT_ENABLE_CHECKBOX, ID_MAIN_PANEL, &pCheckbox);
    pCheckbox->SetCheckboxState(CPUT_CHECKBOX_UNCHECKED);
    g_pEnableCameraCheckbox = pCheckbox;
    pGUI->CreateCheckbox(_L("Show Input Camera"), ID_INPUT_CAMERA_CHECKBOX, ID_MAIN_PANEL, &pCheckbox);
    pCheckbox->SetCheckboxState(CPUT_CHECKBOX_UNCHECKED);
    pCheckbox->SetEnable(false);
    g_pShowInputCheckbox = pCheckbox;   // Keep pointer to control, to enable/disable
    pGUI->CreateCheckbox(_L("Estimate Depth From Landmarks"), ID_ESTIMATE_DEPTH_CHECKBOX, ID_MAIN_PANEL, &pCheckbox);
    pCheckbox->SetCheckboxState(CPUT_CHECKBOX_UNCHECKED);
    pCheckbox->SetEnable(false);
    g_pEstimateDepthCheckbox = pCheckbox;   // Keep pointer to control, to enable/disable

    CPUTSlider* pSlider = NULL;
    pGUI->CreateSlider(_L("XY Gain = 1.0"), ID_INPUT_GAIN_XY_SLIDER, ID_MAIN_PANEL, &pSlider);
    pSlider->SetScale(0.0f, 5.0f, 100);
    pSlider->SetValue(g_XYScale);
    pSlider->SetTickDrawing(false);

    pGUI->SetActivePanel(ID_MAIN_PANEL);
    pGUI->DrawFPS(false);

    // Init the selected scene 
    m_pScenes[m_nActiveSceneNumber]->Initialize();
    SetCameraParams();

}

//-----------------------------------------------------------------------------
void HCDSample::SetCameraParams()
{
    int width, height;
    
    ASSERT(m_nActiveSceneNumber < NUM_SCENES, L"Attempt to set camera params with no active scene.")

    mpCamera = mpShadowCamera = NULL;
    mpCamera = m_pScenes[m_nActiveSceneNumber]->GetCamera(); 

    // Set camera projection
    CPUTOSServices::GetOSServices()->GetClientDimensions(&width, &height);
    mpCamera->SetAspectRatio(((float)width)/((float)height));
    mpCamera->SetFov(2 * atan(0.5f / g_viewDistance));
    mpCamera->SetFarPlaneDistance(10000.0f);
    mpCamera->SetNearPlaneDistance(1.0f);
    mpCamera->Update();

    // Assign a camera controller
    if (!m_pCameraController)
    {
        m_pCameraController = new HCDCameraController();    
    }

    m_pCameraController->SetHCDCamera(mpCamera);    // SetCamera isn't virtual, so created an HCD-specific setup that 
    m_pCameraController->SetLookSpeed(0.004f);      // Mouse pointing sensitivity
    m_pCameraController->SetMoveSpeed(20.0f);       // Keyboard translate sensitivity
}

//-----------------------------------------------------------------------------
void HCDSample::Update(double elapsed)
{
    if (g_EnableInputCamera)
    {
        DetectionData detData;
        bool ok = m_Detector.GetFaceData(detData, elapsed);
        if (ok)
        {
            m_pCameraController->SetOffsets(detData, g_viewDistance, g_EnableDepthFromCamera);
        }
    }
    m_pCameraController->Update((float) elapsed);
}

// Render callback (DX-specific)
//-----------------------------------------------------------------------------
void HCDSample::Render(double elapsed)
{
    // Clear back buffer
    const float srgbClearColor[] = { 0.02f, 0.02f, 0.1f, 1.0f };
    mpContext->ClearRenderTargetView( mpRenderTargetView,  srgbClearColor );
    mpContext->ClearDepthStencilView( mpDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0.0f, 0);

    CPUTRenderParametersDX drawParams(mpContext);

    // Draw scene
    if (g_EnableInputCamera)
    {
        CPUTCamera *pLastCamera = mpCamera;
        mpCamera = m_pCameraController->GetOffsetCamera();
        m_pScenes[m_nActiveSceneNumber]->Render(drawParams);
        mpCamera = pLastCamera;
    }
    else
    {
        m_pScenes[m_nActiveSceneNumber]->Render(drawParams);
    }

    CPUTDrawGUI();
}

// Handle OnShutdown events
//-----------------------------------------------------------------------------
void HCDSample::Shutdown()
{
    // Workaround - CPUT doesn't clean up correctly if we exit from full-screen
    //if (g_FullScreenMode) CPUTToggleFullScreenMode();

    CPUTAssetLibraryDX11* pLib = (CPUTAssetLibraryDX11 *)CPUTAssetLibraryDX11::GetAssetLibrary();
    if (pLib)
    {
        pLib->ReleaseAllLibraryLists();
        pLib->DeleteAssetLibrary();
    }
    CPUT_DX11::Shutdown();
}

// ctor
//-----------------------------------------------------------------------------
HCDSample::HCDSample() : 
    m_pCameraController(NULL),
    m_nActiveSceneNumber(0xFF)
{ 
    mpCamera = NULL;
    for (UINT i = 0; i < NUM_SCENES; i++)
    {
        m_pScenes[i] = NULL;
    }
}

// dtor
//-----------------------------------------------------------------------------
HCDSample::~HCDSample()
{
    for (UINT i = 0; i < NUM_SCENES; i++)
    {
        SAFE_DELETE( m_pScenes[i] );
    }
    SAFE_DELETE( m_pCameraController );
}


// Handle keyboard events
//-----------------------------------------------------------------------------
CPUTEventHandledCode HCDSample::HandleKeyboardEvent(CPUTKey key)
{
    CPUTEventHandledCode    handled = CPUT_EVENT_UNHANDLED;

    switch(key)
    {
    case KEY_F1:    // no-op
        break;

    case KEY_ESCAPE:
        handled = CPUT_EVENT_HANDLED;
        Shutdown();
        break;

    // EMEA test lab keyboard shortcuts
    // ================================
    case KEY_1:
        // Toggle 1st checkbox
        if (g_pEnableCameraCheckbox->IsEnabled())
        {
            CPUTCheckboxState s = g_pEnableCameraCheckbox->GetCheckboxState();
            s = (CPUT_CHECKBOX_CHECKED == s ? CPUT_CHECKBOX_UNCHECKED : CPUT_CHECKBOX_CHECKED);
            g_pEnableCameraCheckbox->SetCheckboxState(s);
            // And call handler
            HandleCallbackEvent(0, ID_INPUT_ENABLE_CHECKBOX, g_pEnableCameraCheckbox);
        }
        break;

    case KEY_2:
        // Toggle 2nd checkbox
        if (g_pShowInputCheckbox->IsEnabled())
        {
            CPUTCheckboxState s = g_pShowInputCheckbox->GetCheckboxState();
            s = (CPUT_CHECKBOX_CHECKED == s ? CPUT_CHECKBOX_UNCHECKED : CPUT_CHECKBOX_CHECKED);
            g_pShowInputCheckbox->SetCheckboxState(s);
            // And call handler
            HandleCallbackEvent(0, ID_INPUT_CAMERA_CHECKBOX, g_pShowInputCheckbox);
        }
        break;

    case KEY_3:
        // Toggle 3rd checkbox
        if (g_pEstimateDepthCheckbox->IsEnabled())
        {
            CPUTCheckboxState s = g_pEstimateDepthCheckbox->GetCheckboxState();
            s = (CPUT_CHECKBOX_CHECKED == s ? CPUT_CHECKBOX_UNCHECKED : CPUT_CHECKBOX_CHECKED);
            g_pEstimateDepthCheckbox->SetCheckboxState(s);
            // And call handler
            HandleCallbackEvent(0, ID_ESTIMATE_DEPTH_CHECKBOX, g_pEstimateDepthCheckbox);
        }
        break;

    }

    if(handled == CPUT_EVENT_UNHANDLED)
    {
        m_pCameraController->HandleKeyboardEvent(key);
    }

    return handled;
}

// Handle mouse wheel zoom events
//-----------------------------------------------------------------------------
CPUTEventHandledCode HCDSample::HandleMouseEvent(int x, int y, int wheel, CPUTMouseState state)
{
    // Discard mouse events if camera isn't yet set up, or getting depth from camera
    if (!mpCamera || !m_pCameraController ) return CPUT_EVENT_HANDLED;

    if (!g_EnableDepthFromCamera)   // Depth from mouse wheel
    {
        if (wheel < 0)
            g_viewDistance *= 0.95f; // Move in

        if (wheel > 0)
            g_viewDistance *= 1.05f; // Move out

        // clamp view range extents
        g_viewDistance = max(0.5f, g_viewDistance);
        g_viewDistance = min(3.0f, g_viewDistance);
        float viewAngle = 2 * atan(0.5f / g_viewDistance);

        // Might be using normal or offset camera, so update both
        mpCamera->SetFov(viewAngle);
        CPUTCamera *pCam = m_pCameraController->GetOffsetCamera();
        pCam->SetFov(viewAngle);
    }

    // Let controller handle any other mouse events
    return m_pCameraController->HandleMouseEvent(x, y, wheel, state);
}

// Handle any control callback events
//-----------------------------------------------------------------------------
void HCDSample::HandleCallbackEvent( CPUTEventID Event, CPUTControlID ControlID, CPUTControl* pControl )
{
    UNREFERENCED_PARAMETER(Event);

    switch(ControlID)
    {
    case ID_FULLSCREEN_BUTTON:        
        {
            CPUTToggleFullScreenMode();
            g_FullScreenMode = !g_FullScreenMode;
        }
        break;

    case ID_SCENE_DROPDOWN:
        {
            UINT sceneID;
            static_cast<CPUTDropdown*>(pControl)->GetSelectedItem(sceneID);
            sceneID--;  // dropdown units are 1-based, adjust to 0-base

            // Switch scenes if ID is valid and has changed
            if ((sceneID < NUM_SCENES) && (sceneID != m_nActiveSceneNumber))
            {
                m_nActiveSceneNumber = sceneID;
                m_pScenes[m_nActiveSceneNumber]->Initialize();
                SetCameraParams();

                // Ok, this is a bit of a HACK
                // Only have 2 scenes, scene 0 camera can move, scene 1 camera should be fixed
                // QED:
                m_pCameraController->LockPosition(1 == sceneID);
            }
        }
        break;

    case ID_INPUT_ENABLE_CHECKBOX:
        {
            CPUTCheckboxState s = static_cast<CPUTCheckbox*>(pControl)->GetCheckboxState();
            g_EnableInputCamera = (s == CPUT_CHECKBOX_CHECKED);

            // Uncheck and set enable/disable on dependent checkboxes
            g_pShowInputCheckbox->SetCheckboxState(CPUT_CHECKBOX_UNCHECKED);
            g_pShowInputCheckbox->SetEnable(g_EnableInputCamera);
            g_pEstimateDepthCheckbox->SetCheckboxState(CPUT_CHECKBOX_UNCHECKED);
            g_pEstimateDepthCheckbox->SetEnable(g_EnableInputCamera);
            g_viewDistance = 1.0f;

            if (g_ShowInputCamera) // if window currently enabled
            {
                m_Detector.EnableCameraWindow(false);
                g_ShowInputCamera = false;
                g_EnableDepthFromCamera = false;
            }
        }
        break;

    case ID_INPUT_CAMERA_CHECKBOX:
        {
            CPUTCheckboxState s = static_cast<CPUTCheckbox*>(pControl)->GetCheckboxState();
            g_ShowInputCamera = (s == CPUT_CHECKBOX_CHECKED);
            m_Detector.EnableCameraWindow(g_ShowInputCamera);
        }
        break;

    case ID_ESTIMATE_DEPTH_CHECKBOX:
        {
            CPUTCheckboxState s = static_cast<CPUTCheckbox*>(pControl)->GetCheckboxState();
            g_EnableDepthFromCamera = (s == CPUT_CHECKBOX_CHECKED);
            g_viewDistance = 1.0f;
        }
        break;

    case ID_INPUT_GAIN_XY_SLIDER:
        {
            float x, y, z, val;
            wchar_t  lbl[32];

            CPUTSlider *pSlider = static_cast<CPUTSlider*>(pControl);
            pSlider->GetValue(val);
            swprintf(lbl, 32, _L("XY Scale = %3.1f"), val);  // New label
            std::wstring newLabel(lbl);                 // as a wstring
            pSlider->SetText(newLabel);

            m_pCameraController->GetScales(x, y, z);
            m_pCameraController->SetScales(val, val, z);
        }
        break;

    default:
        { }
        break;
    }
}

// Handle resize events
//-----------------------------------------------------------------------------
void HCDSample::ResizeWindow(UINT width, UINT height)
{
    CPUT_DX11::ResizeWindow( width, height );

    if (mpCamera)
    {
        mpCamera->SetAspectRatio(((float)width)/((float)height));
        mpCamera->Update();
    }
}

// Set up some default items for CPUT
//-----------------------------------------------------------------------------

bool HCDSample::SetCPUTDefaults()
{
    // Add our programatic (and global) material parameters
    CPUTMaterial::mGlobalProperties.AddValue( _L("cbPerFrameValues"), _L("$cbPerFrameValues") );
    CPUTMaterial::mGlobalProperties.AddValue( _L("cbPerModelValues"), _L("#cbPerModelValues") );
    CPUTMaterial::mGlobalProperties.AddValue( _L("_Shadow"), _L("$shadow_depth") );

    // Load assets
    CPUTAssetLibraryDX11 *pLib = (CPUTAssetLibraryDX11 *)CPUTAssetLibrary::GetAssetLibrary();
    CPUTOSServices *pServices = CPUTOSServices::GetOSServices();

    // Search a up the dir tree for the media dir, fail if we can't find it
    cString exeDir, mediaDir, upDir;
    mediaDir.clear();
    upDir.clear();
    pServices->GetExecutableDirectory(&exeDir);

    bool mediaDirSet = false;
    for (int upLevel = 0; upLevel < 4; upLevel++)   // try looking up to 3 levels above current working dir
    {
        mediaDir = exeDir + upDir + _L("Media\\");
        if (CPUTSUCCESS(pServices->DoesDirectoryExist(mediaDir)))
        {
            cString defxFilename = mediaDir + _L("Shader\\default.fx"); // Make sure we're found it by looking for default shader file
            if (CPUTSUCCESS(pServices->DoesFileExist(defxFilename)))
            {
                pLib->SetMediaDirectoryName(mediaDir);
                mediaDirSet = true;
                break;
            }
        }
        upDir.append(_L("..\\"));
    }

    if (!mediaDirSet) return false;

    CPUTPixelShaderDX11  *pPS, *pPSNoTex ;
    CPUTVertexShaderDX11 *pVS, *pVSNoTex ;

    pLib->GetPixelShader(  L"default.fx", CPUT_DX11::mpD3dDevice, L"PSMain",          L"ps_4_0", &pPS );
    pLib->GetPixelShader(  L"default.fx", CPUT_DX11::mpD3dDevice, L"PSMainNoTexture", L"ps_4_0", &pPSNoTex );
    pLib->GetVertexShader( L"default.fx", CPUT_DX11::mpD3dDevice, L"VSMain",          L"vs_4_0", &pVS );
    pLib->GetVertexShader( L"default.fx", CPUT_DX11::mpD3dDevice, L"VSMainNoTexture", L"vs_4_0", &pVSNoTex );

    // We just want to create them, which adds them to the library.  We don't need them any more so release them, leaving refCount at 1 (only library owns a ref)
    SAFE_RELEASE(pPS);
    SAFE_RELEASE(pPSNoTex);
    SAFE_RELEASE(pVS);
    SAFE_RELEASE(pVSNoTex);

    // Load shadow material (un-used, but some assets reference shadow_depth - easiest way avoid missing asset error)
    CPUTRenderTargetDepth *rtd = new CPUTRenderTargetDepth();
    rtd->CreateRenderTarget( cString(_L("$shadow_depth")), 16, 16, DXGI_FORMAT_D32_FLOAT );
    SAFE_DELETE(rtd);   // $shadow_depth resource created - no longer need render target

    CPUTRenderStateBlockDX11 *pBlock = new CPUTRenderStateBlockDX11();
    CPUTRenderStateDX11 *pStates = pBlock->GetState();

    // Override default sampler desc for our default shadowing sampler
    pStates->SamplerDesc[1].Filter         = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
    pStates->SamplerDesc[1].AddressU       = D3D11_TEXTURE_ADDRESS_BORDER;
    pStates->SamplerDesc[1].AddressV       = D3D11_TEXTURE_ADDRESS_BORDER;
    pStates->SamplerDesc[1].ComparisonFunc = D3D11_COMPARISON_GREATER;
    pBlock->CreateNativeResources();
    CPUTAssetLibrary::GetAssetLibrary()->AddRenderStateBlock( _L("$DefaultRenderStates"), pBlock );
    pBlock->Release(); // We're done with it.  The library owns it now.
	return true;
}

