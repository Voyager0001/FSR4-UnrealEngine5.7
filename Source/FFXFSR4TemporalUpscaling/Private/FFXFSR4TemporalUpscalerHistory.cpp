// This file is part of the FidelityFX Super Resolution 4.0 Unreal Engine Plugin.
//
// Copyright (c) 2023-2025 Advanced Micro Devices, Inc. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "FFXFSR4TemporalUpscalerHistory.h"
#include "FFXFSR4TemporalUpscaler.h"
#include "FFXFSR4TemporalUpscaling.h"

const TCHAR* FFXFSR4TemporalUpscalerHistory::FfxFsr4DebugName = TEXT("FFXFSR4TemporalUpscaler");

TCHAR const* FFXFSR4TemporalUpscalerHistory::GetUpscalerName()
{
	return FfxFsr4DebugName;
}

uint64 FFXFSR4TemporalUpscalerHistory::GetFsrHistoryIdFromDebugName()
{
	return *(uint32*)FFXFSR4TemporalUpscalerHistory::FfxFsr4DebugName;
}

FFXFSR4TemporalUpscalerHistory::FFXFSR4TemporalUpscalerHistory(FSR4StateRef NewState, FFXFSR4TemporalUpscaler* _Upscaler, TRefCountPtr<IPooledRenderTarget> InMotionVectors)
{
	FsrHistoryId = FFXFSR4TemporalUpscalerHistory::GetFsrHistoryIdFromDebugName();
	MotionVectors = InMotionVectors;
	Upscaler = _Upscaler;
	SetState(NewState);
}

FFXFSR4TemporalUpscalerHistory::~FFXFSR4TemporalUpscalerHistory()
{
	if (FFXFSR4TemporalUpscalingModule::IsInitialized())
	{
		Upscaler->ReleaseState(Fsr4);
	}
}

bool FFXFSR4TemporalUpscalerHistory::HasFsrHistoryId() const {
	return FsrHistoryId == FFXFSR4TemporalUpscalerHistory::GetFsrHistoryIdFromDebugName();
}

#if UE_VERSION_AT_LEAST(5, 3, 0)
const TCHAR* FFXFSR4TemporalUpscalerHistory::GetDebugName() const {
	// this has to match FFXFSR4TemporalUpscalerHistory::GetDebugName()
	return FfxFsr4DebugName;
}

uint64 FFXFSR4TemporalUpscalerHistory::GetGPUSizeBytes() const {
	// 5.3 not done
	return 0;
}
#endif

void FFXFSR4TemporalUpscalerHistory::SetState(FSR4StateRef NewState)
{
	Upscaler->ReleaseState(Fsr4);
	Fsr4 = NewState;
}

ffxContext* FFXFSR4TemporalUpscalerHistory::GetFSRContext() const
{
	return Fsr4.IsValid() ? &Fsr4->Fsr4 : nullptr;
}

ffxCreateContextDescUpscale* FFXFSR4TemporalUpscalerHistory::GetFSRContextDesc() const
{
	return Fsr4.IsValid() ? &Fsr4->Params : nullptr;
}

TRefCountPtr<IPooledRenderTarget> FFXFSR4TemporalUpscalerHistory::GetMotionVectors() const
{
	return Fsr4.IsValid() ? MotionVectors : nullptr;
}