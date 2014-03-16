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
#include "hcdUtil.h"

//
// Timer is initially not running and set to a zero state.
// Performance counter frequency is obtained.
//
Timer::Timer():m_bCounting(false)
{
    Reset();

    //
    // Frequency only needs to be collected once.
    //
    QueryPerformanceFrequency(&m_lFrequency);
}

//
// Reset timer to zero
//
void Timer::Reset()
{
    m_lStartCount.QuadPart   = 0;
    m_lLastMeasured.QuadPart = 0;
    m_bCounting              = false;
}

//
// Starts timer and resets everything. If timer is already running,
// nothing happens.
//
void Timer::Start()
{

    if(!m_bCounting)
    {
        Reset();
        m_bCounting = true;
        QueryPerformanceCounter(&m_lLastMeasured);
        m_lStartCount = m_lLastMeasured;
    }

    return;
}

//
// If the timer is running, stops the timer and returns the time in seconds since StartTimer() was called.
//
// If the timer is not running, returns the time in seconds between the
// last start/stop pair.
//
double Timer::Stop()
{
    if(m_bCounting)
    {
        m_bCounting = false;
        QueryPerformanceCounter(&m_lLastMeasured);
    }

    return (((double)(m_lLastMeasured.QuadPart - m_lStartCount.QuadPart)) / ((double)(m_lFrequency.QuadPart)));
}

//
// If the timer is running, returns the total time in seconds since StartTimer was called.
//
// If the timer is not running, returns the time in seconds between the
// last start/stop pair.
//
double Timer::GetTotal()
{
    LARGE_INTEGER temp;

    if (m_bCounting) {
        QueryPerformanceCounter(&temp);
        return ((double)(temp.QuadPart - m_lStartCount.QuadPart) / (double)(m_lFrequency.QuadPart));
    }

    return ((double)(m_lLastMeasured.QuadPart - m_lStartCount.QuadPart) / (double)(m_lFrequency.QuadPart));
}

//
// If the timer is running, returns the total time in seconds that the timer
// has run since it was last started or elapsed time was read from. Updates last measured time.
//
// If the timer is not running, returns 0.
//
double Timer::GetElapsed()
{
    LARGE_INTEGER temp;
    LARGE_INTEGER elapsedTime;
    elapsedTime.QuadPart = 0;

    if (m_bCounting) {
        QueryPerformanceCounter(&temp);
        elapsedTime.QuadPart = temp.QuadPart - m_lLastMeasured.QuadPart;
        m_lLastMeasured = temp;
    }

    return ((double)elapsedTime.QuadPart / (double)m_lFrequency.QuadPart);
}
