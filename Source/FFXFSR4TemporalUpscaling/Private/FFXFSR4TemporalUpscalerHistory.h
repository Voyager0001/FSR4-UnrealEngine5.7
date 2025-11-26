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

#pragma once

#include "CoreMinimal.h"
#include "SceneRendering.h"
#include "FFXFSR4Include.h"
#include "FFXFSR4History.h"

class FFXFSR4TemporalUpscaler;

#define FFX_FSR4UPSCALER_MAX_NUM_BUFFERS 3

//-------------------------------------------------------------------------------------
// The FSR4 state wrapper, deletion is handled by the RHI so that they aren't removed out from under the GPU.
//-------------------------------------------------------------------------------------
struct FFXFSR4State : public FRHIResource
{
	FFXFSR4State(IFFXSharedBackend* InBackend)
	: FRHIResource(RRT_None)
	, Backend(InBackend)
	, LastUsedFrame(~0u)
	{
	}
	~FFXFSR4State()
	{
		Backend->ffxDestroyContext(&Fsr4);
	}

	uint32 AddRef() const
	{
		return FRHIResource::AddRef();
	}

	uint32 Release() const
	{
		return FRHIResource::Release();
	}

	uint32 GetRefCount() const
	{
		return FRHIResource::GetRefCount();
	}

	void PushActivity(FGPUFenceRHIRef Fence)
	{
		// ActiveFences is a single-producer single-consumer queue.  only produce from the RHI thread so we don't have to synchronize.
		check((IsRHIThreadRunning() && IsInRHIThread()) || IsInRenderingThread());

		ActiveFences.Enqueue(Fence);
	}

	bool PollActivity()
	{
		// ActiveFences is a single-producer single-consumer queue.  only consume from the Render thread so we don't have to synchronize.
		check(IsInRenderingThread());

		while (!ActiveFences.IsEmpty())
		{
			FGPUFenceRHIRef* ActiveFence = ActiveFences.Peek();
			if (ActiveFence && ActiveFence->IsValid() && (*ActiveFence)->Poll())
			{
				ActiveFences.Pop();
			}
			else
			{
				break;
			}
		}

		return ActiveFences.IsEmpty();
	}

	IFFXSharedBackend* Backend;
	ffxCreateContextDescUpscale Params;
	ffxContext Fsr4;
	uint64 LastUsedFrame;
	uint32 ViewID;
	uint32 RequestedFSRProvider;

private:
	TQueue<FGPUFenceRHIRef, EQueueMode::Spsc> ActiveFences;
};
typedef TRefCountPtr<FFXFSR4State> FSR4StateRef;

#if UE_VERSION_OLDER_THAN(5, 6, 0)
#define FReturnedRefCountValue uint32
#endif

//-------------------------------------------------------------------------------------
// The ICustomTemporalAAHistory for FSR4, this retains the FSR4 state object.
//-------------------------------------------------------------------------------------
class FFXFSR4TemporalUpscalerHistory final : public IFFXFSR4History, public FRefCountBase
{
public:
	FFXFSR4TemporalUpscalerHistory(FSR4StateRef NewState, FFXFSR4TemporalUpscaler* Upscaler, TRefCountPtr<IPooledRenderTarget> InMotionVectors);

	virtual ~FFXFSR4TemporalUpscalerHistory();

#if UE_VERSION_AT_LEAST(5, 3, 0)
	virtual const TCHAR* GetDebugName() const override;
	virtual uint64 GetGPUSizeBytes() const override;
#endif

	ffxContext* GetFSRContext() const final;
    ffxCreateContextDescUpscale* GetFSRContextDesc() const final;
	TRefCountPtr<IPooledRenderTarget> GetMotionVectors() const final;

	void SetState(FSR4StateRef NewState);

	bool HasFsrHistoryId() const;

	inline FSR4StateRef const& GetState() const
	{
		return Fsr4;
	}
	
	FReturnedRefCountValue AddRef() const final
	{
		return FRefCountBase::AddRef();
	}

	uint32 Release() const final
	{
		return FRefCountBase::Release();
	}

	uint32 GetRefCount() const final
	{
		return FRefCountBase::GetRefCount();
	}

	static TCHAR const* GetUpscalerName();
	static uint64 GetFsrHistoryIdFromDebugName();

private:
	static TCHAR const* FfxFsr4DebugName;
	uint64 FsrHistoryId;
	FSR4StateRef Fsr4;
	FFXFSR4TemporalUpscaler* Upscaler;
	TRefCountPtr<IPooledRenderTarget> MotionVectors;
};
