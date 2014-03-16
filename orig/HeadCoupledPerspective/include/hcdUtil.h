//--------------------------------------------------------------------------------------
// Copyright 2011 Intel Corporation
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

// 
// Helpful definitions and classes go here.  Everything is SDK-agnostic, so can be
// included by both CPUT and PC-SDK dependant files
//

#pragma once

#include "Windows.h"

//#define _DBGOUT_ON_RELEASE    // Force debug output on release builds
#if defined(DEBUG) | defined(_DEBUG) | defined (_DBGOUT_ON_RELEASE)
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#define DBGOUT(...) printf(__VA_ARGS__)
#else
#define DBGOUT(...)
#endif

// Useful definitions
#define CONST_PI    3.141592654f
#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif
#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

struct DetectionData
{
    UINT imageWidth, imageHeight;
    float faceX, faceY;
    float zoom;
};

// Simple QueryPerformanceCounter()-based timer class
//-----------------------------------------------------------------------------
class Timer
{
public:
    Timer();
    void   Start();
    double Stop();
    double GetTotal();
    double GetElapsed();
    void   Reset();

private:
    bool m_bCounting;
    LARGE_INTEGER m_lStartCount;
    LARGE_INTEGER m_lLastMeasured;
    LARGE_INTEGER m_lFrequency;
};
