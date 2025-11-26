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

#include "Engine/Engine.h"
#include "PostProcess/PostProcessing.h"
#include "PostProcess/PostProcessUpscale.h"
#include "PostProcess/TemporalAA.h"
#include "ScreenSpaceDenoise.h"
#include "Containers/LockFreeList.h"
#include "FFXFSR4TemporalUpscalerHistory.h"
#include "FFXSharedBackend.h"

#if UE_VERSION_AT_LEAST(5, 3, 0)
#include "TemporalUpscaler.h"
using IFFXFSR4TemporalUpscaler = UE::Renderer::Private::ITemporalUpscaler;
using FFXFSR4PassInput = UE::Renderer::Private::ITemporalUpscaler::FInputs;
using FFXFSR4View = FSceneView;
#else
using IFFXFSR4TemporalUpscaler = ITemporalUpscaler;
using FFXFSR4PassInput = ITemporalUpscaler::FPassInputs;
using FFXFSR4View = FViewInfo;
#endif

#ifndef ENGINE_HAS_DENOISE_INDIRECT
#define ENGINE_HAS_DENOISE_INDIRECT 0
#endif

namespace FFXFSR4Strings
{
	static constexpr auto D3D12 = TEXT("D3D12");
}

struct FPostProcessingInputs;

#if UE_VERSION_OLDER_THAN(5, 0, 0)
//-------------------------------------------------------------------------------------
// Simplifies cross engine support.
//-------------------------------------------------------------------------------------
typedef IScreenSpaceDenoiser::FDiffuseIndirectOutputs FSSDSignalTextures;
#endif

//-------------------------------------------------------------------------------------
// The core upscaler implementation for FSR4.
// Implements IScreenSpaceDenoiser in order to access the reflection texture data.
//-------------------------------------------------------------------------------------
class FFXFSR4TemporalUpscaler final : public IFFXFSR4TemporalUpscaler, public IScreenSpaceDenoiser
{
	friend class FFXFSR4FXSystem;
public:
	FFXFSR4TemporalUpscaler();
	virtual ~FFXFSR4TemporalUpscaler();

	void Initialize() const;

	const TCHAR* GetDebugName() const override;

	void ReleaseState(FSR4StateRef State);

	static class IFFXSharedBackend* GetApiAccessor(EFFXBackendAPI& Api);
	static float GetResolutionFraction(uint32 Mode);

#if DO_CHECK || DO_GUARD_SLOW || DO_ENSURE
	static void OnFSRMessage(uint32 type, const wchar_t* message);
#endif // DO_CHECK || DO_GUARD_SLOW || DO_ENSURE

	static void SaveScreenPercentage();
	static void UpdateScreenPercentage();
	static void RestoreScreenPercentage();

	static void OnChangeFFXFSR4Enabled(IConsoleVariable* Var);
	static void OnChangeFFXFSR4QualityMode(IConsoleVariable* Var);

	class FRDGBuilder* GetGraphBuilder();

#if UE_VERSION_AT_LEAST(5, 0, 0)
	IFFXFSR4TemporalUpscaler::FOutputs AddPasses(
		FRDGBuilder& GraphBuilder,
		const FFXFSR4View& View,
		const FFXFSR4PassInput& PassInputs) const override;
#else		
	void AddPasses(
		FRDGBuilder& GraphBuilder,
		const FFXFSR4View& View,
		const FFXFSR4PassInput& PassInputs,
		FRDGTextureRef* OutSceneColorTexture,
		FIntRect* OutSceneColorViewRect,
		FRDGTextureRef* OutSceneColorHalfResTexture,
		FIntRect* OutSceneColorHalfResViewRect) const override;
#endif

#if UE_VERSION_AT_LEAST(5, 1, 0)
	IFFXFSR4TemporalUpscaler* Fork_GameThread(const class FSceneViewFamily& InViewFamily) const override;
#endif

	float GetMinUpsampleResolutionFraction() const override;
	float GetMaxUpsampleResolutionFraction() const override;

	void SetSSRShader(FGlobalShaderMap* GlobalMap);

	void CopyOpaqueSceneColor(FRHICommandListImmediate& RHICmdList, FRHIUniformBuffer* ViewUniformBuffer, const class FShaderParametersMetadata* SceneTexturesUniformBufferStruct, FRHIUniformBuffer* SceneTexturesUniformBuffer);

