/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/

#include "util_pipeline.h"
#include "pxcvoice.h"
#include "util_capture.h"
#include "..\..\AUXlib\AUXAudioRender.h"
#include "..\..\src\dispatcher.h"
#include "..\..\src\invoker.h"
#include "sensor_voice.hpp"

extern CInvoker g_Invoker;
extern CDispatcher g_Dispatcher;

#define MY_VOICE_ANY_COMMAND 1 
#define MY_SENSOR_ID CSensor::Sensor_Voice_recognition

class CSensorVoice;

// Callback for recognized command
class VoiceAction : public PXCBaseImpl<PXCVoiceRecognition::Recognition::Handler> {
public:

    VoiceAction(CSensorVoice* sensor) 
    {
        this->sensor=sensor;
    }
    virtual void PXCAPI OnRecognized(PXCVoiceRecognition::Recognition *cmd) 
    {
        if (sensor->m_dictationMode) {
            for (int i=0;i<(int)sensor->m_eventList.size();i++) {
                if (sensor->m_eventList[i].id == MY_VOICE_ANY_COMMAND) {
                    g_Invoker.ExecuteEvent(MY_SENSOR_ID, sensor->m_eventList[i].id, cmd->dictation);
                    continue;
                }
                if (_wcsicmp(cmd->dictation, sensor->m_eventList[i].name)==0) {
                    g_Invoker.ExecuteEvent(MY_SENSOR_ID, sensor->m_eventList[i].id); 
                    return;
                }
            }
        } else {
            g_Invoker.ExecuteEvent(MY_SENSOR_ID, cmd->label); 
        }
    }
    CSensorVoice* sensor;
};

// fix problem with the voice sensor restart
//#define MY_FIX
#ifdef MY_FIX
static PXCSession* session = NULL;
static PXCVoiceRecognition* vc = NULL;
#endif
void StopAudio()
{
#ifdef MY_FIX
    if (vc) {
        vc->Release();
        vc = NULL;
    }
    if (session) {
        session->Release();
        session = NULL;
    }
#endif
}
// temporary fix problem with the voice sensor restart

