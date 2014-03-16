/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2013 Intel Corporation. All Rights Reserved.

*******************************************************************************/

//#include "stdafx.h"
#include <Windows.h>
#include "thread.h"

DWORD WINAPI Thread::ThreadCallback(LPVOID hdl) {
    Thread *pthis=(Thread *)hdl;
    pthis->RunThread();
    pthis->thread=0;
    return 0;
}

void Thread::Start(BOOL videoControl) {
    stop=false;
    video= videoControl==TRUE;
    thread=CreateThread(0,0,ThreadCallback,this,0,0);
}

void Thread::Stop(void) {
    stop=true;
}