	void SetLumenReflections(FSceneView& InView);

	void SetPostProcessingInputs(FPostProcessingInputs const& Inputs);

	void EndOfFrame();

	void UpdateDynamicResolutionState();

#if WITH_EDITOR
	bool IsEnabledInEditor() const;
	void SetEnabledInEditor(bool bEnabled);
#endif

	FReflectionsOutputs DenoiseReflections(
		FRDGBuilder& GraphBuilder,
		const FViewInfo& View,
		FPreviousViewInfo* PreviousViewInfos,
		const FSceneTextureParameters& SceneTextures,
		const FReflectionsInputs& ReflectionInputs,
		const FReflectionsRayTracingConfig RayTracingConfig) const override;

	EShadowRequirements GetShadowRequirements(
		const FViewInfo& View,
		const FLightSceneInfo& LightSceneInfo,
		const FShadowRayTracingConfig& RayTracingConfig) const override;

	void DenoiseShadowVisibilityMasks(
		FRDGBuilder& GraphBuilder,
		const FViewInfo& View,
		FPreviousViewInfo* PreviousViewInfos,
		const FSceneTextureParameters& SceneTextures,
		const TStaticArray<FShadowVisibilityParameters, IScreenSpaceDenoiser::kMaxBatchSize>& InputParameters,
		const int32 InputParameterCount,
		TStaticArray<FShadowVisibilityOutputs, IScreenSpaceDenoiser::kMaxBatchSize>& Outputs) const override;

	FPolychromaticPenumbraOutputs DenoisePolychromaticPenumbraHarmonics(
		FRDGBuilder& GraphBuilder,
		const FViewInfo& View,
		FPreviousViewInfo* PreviousViewInfos,
		const FSceneTextureParameters& SceneTextures,
		const FPolychromaticPenumbraHarmonics& Inputs) const override;

	FReflectionsOutputs DenoiseWaterReflections(
		FRDGBuilder& GraphBuilder,
		const FViewInfo& View,
		FPreviousViewInfo* PreviousViewInfos,
		const FSceneTextureParameters& SceneTextures,
		const FReflectionsInputs& ReflectionInputs,
		const FReflectionsRayTracingConfig RayTracingConfig) const override;

	FAmbientOcclusionOutputs DenoiseAmbientOcclusion(
		FRDGBuilder& GraphBuilder,
		const FViewInfo& View,
		FPreviousViewInfo* PreviousViewInfos,
		const FSceneTextureParameters& SceneTextures,
		const FAmbientOcclusionInputs& ReflectionInputs,
		const FAmbientOcclusionRayTracingConfig RayTracingConfig) const override;

	FSSDSignalTextures DenoiseDiffuseIndirect(
		FRDGBuilder& GraphBuilder,
		const FViewInfo& View,
		FPreviousViewInfo* PreviousViewInfos,
		const FSceneTextureParameters& SceneTextures,
		const FDiffuseIndirectInputs& Inputs,
		const FAmbientOcclusionRayTracingConfig Config) const override;

#if ENGINE_HAS_DENOISE_INDIRECT
	FSSDSignalTextures DenoiseIndirect(
		FRDGBuilder& GraphBuilder,
		const FViewInfo& View,
		FPreviousViewInfo* PreviousViewInfos,
		const FSceneTextureParameters& SceneTextures,
		const FIndirectInputs& Inputs,
		const FAmbientOcclusionRayTracingConfig Config) const override;
#endif

