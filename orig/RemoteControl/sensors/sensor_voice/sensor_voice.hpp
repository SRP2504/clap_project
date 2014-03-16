/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/

#pragma once

#include "..\common\sensor.h"

class CSensorVoice : public CSensor
{
public:
    CSensorVoice() { CSensor(); OnInit();};
    virtual ~CSensorVoice() { OnStop();};

    virtual void OnStart();
    virtual void OnStop();
    virtual int  OnInit();
    BOOL m_dictationMode;
};
