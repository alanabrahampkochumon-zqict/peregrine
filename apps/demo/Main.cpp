/**
 * @file Main.cpp
 * @author Alan Abraham P Kochumon
 * @date Created on: May 02, 2026
 *
 * @brief Entry point for Demo application.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */


#include <SDL3/SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlgpu3.h>
#include <iostream>
#include <peregrine/allocators/Arena.h>
#include <peregrine/utils/Constants.h>


int main()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Error: SDL_Init(): %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    // Create SDL window graphics context
    const float mainScale                 = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    constexpr SDL_WindowFlags windowFlags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY;
    SDL_Window* window                    = SDL_CreateWindow("Demo Application", static_cast<int>(1280 * mainScale),
                                                             static_cast<int>(720 * mainScale), windowFlags);


    if (window == nullptr)
    {
        SDL_Log("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    // Create GPU Device
    SDL_GPUDevice* sdlGPU = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL |
                                                    SDL_GPU_SHADERFORMAT_MSL | SDL_GPU_SHADERFORMAT_METALLIB,
                                                true, nullptr);
    if (sdlGPU == nullptr)
    {
        SDL_Log("Error: SDL_CreateGPUDevice(): %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    // Claim window for GPU Device
    if (!SDL_ClaimWindowForGPUDevice(sdlGPU, window))
    {
        SDL_Log("Error: SDL_ClaimWindowForGPUDevice(): %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    SDL_SetGPUSwapchainParameters(sdlGPU, window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, SDL_GPU_PRESENTMODE_VSYNC);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls


    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(mainScale); // Bake a fixed style scale. (until we have a solution for dynamic style scaling,
                                    // changing this requires resetting Style + calling this again)
    style.FontScaleDpi = mainScale; // Set initial font scale. (in docking branch: using io.ConfigDpiScaleFonts=true
                                    // automatically overrides this for every window depending on the current monitor)

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForSDLGPU(window);
    ImGui_ImplSDLGPU3_InitInfo initInfo = {};
    initInfo.Device                     = sdlGPU;
    initInfo.ColorTargetFormat          = SDL_GetGPUSwapchainTextureFormat(sdlGPU, window);
    initInfo.MSAASamples                = SDL_GPU_SAMPLECOUNT_1;            // Only used in multi-viewports mode.
    initInfo.SwapchainComposition       = SDL_GPU_SWAPCHAINCOMPOSITION_SDR; // Only used in multi-viewports mode.
    initInfo.PresentMode                = SDL_GPU_PRESENTMODE_VSYNC;
    ImGui_ImplSDLGPU3_Init(&initInfo);

    // Load Fonts
    // - If fonts are not explicitly loaded, Dear ImGui will select an embedded font: either AddFontDefaultVector() or
    // AddFontDefaultBitmap().
    //   This selection is based on (style.FontSizeBase * style.FontScaleMain * style.FontScaleDpi) reaching a small
    //   threshold.
    // - You can load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - If a file cannot be loaded, AddFont functions will return a nullptr. Please handle those errors in your code
    // (e.g. use an assertion, display an error and quit).
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use FreeType for higher quality font rendering.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double
    // backslash \\ !
    // style.FontSizeBase = 20.0f;
    // io.Fonts->AddFontDefaultVector();
    // io.Fonts->AddFontDefaultBitmap();
    // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf");
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf");
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf");
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf");
    // ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf");
    // IM_ASSERT(font != nullptr);

    // Our state
    bool showDemoWindow    = true;
    bool showAnotherWindow = false;
    ImVec4 clearColor      = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your
        // inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or
        // clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or
        // clear/overwrite your copy of the keyboard data. Generally you may always pass all inputs to dear imgui, and
        // hide them from your application based on those two flags. [If using SDL_MAIN_USE_CALLBACKS: call
        // ImGui_ImplSDL3_ProcessEvent() from your SDL_AppEvent() function]
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT)
            {
                done = true;
            }
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window))
            {
                done = true;
            }
        }

        // [If using SDL_MAIN_USE_CALLBACKS: all code below would likely be your SDL_AppIterate() function]
        if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
        {
            SDL_Delay(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplSDLGPU3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code
        // to learn more about Dear ImGui!).
        if (showDemoWindow)
        {
            ImGui::ShowDemoWindow(&showDemoWindow);
        }

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            static float f     = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");        // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &showDemoWindow); // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &showAnotherWindow);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f); // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color",
                              reinterpret_cast<float*>(&clearColor)); // Edit 3 floats representing a color

            if (ImGui::Button(
                    "Button")) // Buttons return true when clicked (most widgets return true when edited/activated)
            {
                counter++;
            }
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (showAnotherWindow)
        {
            ImGui::Begin("Another Window",
                         &showAnotherWindow); // Pass a pointer to our bool variable (the window will have a closing
                                              // button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
            {
                showAnotherWindow = false;
            }
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        ImDrawData* drawData   = ImGui::GetDrawData();
        const bool isMinimized = drawData->DisplaySize.x <= 0.0f || drawData->DisplaySize.y <= 0.0f;

        SDL_GPUCommandBuffer* commandBuffer = SDL_AcquireGPUCommandBuffer(sdlGPU); // Acquire a GPU command buffer

        SDL_GPUTexture* swapChainTexture;
        SDL_WaitAndAcquireGPUSwapchainTexture(commandBuffer, window, &swapChainTexture, nullptr,
                                              nullptr); // Acquire a swapchain texture

        if (swapChainTexture != nullptr && !isMinimized)
        {
            // This is mandatory: call ImGui_ImplSDLGPU3_PrepareDrawData() to upload the vertex/index buffer!
            ImGui_ImplSDLGPU3_PrepareDrawData(drawData, commandBuffer);

            // Setup and start a render pass
            SDL_GPUColorTargetInfo targetInfo = {};
            targetInfo.texture                = swapChainTexture;
            targetInfo.clear_color =
                SDL_FColor{ .r = clearColor.x, .g = clearColor.y, .b = clearColor.z, .a = clearColor.w };
            targetInfo.load_op              = SDL_GPU_LOADOP_CLEAR;
            targetInfo.store_op             = SDL_GPU_STOREOP_STORE;
            targetInfo.mip_level            = 0;
            targetInfo.layer_or_depth_plane = 0;
            targetInfo.cycle                = false;
            SDL_GPURenderPass* renderPass   = SDL_BeginGPURenderPass(commandBuffer, &targetInfo, 1, nullptr);

            // Render ImGui
            ImGui_ImplSDLGPU3_RenderDrawData(drawData, commandBuffer, renderPass);

            SDL_EndGPURenderPass(renderPass);
        }

        // Submit the command buffer
        SDL_SubmitGPUCommandBuffer(commandBuffer);
    }

    // Cleanup
    // [If using SDL_MAIN_USE_CALLBACKS: all code below would likely be your SDL_AppQuit() function]
    SDL_WaitForGPUIdle(sdlGPU);
    ImGui_ImplSDL3_Shutdown();
    ImGui_ImplSDLGPU3_Shutdown();
    ImGui::DestroyContext();

    SDL_ReleaseWindowFromGPUDevice(sdlGPU, window);
    SDL_DestroyGPUDevice(sdlGPU);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