class VoicePipeline
{
public:
    VoicePipeline(bool videoControl, CSensorVoice* sensor) 
    {
        m_id = MY_SENSOR_ID;
        m_sensor = sensor;
        m_render = NULL;
        if (videoControl) m_render = new CAUXAudioRender(1, 2, L"Voice Viewer");
    }
    ~VoicePipeline() 
    {
        Close();
    }
    virtual void Close(void) 
    {
        if (m_render)  { delete m_render; m_render=NULL; }
    }
    virtual bool OnNewFrame(PXCAudio* audio) 
    {
        if (m_render) m_render->UpdateFrame(audio);
        return true;
    }
    int LoopFrames(void) 
    {
        pxcStatus sts;
        #ifndef MY_FIX
            PXCSmartPtr<PXCSession> session;
            PXCSmartPtr<PXCVoiceRecognition> vc;
        #endif

        // Create session
        if (session == NULL) {
            sts = PXCSession_Create(&session);
            if (sts < PXC_STATUS_NO_ERROR) {
                MessageBox(NULL,L"Failed to create the PXCSession",L"Failed to Locate PCSDK",MB_OK); 
                return 1;
            }
        }
        // Create PXCVoiceRecognition instance
        if (vc == NULL) {
            sts=session->CreateImpl(0, PXCVoiceRecognition::CUID, (void **)&vc);
            if (sts<PXC_STATUS_NO_ERROR) {
                MessageBox(NULL, L"Voice recognition can not be initialized", L"Error", MB_OK | MB_ICONSTOP); 
                return 1;
            }
        }
        // Find and initilize capture module
        UtilCapture capture(session);
        // Query PXCVoiceRecognition profile
        PXCVoiceRecognition::ProfileInfo profile;
        for (int i=0;;i++) {
            sts=vc->QueryProfile(i, &profile);
            if (sts<PXC_STATUS_NO_ERROR) break;
            sts=capture.LocateStreams(&profile.inputs);
            if (sts>=PXC_STATUS_NO_ERROR) break;
        }
        if (sts<PXC_STATUS_NO_ERROR) {
            MessageBox(NULL,L"This demo applicataion requires to use a DepthSense camera. Please plugin the DepthSense camera and try again.",L"Failed to Locate Device",MB_OK); 
            return 1;
        }
        // Set PXCVoiceRecognition profile
        profile.endOfSentence = 100;
        sts=vc->SetProfile(&profile);
        if (sts<PXC_STATUS_NO_ERROR) {
            MessageBox(NULL, L"Voice recognition can not be configurated", L"Error", MB_OK | MB_ICONSTOP);
            return 1;
        }

        // check grammar mode
        m_sensor->m_dictationMode = FALSE;
        stActionDescriptor* descr = NULL;
        for (int i=0;i<10000;i++) {
            descr = g_Dispatcher.GetAction(i);
            if (descr == NULL) break;
            if (descr->sensorId==CSensor::Sensor_Voice_recognition && descr->eventId==MY_VOICE_ANY_COMMAND) {
                m_sensor->m_dictationMode = TRUE;
                break;
            }
        }

        if (m_sensor->m_dictationMode==FALSE) {
            pxcEnum grammar = 0;
            sts = vc->CreateGrammar(&grammar);
            if (sts<PXC_STATUS_NO_ERROR) {
                MessageBox(NULL,L"Voice recognition :: CreateGrammar",L"Error",MB_OK); 
                return 1;
            }
            for (int i=0;i<(int)m_sensor->m_eventList.size();i++) {
                sts = vc->AddGrammar(grammar, m_sensor->m_eventList[i].id, m_sensor->m_eventList[i].name);
                if (sts<PXC_STATUS_NO_ERROR) {
                    MessageBox(NULL,L"Voice recognition :: AddGrammar",L"Error",MB_OK); 
                    return 1;
                }
            }
            sts = vc->SetGrammar(grammar);
            if (sts<PXC_STATUS_NO_ERROR) {
                MessageBox(NULL,L"Voice recognition :: SetGrammar",L"Error",MB_OK); 
                return 1;
            }
        }

        VoiceAction handler(m_sensor);
        vc->SubscribeRecognition(0, &handler); 

        // Processing loop
        while (!m_sensor->stop) {
            PXCSmartPtr<PXCAudio> input_audio;
            PXCSmartSPArray sps(3);
            sts = capture.ReadStreamAsync(input_audio.ReleaseRef(),sps.ReleaseRef(0));
            if (sts==PXC_STATUS_DEVICE_LOST) {
		        Sleep(5);
                continue;
            }
            if (sts<PXC_STATUS_NO_ERROR) break;
            sts=vc->ProcessAudioAsync(input_audio,sps.ReleaseRef(1));
            if (sts<PXC_STATUS_NO_ERROR) break;
            sps.SynchronizeEx();
            OnNewFrame(input_audio);
        }
        vc->ProcessAudioAsync(0,0); // Wait until the remaining processing is completed.
        vc->SubscribeRecognition(0,0);
        vc->SubscribeAlert(0);
        return 0;
    }
    CSensorVoice* m_sensor;
protected:
    CSensor::SENSOR_ID  m_id;
    CAUXAudioRender* m_render;
};

///////////////////////

int CSensorVoice::OnInit()
{
    wsprintf(m_name, L"Voice Recognition");
    m_id = MY_SENSOR_ID;
    m_eventList.clear();
    m_eventList.push_back(st_eventDescriptor( MY_VOICE_ANY_COMMAND, L"__any__" ));
    m_dictationMode = FALSE;
    return 0;
}

void CSensorVoice::OnStart()
{
    m_pipeline = NULL;
    while(!stop) {
        VoicePipeline* pipeline = new VoicePipeline(this->video, this);
        pipeline->LoopFrames();
        delete pipeline;
    }
}

void CSensorVoice::OnStop()
{
}

