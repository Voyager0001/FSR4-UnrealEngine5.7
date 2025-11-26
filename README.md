# FidelityFX Super Resolution 4

The AMD FidelityFX Super Resolution 4 (FSR 4) plugin for Unreal Engine provides a cutting-edge ML upscaler combined with analytical frame generation to deliver a massive increase in framerates in supported games.

The new ML-accelerated AMD FSR 4 upscaling algorithm is trained using high-quality ground truth game data on AMD Instinct™ GPUs and uses the hardware-accelerated feature of the AMD RDNA™ 4 architecture designed to give the user maximum upscaling quality while still providing a next-level gaming experience with a substantial game performance boost.

The package also includes the **FSR4MovieRenderPipeline** plugin which enables use of AMD FSR 4 technology to accelerate rendering when using the Unreal Movie Render Queue.

### v4.0 features:

  - Initial release:
    - FSR 4 4.0.2
    - FSR 3 3.1.5
    - FSR 2 2.3.4
    - Frame Generation 3.1.5

#### Supersedes FSR 3 plugin

The FSR 4 plugin supersedes the FSR 3 plugin and provides FSR 2 Upscaling, FSR 3 Upscaling, Frame Generation and the new ML-accelerated AMD FSR 4 upscaling algorithm within a single plugin.

On AMD Radeon RX 9000 series GPUs or better the plugin will automatically select FSR 4 Upscaling when enabled and fallback to FSR 3 on other hardware.

**Recommendations and Known Issues for both FSR 4 & FSR 3 _must_ be understood and addressed on a per-project basis to achieve optimal quality. See the respective section in this guide.**

#### Single implementation

The FSR 4 plugin provides a single implementation built upon the FidelityFX 2.0 SDK libraries. This allows titles that integrate the FSR 4 plugin to benefit from future automatic updates via future AMD Software: Adrenalin Edition™ releases.

The RHI backend has been removed.

## Known issues

### Plugin engine version warning

When building the Unreal Engine from source it may result in the engine displaying a warning that the FSR 4 plugin was built for a different engine version. When this occurs, shaders may not be cooked and packaged properly. Using the appropriate FSR 4 plugin build for the major and minor version of the engine and modifying the patch number in the FSR4.uplugin file (and optionally the FSR4MovieRenderPipeline.uplugin file) to match the version of engine source being used is sufficient to resolve the error and ensure the shaders are properly cooked.

### World-Position-Offset on static objects

In specific circumstances static objects that use a material with World-Position-Offset do not always generate motion vectors. This affects FSR 4 plugin's ability to correctly upscale such materials and results in blurring/ghosting of the affected objects.

Objects with WPO should render velocity by default. The default is to render velocity in the base pass and this can be changed using `r.VelocityOutputPass`. If it is necessary `r.Velocity.ForceOutput` can be used to force all primitives to emit velocity.

### UE post-processing volume screen-percentage overrides

UE developers should be aware that the FSR 4 plugin's quality mode (when enabled) will determine the screen-percentage and ignores any screen-percentage override present in a post-processing volume. This will result in different visual and performance results.

### UE5.2 RDG debugging

Usage of some RDG debugging features can lead to GPU device removal when Lumen and one of FSR 2/3/4 Upscaling are active. This does not affect execution when not using RDG debugging.

### Direct3D debug errors

When using frame interpolation and `r.FidelityFX.FI.OverrideSwapChainDX12` enabled changing the display mode can cause cross-queue resource access errors when the Direct3D debug layer is enabled. These errors should not affect execution without the debug layer.

## Setup

