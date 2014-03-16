/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/

#pragma once
#include <Windows.h>
#include "pxcaudio.h"
#include "pxcimage.h"
#include "AUXWidget.h"

#define AUDIO_SAMPLE_MAX_CHANNEL 10

class CAUXAudioRender : public CAUXWidget
{
public:
    CAUXAudioRender(int posX, int posY, pxcCHAR *title );
    virtual ~CAUXAudioRender(void);

    void SetSize(pxcU32 width, pxcU32 height);
    bool UpdateFrame(PXCAudio *audio);
    void Release() { delete this; };
protected:
    double  m_volumeLevel[AUDIO_SAMPLE_MAX_CHANNEL];
    int     m_numChannels;
    int     m_countFrame;
    virtual void OnPaint(HDC hdc);

    HBRUSH m_hbrFrame;
};

