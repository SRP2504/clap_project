/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/

#pragma once

#ifndef __AFXWIN_H__
    #error "include 'stdafx.h' before including this file for PCH"
#endif
#include "resource.h"
#include "Dispatcher.h"

class CRemoteControlApp : public CWinApp
{
public:
    CRemoteControlApp();
public:
    virtual BOOL InitInstance();
    DECLARE_MESSAGE_MAP()
};

extern CRemoteControlApp theApp;