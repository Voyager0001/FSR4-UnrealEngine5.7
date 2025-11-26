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

#include "FFXFSR4TemporalUpscaling.h"
#include "FFXFSR4TemporalUpscaler.h"
#include "FFXFSR4ViewExtension.h"
#include "LogFFXFSR4.h"

#include "CoreMinimal.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/ConfigCacheIni.h"
#if UE_VERSION_AT_LEAST(5, 1, 0)
#include "Misc/ConfigUtilities.h"
#endif

#if UE_VERSION_AT_LEAST(5, 2, 0)
#include "DataDrivenShaderPlatformInfo.h"
#else
#include "RHIDefinitions.h"
#endif

#if UE_VERSION_OLDER_THAN(5, 3, 0)
#include "PostProcess/TemporalAA.h"
#endif

IMPLEMENT_MODULE(FFXFSR4TemporalUpscalingModule, FFXFSR4TemporalUpscaling)

#define LOCTEXT_NAMESPACE "FSR4"

DEFINE_LOG_CATEGORY(LogFSR4);

static bool GFFXFSR4TemporalUpscalingModuleInit = false;

void FFXFSR4TemporalUpscalingModule::StartupModule()
{
	FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("FSR4"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/Plugin/FSR4"), PluginShaderDir);
	FCoreDelegates::OnPostEngineInit.AddRaw(this, &FFXFSR4TemporalUpscalingModule::OnPostEngineInit);
	GFFXFSR4TemporalUpscalingModuleInit = true;
	UE_LOG(LogFSR4, Log, TEXT("FSR4 Temporal Upscaling Module Started"));
}

void FFXFSR4TemporalUpscalingModule::ShutdownModule()
{
	GFFXFSR4TemporalUpscalingModuleInit = false;
	UE_LOG(LogFSR4, Log, TEXT("FSR4 Temporal Upscaling Module Shutdown"));
}

bool FFXFSR4TemporalUpscalingModule::IsInitialized()
{
	return GFFXFSR4TemporalUpscalingModuleInit;
}

void FFXFSR4TemporalUpscalingModule::SetTemporalUpscaler(TSharedPtr<FFXFSR4TemporalUpscaler, ESPMode::ThreadSafe> Upscaler)
{
	TemporalUpscaler = Upscaler;
}

void FFXFSR4TemporalUpscalingModule::OnPostEngineInit()
{
	ViewExtension = FSceneViewExtensions::NewExtension<FFXFSR4ViewExtension>();
}

FFXFSR4TemporalUpscaler* FFXFSR4TemporalUpscalingModule::GetFSR4Upscaler() const
{
	return TemporalUpscaler.Get();
}

IFFXFSR4TemporalUpscaler* FFXFSR4TemporalUpscalingModule::GetTemporalUpscaler() const
{
	return TemporalUpscaler.Get();
}

float FFXFSR4TemporalUpscalingModule::GetResolutionFraction(uint32 Mode) const
{
	return TemporalUpscaler->GetResolutionFraction(Mode);
}

bool FFXFSR4TemporalUpscalingModule::IsPlatformSupported(EShaderPlatform Platform) const
{
	FStaticShaderPlatform ShaderPlatform(Platform);
	
	// All we need is SM5, which can run the RHI backend. Specific backends are handled elsewhere.
	bool bIsSupported = IsFeatureLevelSupported(Platform, ERHIFeatureLevel::SM5);
	
	return bIsSupported;
}

void FFXFSR4TemporalUpscalingModule::SetEnabledInEditor(bool bEnabled)
{
#if WITH_EDITOR
	return TemporalUpscaler->SetEnabledInEditor(bEnabled);
#endif
}

#undef LOCTEXT_NAMESPACE