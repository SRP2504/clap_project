/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/

#include <tchar.h>
#include <math.h>
#include "AUXAudioRender.h"
#include "pxcsmartptr.h"

CAUXAudioRender::CAUXAudioRender(int posX, int posY, pxcCHAR *title) : CAUXWidget(posX, posY, title)
{
    m_hbrFrame = CreateSolidBrush(RGB(0,162,232));
    for (int i=0; i<AUDIO_SAMPLE_MAX_CHANNEL; i++) m_volumeLevel[i] = 0.01;
    m_countFrame = 0;
}

CAUXAudioRender::~CAUXAudioRender(void) 
{
    if (m_hbrFrame) DeleteObject(m_hbrFrame);
}

void CAUXAudioRender::SetSize(pxcU32 width, pxcU32 height) 
{
    GetWindowInfo(m_hWnd, &this->m_lastWinInfo);
    SetWindowPos( m_hWnd, NULL, m_lastWinInfo.rcWindow.left, m_lastWinInfo.rcWindow.top, 
        m_lastWinInfo.rcClient.right-m_lastWinInfo.rcClient.left, m_lastWinInfo.rcClient.bottom-m_lastWinInfo.rcClient.top, 0);//SWP_SHOWWINDOW );
}

bool CAUXAudioRender::UpdateFrame(PXCAudio *audio) 
{
    if (!audio) return false;
    PXCAudio::AudioInfo info;
    audio->QueryInfo(&info);
    PXCAudio::AudioData data;
    pxcStatus sts=audio->AcquireAccess(PXCAudio::ACCESS_READ,PXCAudio::AUDIO_FORMAT_IEEE_FLOAT,&data);
    if (sts < PXC_STATUS_NO_ERROR) return false;
    // find average level
    m_numChannels = min(info.nchannels, AUDIO_SAMPLE_MAX_CHANNEL);
    int nsamples = (int)(data.dataSize/info.nchannels);
    float *srcStream = (float*)data.dataPtr;
    for (int i=0; i<nsamples; ++i) {
        for (int j=0; j<m_numChannels; ++j) {
            m_volumeLevel[j] += abs(srcStream[j]);
        }
        srcStream += info.nchannels;
    }
    audio->ReleaseAccess(&data);
    for (int k=0; k<m_numChannels; ++k) {
        m_volumeLevel[k] = sqrt(m_volumeLevel[k]/nsamples); 
    }
    if (m_countFrame==0) InvalidateRect(m_hWnd, 0, FALSE);
    ++m_countFrame%=3;
    DoMessageLoop();
    return true;
}

void CAUXAudioRender::OnPaint(HDC hdc)
{
    RECT rect;
    GetClientRect(m_hWnd, &rect);

    FillRect(hdc, &rect, GetSysColorBrush(COLOR_BACKGROUND));
    int width = rect.right-rect.left;
    int dX = width/(m_numChannels*2);
    GRADIENT_RECT gRect;
    gRect.UpperLeft  = 0;
    gRect.LowerRight = 1;
    for (int i=0; i<m_numChannels; ++i) {
        // Draw a gradient bar 
        #define BRIGHT 0xefff
        TRIVERTEX vertex[2];
        vertex[0].x     = rect.left+dX*i*2+dX/2;
        vertex[0].y     = rect.bottom;
        vertex[0].Red   = 0;
        vertex[0].Green = BRIGHT;
        vertex[0].Blue  = 0;
        vertex[0].Alpha = 0;
        vertex[1].x     = vertex[0].x+dX;
        vertex[1].y     = rect.bottom-LONG(m_volumeLevel[i]*rect.bottom); 
        vertex[1].Red   = COLOR16(m_volumeLevel[i]*BRIGHT);
        vertex[1].Green = COLOR16((1.f-m_volumeLevel[i])*BRIGHT);
        vertex[1].Blue  = 0;
        vertex[1].Alpha = 0;
        #undef BRIGHT
        ::GradientFill(hdc, vertex, 2, &gRect, 1, GRADIENT_FILL_RECT_V);
    }
    // draw contour
    FrameRect(hdc, &rect, m_hbrFrame); 
}