The FSR 4 and FSR4MovieRenderPipeline plugins are intended for [Unreal Engine 5.2.0](https://github.com/EpicGames/UnrealEngine/releases/tag/5.2.0-release)\* or later.

If you are not a registered Unreal Engine developer, you will need to [follow these instructions](https://github.com/EpicGames/Signup) and register for access to this link.

### Install the plugin

1.  Locate the *Engine/Plugins* directory of your Unreal Engine installation.

2.  Extract the contents of the FSR4.zip file.

3.  Select the sub-folder that corresponds to the Unreal Engine version to be used.

4.  Place the **FSR4** folder within your Unreal Engine source tree at: *Engine/Plugins/Marketplace* (for UE5)
    
    1.  (Optional) Place the **FSR4MovieRenderPipeline** folder within your Unreal Engine source tree at: *Engine/Plugins/Marketplace* (for UE5).

5.  Open your Unreal Engine project.

6.  Navigate to **Edit \> Plugins** in the Unreal Engine toolbar

7.  Within the plugin dialog:
    
    1.  Ensure that All is selected on the left side.
    
    2.  Type fsr into the search box in the top right corner
    
    3.  Select the **Enabled** checkbox for the **FSR 4** plugin.
        
        1.  (Optional) Select the **Enabled** checkbox for the **FSR4MovieRenderPipeline** plugin.
    
    4.  When prompted, click **Restart Now** to apply changes, and restart Unreal Engine.

![](assets/image1.png)

## Plugin configuration

### Usage

**Temporal Upsampling** must be enabled, and **Anti-Aliasing Method** should be set to Temporal Super-Resolution (TSR) in the **Project Settings \> Rendering** window, accessed via **Edit \> Project Settings** in the Unreal Engine toolbar or via Console Variables by enabling `r.TemporalAA.Upsampling` and setting `r.AntiAliasingMethod` to 4.

![](assets/image2.png)

FSR 4 can be enabled or disabled via the **Enabled** option in the **Project Settings \> FidelityFX Super Resolution 4.0** settings window, or with the console variable `r.FidelityFX.FSR4.Enabled` in the configuration files. The variable can be modified at runtime. **However**, this is not guaranteed to be safe when other third-party upscalers are also enabled.

![](assets/image3.png)

### Quality modes

The plugin will use specific quality modes specified via `r.FidelityFX.FSR4.QualityMode`, overriding `r.ScreenPercentage`. The exposed modes are:

  - **Native AA (1.0x)**: `r.FidelityFX.FSR4.QualityMode 0`
    
    Provides an image quality superior to native rendering with a modest performance cost.

  - **Quality (1.5x)**: `r.FidelityFX.FSR4.QualityMode 1`
    
    Provides an image quality equal or superior to native rendering with a significant performance gain.

  - **Balanced (1.7x)**: `r.FidelityFX.FSR4.QualityMode 2`
    
    Offers an ideal compromise between image quality and performance gains.

  - **Performance (2.0x)**: `r.FidelityFX.FSR4.QualityMode 3`
    
    Provides an image quality similar to native rendering with a major performance gain.

  - **Ultra Performance (3.0x)**: `r.FidelityFX.FSR4.QualityMode 4`
    
    Provides the highest performance gain while still maintaining an image quality representative of native rendering.

This can also be set via the **Quality Mode** option in the **Project Settings \> FidelityFX Super Resolution 4.0** settings window.

## Frame Generation

Frame Generation can be enabled or disabled via the **Project Settings \> FidelityFX Super Resolution 4.0** settings panel by changing the setting of **Frame Generation Enabled**. Frame Generation can be used independently of FSR 4 upscaling.

### Direct3D 12

There are additional options to control how Frame Generation executes to take advantage of the proxy IDXGISwapChain implementation.

  - **D3D12 Async. Present**: `r.FidelityFX.FI.OverrideSwapChainDX12`
    
    Enables the FSR 4 proxy IDXGISwapChain implementation that provides superior frame pacing via asynchronous presentation of frames. This is a read-only setting and cannot be changed at runtime, so a restart is required to modify the setting when exposed as an option.

  - **D3D12 Async. Interpolation**: `r.FidelityFX.FI.AllowAsyncWorkloads`
    
    When enabled the frame interpolation executes on the asynchronous queue provided by the FSR 4 proxy IDXGISwapChain implementation which allows frame interpolation to occur concurrently with the beginning of the following game frame. This improves performance compared to running frame interpolation serially with the game rendering.\
    ***NOTE:*** Enabling this mode prevents the plugin from requesting Slate re-render the game UI onto each interpolated and normal game frame, instead the UI is extracted from the normal game frame by comparing the pre-UI and post-UI frame contents. This can result in inferior handling of UI with translucency on interpolated frames.


### Integration instructions

#### RCAS

FidelityFX Super Resolution 4.0 contains a built-in sharpening pass called **Robust Contrast Adaptive Sharpening** that can be configured through the CVar `r.FidelityFX.FSR4.Sharpness`. This is disabled by default. If your project has already integrated [FidelityFX-CAS](https://github.com/GPUOpenSoftware/UnrealEngine/tree/FidelityFXCAS-4.26), it may be necessary to disable FidelityFX CAS - including any in-game menu options - while `r.FidelityFX.FSR4.Sharpness` is enabled to prevent over-sharpening your final renders and improve integration results.

#### World-Position-Offset

For FSR 4 to process materials with World Position Offset and/or World Displacement correctly the `r.Velocity.EnableVertexDeformation` option must be enabled. The `r.FidelityFX.FSR4.ForceVertexDeformationOutputsVelocity` setting is enabled by default and when enabled FSR 4 will force `r.Velocity.EnableVertexDeformation` on. The plugin will also enable `r.Velocity.EnableLandscapeGrass` if it has been disabled.

This option also forces on the console-variable `r.BasePassForceOutputsVelocity` to ensure that all objects render velocity during the base-pass when a project enables `r.BasePassOutputsVelocity`.

#### Multiple upscalers

To switch to or from FSR 4 to another temporal upscaler always ensure that only one external temporal upscaler is enabled at a time. Disable the current upscaler before enabling the desired upscaler

#### Frame Generation

FSR Frame Generation does not requires FSR 4 Upscaling to be enabled and can be enabled with other upscaling algorithms. 

Using the **D3D12 Async. Present** (`r.FidelityFX.FI.OverrideSwapChainDX12`) option is incompatible with other frame interpolation technologies. Only one may be enabled at one time and toggling between them requires a restart of the title.

#### UI Rendering 

There are two mechanisms to render the UI when FSR Frame Generation is enabled with different trade-offs and availability. 

When `r.FidelityFX.FI.UIMode` is set to ‘Slate Redraw’ (0) the UI is rendered via Slate on to both the generated and normal frame. This is done by forcing Slate to re-render the window. This will invoke NativeTick on each Slate widget twice (once for the generated and once for the normal frame) - the second invocation will have a delta-time of 0 unless `r.FidelityFX.FI.ModifySlateDeltaTime` is disabled.

When `r.FidelityFX.FI.OverrideSwapChainDX12` is enabled, setting `r.FidelityFX.FI.UIMode` to ‘UI Extraction’ (1) causes UI to be extracted from the normal frame rendering by comparing the frame-buffer prior to UI rendering with the frame-buffer after UI rendering and then using the result to integrate the UI onto the interpolated frame. This decouples Frame Generation from Slate, NativeTick is not invoked twice, and is automatically enabled by `r.FidelityFX.FI.AllowAsyncWorkloads`. This method generally works acceptably but may produce inferior results with translucent UI elements.

#### Debug UI
When using FSR Frame Generation without `r.FidelityFX.FI.AllowAsyncWorkloads` the Slate UI is rendered twice but this does not include Unreal’s debug UI e.g. the in-game console. For Debug/Development/Test builds `r.FidelityFX.FI.CaptureDebugUI` is enabled to ensure that these UI elements appear but it is disabled for Shipping by default. Enable this setting explicitly to avoid flickering if debug UI elements are visible in a Shipping build.

## Other configurations

| Console Variable                                           | Default Value | Value Range | Details                                                                                                                                                                                                                                                                  |
| -----------------------------------------------------------| ------------- | ----------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| `r.FidelityFX.FSR4.AdjustMipBias`                          | 1             | 0, 1        | Applies negative MipBias to material textures, improving results.                                                                                                                                                                                                        |
| `r.FidelityFX.FSR4.Sharpness`                              | 0             | 0.0 - 1.0   | When greater than 0.0 this enables **Robust Contrast Adaptive Sharpening** Filter to sharpen the output image.                                                                                                                                                           |
| `r.FidelityFX.FSR4.AutoExposure`                           | 0             | 0, 1        | Set to 1 to use FSR 4's own auto-exposure, otherwise the engine's auto-exposure value is used.                                                                                                                                                                            |
| `r.FidelityFX.FSR4.HistoryFormat`                          | 0             | 0, 1        | Selects the bit-depth for the FSR 4 history texture format, defaults to PF\_FloatRGBA but can be set to PF\_FloatR11G11B10 to reduce bandwidth at the expense of quality.                                                                                                 |
| `r.FidelityFX.FSR4.QualityMode`                            | 1             | 0 - 4       | FSR 4 Mode. Lower values yield superior images. High values yield improved performance.                                                                                                                                                                                   |
| `r.FidelityFX.FSR4.ForceVertexDeformationOutputsVelocity`  | 1             | 0, 1        | Force enables materials with World Position Offset and/or World Displacement to output velocities during velocity pass even when the actor has not moved.                                                                                                                |
| `r.FidelityFX.FSR4.ForceLandscapeHISMMobility`             | 0             | 0, 1, 2     | Allow FSR 4 to force the mobility of Landscape actors Hierarchical Instance Static Mesh components that use World-Position-Offset materials so they render valid velocities. Setting 1 is faster on the CPU, 2 is faster on the GPU.                                      |
| `r.FidelityFX.FSR4.UseSSRExperimentalDenoiser`             | 0             | 0, 1        | Enable to use *r.SSR.ExperimentalDenoiser* when FSR 4 is enabled. This is required when *r.FidelityFX.FSR4.CreateReactiveMask* is enabled as the FSR 4 plugin overrides *r.SSR.ExperimentalDenoiser* to capture reflection data to generate the reactive mask.             |
| `r.FidelityFX.FSR4.QuantizeInternalTextures`               | 0             | 0, 1        | Setting this to 1 will round up the size of some internal texture to ensure a specific divisibility. This is only intended for compatibility if required. Default is 0.                                                                                                  |
| `r.FidelityFX.FSR4.EnabledInEditorViewport`                | 0             | 0, 1        | Enable FidelityFX Super Resolution for Temporal Upscale in the Editor viewport by default.                                                                                                                                                                               |
| `r.FidelityFX.FI.Enabled`                                  | 1             | 0, 1        | Enable FidelityFX Frame Interpolation.                                                                                                                                                                                                                                   |
| `r.FidelityFX.FI.CaptureDebugUI`                           | 1             | 0, 1        | Force FidelityFX Frame Interpolation to detect and copy any debug UI which only renders on the first invocation of Slate's DrawWindow command. **Disabled in Shipping builds.**                                                                                          |
| `r.FidelityFX.FI.OverrideSwapChainDX12`                    | 1             | 0, 1        | True to use FSR 4's D3D12 swap-chain override that improves frame pacing, false to use the fallback implementation based on Unreal's RHI.                                                                                                                                 |
| `r.FidelityFX.FI.AllowAsyncWorkloads`                      | 1             | 0, 1        | True to use async. execution of Frame Interpolation, 0 to run Frame Interpolation synchronously with the game.                                                                                                                                                           |
| `r.FidelityFX.FI.UpdateGlobalFrameTime`                    | 0             | 0, 1        | True to update the GAverageMs/GAverageFPS globals with average frame time/FPS calculated by the frame interpolation code.                                                                                                                                                |
| `r.FidelityFX.FI.ShowDebugTearLines`                       | 1             | 0, 1        | Show the debug tear lines when running Frame Interpolation. **Not available in Test/Shipping builds.**                                                                                                                                                                   |
| `r.FidelityFX.FI.ShowDebugView`                            | 0             | 0, 1        | Show the debug view when running Frame Interpolation. **Not available in Test/Shipping builds.**                                                                                                                                                                         |
| `r.FidelityFX.FI.UIMode`                                   | 0             | 0, 1        | The method to render the UI when using Frame Generation. Slate Redraw (0): will cause Slate to render the UI on to both the real & generated images. UI Extraction (1): will compare the pre- & post- UI frame to extract the UI and copy it on to the generated frame.  |
| `r.FidelityFX.FI.ModifySlateDeltaTime`                     | 1             | 0, 1        | Set the FSlateApplication delta time to 0.0 when redrawing the UI for the 'Slate Redraw' UI mode to prevent widgets' NativeTick implementations updating incorrectly, ignored when using 'UI Extraction'.                                                                |
| `r.FidelityFX.FI.UseDistortionTexture`                     | 0             | 0, 1        | Enable distortion texture input using Unreal Engine's distortion texture frame.                                                                                                                     |

</div>

## Movie Render Pipeline plugin

The FSR4MovieRenderPipeline plugin, once enabled, allows using FSR 4 to accelerate rendering of Sequencer cinematics using Unreal's Movie Render Queue.

To use the plugin:

1.  Open your Unreal project.

2.  Open a **Sequencer** cinematic through the Editor.

3.  Select the movie output in the toolbar. ![](assets/image4.png)

4.  Click on **'Unsaved Config'** to open the settings for the Movie Render Queue. ![](assets/image5.png)

5.  Select the **'+ Setting'** option and enable **'FidelityFX Super Resolution 4.0 Settings'**. ![](assets/image6.png)

6.  Then select the new **'FidelityFX Super Resolution 4.0 Settings'** in the list and choose the desired quality mode for rendering. ![](assets/image7.png)

7.  Click Accept and then Render (Local).

8.  The output will be rendered using FSR 4 to upscale the output to the target resolution.

## FSR 3 Specific Guide

The FSR 4 plugin includes FSR 3 Upscaling which is used when not running on an AMD Radeon RX 9000 series or better GPU. As such developers should remain mindful of the requirements for integrating FSR 3 when integrating the FSR 4 plugin.

### Patch the engine. 

For optimal quality when using FSR 3 it is necessary to use Unreal Engine from source code and apply source code patches.

1.  To improve FSR 3's handling of animated opaque materials:
    
    1.  Use: `git apply <VERS>-LitReactiveShadingModel.patch`
        
        1.  Where `<VERS>` should be the engine-version in use.

#### Reactive mask and experimental screen space reflection denoiser

When `r.FidelityFX.FSR4.CreateReactiveMask` is enabled the plugin forces `r.SSR.ExperimentalDenoiser` to 1 to capture the screen space reflections to contribute to the FSR 3 reactive mask, to handle this the initial value of `r.SSR.ExperimentalDenoiser` will be applied to `r.FidelityFX.FSR4.UseSSRExperimentalDenoiser`. Subsequent changes to the value of `r.FidelityFX.FSR4.UseSSRExperimentalDenoiser` will override this.

#### Use Custom Depth/Stencil to write to the Reactive Mask

For FSR 3 the plugin can use a Custom Depth/Stencil pass to setup select objects to write specific values into the reactive mask. To enable a custom Depth/Stencil pass in Unreal Engine 5 go to `Edit > Project Settings > Rendering` and setting `Custom Depth-Stencil Pass` to `Enabled with Stencil`, enabling `Custom Depth Taa Jitter` may reduce jitter. Then select objects in the main editor and navigate to the `Rendering > Advanced` settings and enable `Render CustomDepth Pass` and set the `CustomDepth Stencil Value` to a value between `1-255`, it is important to ensure that masking this value with `r.FidelityFX.FSR4.CustomStencilMask` produces a non-zero result. Set `r.FidelityFX.FSR4.CustomStencilShift` to make sure that the range for `r.FidelityFX.FSR4.CustomStencilMask` begins at `0-n`.

### Animated opaque materials

Animated opaque materials which do not generate motion vectors for the animated content, such as in-world video screens, may also blur or ghost when obscured by static geometry. This can be reduced by ensuring such materials write into the Reactive Mask generated in the plugin.

For the Deferred Renderer this can be achieved by selecting a Shading Model for the *'Reactive Shading Model*' option in the FSR 4 section of the project settings or using the `r.FidelityFX.FSR4.ReactiveMaskForceReactiveMaterialValue` console variable. Materials that use this Shading Model will be treated as reactive. This means that when this Shading Model is selected in the Material Editor that material will write either the value of the CustomData0.x channel, when exposed by the Shading Model, or the value of the console-variable `r.FidelityFX.FSR4.ReactiveMaskForceReactiveMaterialValue` provided it is set to a value greater than 0. Using `r.FidelityFX.FSR4.ReactiveMaskForceReactiveMaterialValue` overrides any material specific reactivity specified in the CustomData0.x channel.

Where selecting an existing shading model is unsuitable follow the installation instructions to install the correct version of the 'LitReactiveShadingModel' engine patch which adds a new *'LitReactive'* shading model that can be used specifically for this purpose.

This problem cannot currently be resolved in the Forward Renderer where the Shading Model cannot be determined by the plugin.

Substrate materials are currently not supported by the LitReactive shading model.

### Optimizing translucency appearance

While the default settings for the FSR 3 Reactive Mask should generate reasonable results, it is important that developers are aware that the appearance can be altered via the `r.FidelityFX.FSR4.ReactiveMask` console-variables. Tuning these variables to suit the content may be necessary to optimise visual results.

### Translucent skyboxes and background planes

When using a skybox or a distant background plane it is beneficial for this to be rendered with the Opaque or Masked shading model when using FSR 3. If these are rendered with the Translucent shading model they will contribute to the FSR 3 translucency and reactive masks, which can result in unnecessary artefacts. This is especially noticeable when other translucent materials are rendered over the top of the skybox/background-plane and the camera moves. This occurs because the plugin cannot distinguish the purpose of individual translucent materials, so they are all treated the same.

To address this issue, the FSR 3 plugin assumes that a translucent skybox or background-plane is used and will fade out translucency contribution based on reconstructed distance from the camera. This will cut out all translucency rendered over distant opaque geometry and can be controlled with the `r.FidelityFX.FSR4.ReactiveMaskTranslucencyMaxDistance` console variable.

When using an opaque skybox or backplane, adjust the `r.FidelityFX.FSR4.ReactiveMaskTranslucencyMaxDistance` console variable to avoid translucency cut-outs.

### Hair and dither effects

FSR 3 does not smooth dither effects in the way other upscalers do. They are retained as thin features which may not be intentional. To avoid this, especially with hair, enable the `r.FidelityFX.FSR4.DeDither` console variable which attempts to smooth dither effects prior to FSR 3 upscaling.

### Legacy Settings

| Console Variable                                           | Default Value | Value Range | Details                                                                                                                                                                                                                                                                  |
| -----------------------------------------------------------| ------------- | ----------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| `r.FidelityFX.FSR4.CreateReactiveMask`                     | 1             | 0, 1        | Enable to generate a mask from the SceneColor, GBuffer, SeparateTranslucency and ScreenspaceReflections that determines how reactive each pixel should be.                                                                                                               |
| `r.FidelityFX.FSR4.ReactiveMaskReflectionScale`            | 0.4           | 0.0 - 1.0   | Scales the Unreal engine reflection contribution to the reactive mask, which can be used to control the amount of aliasing on reflective surfaces.                                                                                                                       |
| `r.FidelityFX.FSR4.ReactiveMaskReflectionLumaBias`         | 0             | 0.0 - 1.0   | Biases the reactive mask by the luminance of the reflection. Use to balance aliasing against ghosting on brightly lit reflective surfaces.                                                                                                                               |
| `r.FidelityFX.FSR4.ReactiveMaskRoughnessScale`             | 0.15          | 0.0 - 1.0   | Scales the GBuffer roughness to provide a fallback value for the reactive mask when screenspace and planar reflections are disabled or do not affect a pixel.                                                                                                            |
| `r.FidelityFX.FSR4.ReactiveMaskRoughnessBias`              | 0.25          | 0.0 - 1.0   | Biases the reactive mask value when screenspace/planar reflections are weak with the GBuffer roughness to account for reflection environment captures.                                                                                                                   |
| `r.FidelityFX.FSR4.ReactiveMaskRoughnessMaxDistance`       | 6000          | 0.0 - INF   | Maximum distance in world units for using material roughness to contribute to the reactive mask, the maximum of this value and View.FurthestReflectionCaptureDistance will be used.                                                                                      |
| `r.FidelityFX.FSR4.ReactiveMaskRoughnessForceMaxDistance`  | 0             | 0, 1        | Enable to force the maximum distance in world units for using material roughness to contribute to the reactive mask rather than using View.FurthestReflectionCaptureDistance.                                                                                            |
| `r.FidelityFX.FSR4.ReactiveMaskTranslucencyBias`           | 1.0           | 0.0 - 1.0   | Scales how much contribution translucency makes to the reactive mask. Higher values will make translucent materials more reactive which can reduce smearing.                                                                                                             |
| `r.FidelityFX.FSR4.ReactiveMaskTranslucencyLumaBias`       | 0.0           | 0.0 - 1.0   | Biases the translucency contribution to the reactive mask by the luminance of the transparency. Higher values will make bright translucent materials more reactive which can reduce smearing.                                                                            |
| `r.FidelityFX.FSR4.ReactiveHistoryTranslucencyBias`        | 0.5           | 0.0 - 1.0   | Scales how much translucency suppresses history via the reactive mask. Higher values will make translucent materials more reactive which can reduce smearing.                                                                                                            |
| `r.FidelityFX.FSR4.ReactiveHistoryTranslucencyLumaBias`    | 0.0           | 0.0 - 1.0   | Biases how much the translucency suppresses history via the reactive mask by the luminance of the transparency. Higher values will make bright translucent materials more reactive which can reduce smearing.                                                            |
| `r.FidelityFX.FSR4.ReactiveMaskPreDOFTranslucencyScale`    | 1.0           | 0.0 - 1.0   | Scales how much contribution pre-Depth-of-Field translucency color makes to the reactive mask. Higher values will make translucent materials more reactive which can reduce smearing.                                                                                    |
| `r.FidelityFX.FSR4.ReactiveMaskPreDOFTranslucencyMax`      | 0             | 0, 1        | Toggle to determine whether to use the `max(SceneColorPostDepthOfField - SceneColorPreDepthOfField)` or `length(SceneColorPostDepthOfField - SceneColorPreDepthOfField)` to determine the contribution of Pre-Depth-of-Field translucency.                               |
| `r.FidelityFX.FSR4.ReactiveMaskTranslucencyMaxDistance`    | 500000        | 0.0 - INF   | Maximum distance in world units for using translucency to contribute to the reactive mask. This is a way to remove sky-boxes and other back-planes from the reactive mask, at the expense of nearer translucency not being reactive.                                     |
| `r.FidelityFX.FSR4.ReactiveMaskReactiveShadingModelID`     | MSM\_NUM      | 0- MSM\_NUM | Treat the specified shading model as reactive, taking the CustomData0.x value as the reactive value to write into the mask.                                                                                                                                              |
| `r.FidelityFX.FSR4.ReactiveMaskForceReactiveMaterialValue` | 0.0           | 0.0-1.0     | Force the reactive mask value for Reactive Shading Model materials, when \> 0 this value can be used to override the value supplied in the Material Graph.                                                                                                               |
| `r.FidelityFX.FSR4.ReactiveMaskDeferredDecalScale`         | 0             | 0.0 - 1.0   | Scales how much deferred decal values contribute to the reactive mask. Higher values will make translucent materials more reactive which can reduce smearing.                                                                       |
| `r.FidelityFX.FSR4.ReactiveHistoryDeferredDecalScale`      | 0             | 0.0 - 1.0   | Scales how much deferred decal values contribute to supressing history. Higher values will make translucent materials more reactive which can reduce smearing.                                                                      |
| `r.FidelityFX.FSR4.ReactiveMaskTAAResponsiveValue`         | 0             | 0.0 - 1.0   | Value to write to reactive mask when pixels are marked in the stencil buffer as TAA Responsive. Higher values will make translucent materials more reactive which can reduce smearing.                                              |
| `r.FidelityFX.FSR4.ReactiveHistoryTAAResponsiveValue`      | 0             | 0.0 - 1.0   | Value to write to reactive history when pixels are marked in the stencil buffer as TAA Responsive. Higher values will make translucent materials more reactive which can reduce smearing.                                           |
| `r.FidelityFX.FSR4.ReactiveMaskCustomStencilScale`         | 0             | 0.0 - 1.0   | Scales how much custom stencil values contribute to the reactive mask. Higher values will make translucent materials more reactive which can reduce smearing.                                                                       |
| `r.FidelityFX.FSR4.ReactiveHistoryCustomStencilScale`      | 0             | 0.0 - 1.0   | Scales how much custom stencil values contribute to supressing history. Higher values will make translucent materials more reactive which can reduce smearing.                                                                      |
| `r.FidelityFX.FSR4.DeDither`                               | 2             | 0, 1, 2     | Enable an extra pass to de-dither rendering before handing over to FSR 3 to avoid over-thinning. Can be set to Full for all pixels or to just around Hair for Deferred Renderer. Defaults to Hair Only.                                                                   |
| `r.FidelityFX.FSR4.VelocityFactor`                         | 1             | 0.0 - 1.0   | Value of 0.0f can improve temporal stability of bright pixels.                                                                                                                                                                      |
| `r.FidelityFX.FSR4.ReactivenessScale`                      | 1             | 0.0 - INF   | Meant for development purpose to test if writing a larger value to reactive mask reduces ghosting.                                                                                                                                                                    |
| `r.FidelityFX.FSR4.ShadingChangeScale`                     | 1             | 0.0 - INF   | Increasing this scales FSR 3 computed shading change value at read to have higher reactiveness.                                                                                                                                                                      |
| `r.FidelityFX.FSR4.AccumulationAddedPerFrame`              | 0.333         | 0.0 - 1.0   | Corresponds to amount of accumulation added per frame at pixel coordinate where disocclusion occurred or when reactive mask value is > 0.0f. Decreasing this and drawing the ghosting object (IE no mv) to reactive mask with value close to 1.0f can decrease temporal ghosting. Decreasing this value could result in more thin feature pixels flickering. |
| `r.FidelityFX.FSR4.MinDisocclutionAccumulation`            | -0.333        | -1.0 - 1.0  | Increasing this value may reduce white pixel temporal flickering around swaying thin objects that are disoccluding one another often. Too high value may increase ghosting. A sufficiently negative value means for pixel coordinate at frame N that is disoccluded, add accumulation starting at frame N+2 based on r.FidelityFX.FSR4.AccumulationAddedPerFrame.  |
| `r.FidelityFX.FSR4.CustomStencilMask`                      | 0             | 0 - 255     | A bitmask 0-255 (0-0xff) used when accessing the custom stencil to read reactive mask values. Setting to 0 will disable use of the custom-depth/stencil buffer. Default is 0.                                                                                            |
| `r.FidelityFX.FSR4.CustomStencilShift`                     | 0             | 0 - 31      | Bitshift to apply to the value read from the custom stencil when using it to provide reactive mask values. Default is 0.                                                                                                                                                 |
|`r.FidelityFX.FSR4.RequestProvider`                         | 0             | 0, 2, 3, 4  | Assigns the Upscaler provider to a specific implementation of FSR.  Can be used to request selection of FSR3 on a configuration that should support FSR4, for example.  Has no effect if the FSR provider cannot be found. A value of 0 selects the highest supported version. |



## Version history

### 4.0.2b

  - Added support for Unreal Engine 5.7.
  - Removed support for Unreal Engine 5.1
  - Added a CVar `r.FidelityFX.FSR4.RequestProvider` to select the underlying FSR upscaler version.
  - Fixed an issue where using FSR4 as a project plugin would cause "missing modules" error on startup.

### 4.0.2a

  - Fix a compilation issue caused by an unnecessary include dependency.

### 4.0.2

  - Initial release.