	FDiffuseIndirectOutputs DenoiseSkyLight(
		FRDGBuilder& GraphBuilder,
		const FViewInfo& View,
		FPreviousViewInfo* PreviousViewInfos,
		const FSceneTextureParameters& SceneTextures,
		const FDiffuseIndirectInputs& Inputs,
		const FAmbientOcclusionRayTracingConfig Config) const override;

#if UE_VERSION_OLDER_THAN(5, 4, 0)
	FDiffuseIndirectOutputs DenoiseReflectedSkyLight(
		FRDGBuilder& GraphBuilder,
		const FViewInfo& View,
		FPreviousViewInfo* PreviousViewInfos,
		const FSceneTextureParameters& SceneTextures,
		const FDiffuseIndirectInputs& Inputs,
		const FAmbientOcclusionRayTracingConfig Config) const override;
#endif

#if UE_VERSION_AT_LEAST(5, 0, 0)
	FSSDSignalTextures DenoiseDiffuseIndirectHarmonic(
		FRDGBuilder& GraphBuilder,
		const FViewInfo& View,
		FPreviousViewInfo* PreviousViewInfos,
		const FSceneTextureParameters& SceneTextures,
		const FDiffuseIndirectHarmonic& Inputs,
		const HybridIndirectLighting::FCommonParameters& CommonDiffuseParameters) const override;
#else
	FDiffuseIndirectHarmonic DenoiseDiffuseIndirectHarmonic(
		FRDGBuilder& GraphBuilder,
		const FViewInfo& View,
		FPreviousViewInfo* PreviousViewInfos,
		const FSceneTextureParameters& SceneTextures,
		const FDiffuseIndirectHarmonic& Inputs,
		const FAmbientOcclusionRayTracingConfig Config) const override;
#endif

	bool SupportsScreenSpaceDiffuseIndirectDenoiser(EShaderPlatform Platform) const override;

	FSSDSignalTextures DenoiseScreenSpaceDiffuseIndirect(
		FRDGBuilder& GraphBuilder,
		const FViewInfo& View,
		FPreviousViewInfo* PreviousViewInfos,
		const FSceneTextureParameters& SceneTextures,
		const FDiffuseIndirectInputs& Inputs,
		const FAmbientOcclusionRayTracingConfig Config) const override;

	inline bool IsApiSupported() const
	{
		return Api != EFFXBackendAPI::Unknown && Api != EFFXBackendAPI::Unsupported;
	}

private:
	void DeferredCleanup(bool force = false) const;
	bool QueryCurrentlyUsedResources(IFFXSharedBackend* ApiAccesor, ffxContext* context) const;

	inline bool IsResourceUsedByCurrentUpscaler(uint64_t queryId) const	{ return (CurrentUsedResources & queryId); }
	inline bool IsReactiveMaskUsedByCurrentUpscaler() const	{ return IsResourceUsedByCurrentUpscaler(FFX_API_QUERY_RESOURCE_INPUT_REACTIVEMASK); }
	inline bool IsCompositeMaskUsedByCurrentUpscaler() const { return IsResourceUsedByCurrentUpscaler(FFX_API_QUERY_RESOURCE_INPUT_TRANSPARENCYCOMPOSITION); }
	inline bool ShouldCreateUpscalingMasks() const { return IsReactiveMaskUsedByCurrentUpscaler() || IsCompositeMaskUsedByCurrentUpscaler(); }

	mutable FPostProcessingInputs PostInputs;
	FDynamicResolutionStateInfos DynamicResolutionStateInfos;
	mutable FCriticalSection Mutex;
	mutable TSet<FSR4StateRef> AvailableStates;
	mutable EFFXBackendAPI Api;
	mutable class IFFXSharedBackend* ApiAccessor;
	mutable class FRDGBuilder* CurrentGraphBuilder;
	mutable const IScreenSpaceDenoiser* WrappedDenoiser;
	mutable FRDGTextureRef ReflectionTexture;
	mutable FTextureRHIRef SceneColorPreAlpha;
	mutable TRefCountPtr<IPooledRenderTarget> SceneColorPreAlphaRT;
	mutable FTextureRHIRef CustomStencil;
	mutable TRefCountPtr<IPooledRenderTarget> CustomStencilRT;
	mutable TRefCountPtr<IPooledRenderTarget> MotionVectorRT;
	mutable TRefCountPtr<IPooledRenderTarget> LumenReflections;
	mutable FRDGTextureMSAA PreAlpha;
	mutable uint64 CurrentUsedResources;
#if WITH_EDITOR
	bool bEnabledInEditor;
#endif
	static float SavedScreenPercentage;
#if UE_VERSION_AT_LEAST(5, 2, 0)
	mutable TRefCountPtr<IPooledRenderTarget> ReactiveExtractedTexture;
	mutable TRefCountPtr<IPooledRenderTarget> CompositeExtractedTexture;
#endif
};