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

#include "Modules/ModuleManager.h"
#include "RHIDefinitions.h"
#ifndef RHI_NEW_GPU_PROFILER
#define RHI_NEW_GPU_PROFILER 0
#endif

#include "FFXShared.h"

class FFXFSR4TemporalUpscaler;
class FFXFSR4ViewExtension;
#if UE_VERSION_AT_LEAST(5, 3, 0)
#include "TemporalUpscaler.h"
using IFFXFSR4TemporalUpscaler = UE::Renderer::Private::ITemporalUpscaler;
#else
#include "RHIShaderPlatform.h"
class ITemporalUpscaler;
using IFFXFSR4TemporalUpscaler = ITemporalUpscaler;
#endif

//-------------------------------------------------------------------------------------
// In order for the FSR4 plugin to support the movie render pipeline some functions have to be exposed.
// This allows the separate FSR4MovieRenderPipeline to behave consistently with the main FSR4 plugin.
//-------------------------------------------------------------------------------------
class IFFXFSR4TemporalUpscalingModule : public IModuleInterface
{
public:
	virtual FFXFSR4TemporalUpscaler* GetFSR4Upscaler() const = 0;
	virtual IFFXFSR4TemporalUpscaler* GetTemporalUpscaler() const = 0;
	virtual float GetResolutionFraction(uint32 Mode) const = 0;
	virtual bool IsPlatformSupported(EShaderPlatform Platform) const = 0;
	virtual void SetEnabledInEditor(bool bEnabled) = 0;
};

class FFXFSR4TemporalUpscalingModule final : public IFFXFSR4TemporalUpscalingModule
{
public:
	// IModuleInterface implementation
	void StartupModule() override;
	void ShutdownModule() override;

	static bool IsInitialized();

	void SetTemporalUpscaler(TSharedPtr<FFXFSR4TemporalUpscaler, ESPMode::ThreadSafe> Upscaler);

	void OnPostEngineInit();

	FFXFSR4TemporalUpscaler* GetFSR4Upscaler() const;
	IFFXFSR4TemporalUpscaler* GetTemporalUpscaler() const;
	float GetResolutionFraction(uint32 Mode) const;
	bool IsPlatformSupported(EShaderPlatform Platform) const;
	void SetEnabledInEditor(bool bEnabled);

private:
	TSharedPtr<FFXFSR4TemporalUpscaler, ESPMode::ThreadSafe> TemporalUpscaler;
	TSharedPtr<FFXFSR4ViewExtension, ESPMode::ThreadSafe> ViewExtension;
};