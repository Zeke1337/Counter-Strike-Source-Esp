// Dear ImGui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs


#include <Windows.h>
#include "GL/glew.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <stdio.h>
#include "helper.h"
#include "utilities.h"
#include "WorldToScreen.h"
#include "hack.h"
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers


// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

// This example can also compile and run with Emscripten! See 'Makefile.emscripten' for details.
#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// Main code
int main(int, char**)
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_HIDE);

    const char* glsl_version = "#version 130";
  


    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize OpgenGL Loader");
    }


    GLFWmonitor* Monitor = glfwGetPrimaryMonitor();
    if (!Monitor)
    {
        return 0;
    }

    glfwWindowHint(GLFW_FLOATING, true);
    glfwWindowHint(GLFW_MAXIMIZED, true);
    glfwWindowHint(GLFW_RESIZABLE, false);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, true);
   

    int width = glfwGetVideoMode(Monitor)->width;
    int height = glfwGetVideoMode(Monitor)->height;

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(width, height, "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
    if (window == nullptr)
        return 1;

    glfwSetWindowAttrib(window, GLFW_DECORATED, false);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

   

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    

    // Our state
    bool show_another_window = false;
    bool bMenuVisible = true;
    bool enable_enemy_esp = false;
    bool enable_team_esp = false;
    bool enable_enemy_healthbar = false;
    bool enable_enemy_armorbar = false;
    bool enable_team_healthbar = false;
    bool enable_team_armorbar = false;
    bool enable_fov = false;
    bool enable_no_flash = false;
    bool enable_thirdperson = false;
    float color_team_box[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
    float color_enemy_box[4] = { 1.0f, 0.0f, 0.0f, 0.0f };
    float color_team_snaplines[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
    float color_enemy_snaplines[4] = { 1.0f, 0.0f, 0.0f, 0.0f };
    bool enable_enemy_snaplines = false;
    bool enable_team_snaplines = false;
    int fov_value = 90;

    DWORD procId = GetProcId(L"hl2.exe");

    if (procId)
        std::cout << "Process Found:" << procId << std::endl;

    uintptr_t moduleBase = GetModuleBaseAddr(procId, L"client.dll");

    uintptr_t engineBase = GetModuleBaseAddr(procId, L"engine.dll");

    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, procId);

    uintptr_t localPlayer = Memory::RPM<uintptr_t>(hProc, (moduleBase + 0x5044A4));

   


    // Main loop
#ifdef __EMSCRIPTEN__
    io.IniFilename = nullptr;
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    while (!glfwWindowShouldClose(window))
#endif
    {
        
        glfwPollEvents();

        // Start the Dear ImGui frame
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (GetAsyncKeyState(VK_INSERT) & 1)
        {
            bMenuVisible = !bMenuVisible;


            if (bMenuVisible)
            {
                ShowMenu(window);
            }
            else
            {
                HideMenu(window);
            }
        }


        if(bMenuVisible)
        {
            //Draw menu

            ImGui::Begin("xphax", NULL, ImGuiWindowFlags_AlwaysAutoResize);

            ImGui::Checkbox("Team Box", &enable_team_esp);
            ImGui::Checkbox("Team Health Bar", &enable_team_healthbar);
            ImGui::Checkbox("Team Armor Bar", &enable_team_armorbar);


            ImGui::Checkbox("Enemy Box", &enable_enemy_esp);
            ImGui::Checkbox("Enemy Health Bar", &enable_enemy_healthbar);
            ImGui::Checkbox("Enemy Armor Bar", &enable_enemy_armorbar);

            

            ImGui::Checkbox("Enable Custom Fov", &enable_fov);
            if (enable_fov)
            {
                ImGui::SliderInt("FOV", &fov_value, 500, 350);
            }

            ImGui::Checkbox("Thirdperson", &enable_thirdperson);

            ImGui::Checkbox("No Flash", &enable_no_flash);
            ImGui::ColorEdit4("Team Esp Color", color_team_box, 0);
            ImGui::ColorEdit4("Enemy Esp Color", color_enemy_box, 0);

            
            ImGui::Checkbox("Team Snaplines", &enable_team_snaplines);
            ImGui::Checkbox("Enemy Snaplines", &enable_enemy_snaplines);
            ImGui::ColorEdit4("Team Snaplines Color", color_team_snaplines, 0);
            ImGui::ColorEdit4("Enemy Snaplines Color", color_enemy_snaplines, 0);
           
            
        }
        else
        {

        }


        
         
        uintptr_t readMatrix = Memory::RPM<uintptr_t>(hProc, (engineBase + 0x5AAAA4));

        ViewMatrix Matrix = Memory::RPM<ViewMatrix>(hProc, (readMatrix + 0x2D4));

        uintptr_t localTeam = Memory::RPM<uintptr_t>(hProc, (localPlayer + 0x9C));

        for (unsigned int i = 1; i < 64; ++i)
        {
            uintptr_t EntityList = Memory::RPM<uintptr_t>(hProc, (moduleBase + 0x4C49D0));

            uintptr_t Entity = Memory::RPM<uintptr_t>(hProc, (EntityList + i * 0x8));

            if (Entity == NULL) continue;

            uintptr_t health = Memory::RPM<uintptr_t>(hProc, (Entity + 0x94));

            uintptr_t armor = Memory::RPM<uintptr_t>(hProc, (Entity + 0x1498));

            uintptr_t liveState = Memory::RPM<uintptr_t>(hProc, (Entity + 0x93));

            uintptr_t EntityTeam = Memory::RPM<uintptr_t>(hProc, (Entity + 0x9C));

            uintptr_t dormant = Memory::RPM<uintptr_t>(hProc, (Entity + 0x17C));

            if (dormant == 65536) continue;


            if (health <= 0) continue;

            if (liveState == 257) continue;


            Vec3 vecOrigin = Memory::RPM<Vec3>(hProc, (Entity + 0x290));

            

            Vec3 headOrigin;
            headOrigin.X = vecOrigin.X;
            headOrigin.Y = vecOrigin.Y;
            headOrigin.Z = vecOrigin.Z + 60.0f;


            Vec2 headCoords;
            Vec2 feetCoords;


            if (!WorldToScreen(vecOrigin, feetCoords, Matrix.Matrix)) continue;

            if (!WorldToScreen(headOrigin, headCoords, Matrix.Matrix)) continue;

           

            if (enable_enemy_esp)
            {
                if (localTeam != EntityTeam)
                {
                    glColor4f(color_enemy_box[0], color_enemy_box[1], color_enemy_box[2], color_enemy_box[3]);
                    DrawBox(feetCoords, headCoords);
                }
            }

            if (enable_team_esp)
            {
                if (localTeam == EntityTeam)
                {
                    
                    glColor4f(color_team_box[0], color_team_box[1], color_team_box[2], color_team_box[3]);
                    DrawBox(feetCoords, headCoords);
                }
            }

            if (enable_team_snaplines)
            {
                if (localTeam == EntityTeam)
                {

                    glColor4f(color_team_snaplines[0], color_team_snaplines[1], color_team_snaplines[2], color_team_snaplines[3]);
                    DrawSnapLines(feetCoords);
                }
            }

            if (enable_enemy_snaplines)
            {
                if (localTeam != EntityTeam)
                {

                    glColor4f(color_enemy_snaplines[0], color_enemy_snaplines[1], color_enemy_snaplines[2], color_enemy_snaplines[3]);
                    DrawSnapLines(feetCoords);
                }
            }


            if(enable_enemy_healthbar)
            {
                if (localTeam != EntityTeam)
                {
                    DrawHealthBar(feetCoords, headCoords, health);
                }
            }

            if (enable_team_healthbar)
            {
                if (localTeam == EntityTeam)
                {
                    DrawHealthBar(feetCoords, headCoords, health);
                }
            }

            if (enable_team_armorbar)
            {
                if (localTeam == EntityTeam)
                {
                    DrawArmorBar(feetCoords, headCoords, armor);
                }
            }

            if (enable_enemy_armorbar)
            {
                if (localTeam != EntityTeam)
                {
                    DrawArmorBar(feetCoords, headCoords, armor);
                }
            }
        }


        if (enable_fov)
        {
            Memory::WPM<uintptr_t>(hProc, (localPlayer + 0x0fe0), fov_value);
        }
        else
        {
            Memory::WPM<uintptr_t>(hProc, (localPlayer + 0x0fe0), 90);
        }

        if (enable_no_flash)
        {
           
            Memory::WPM<uintptr_t>(hProc, (localPlayer + 0x1454), 0);
            Memory::WPM<uintptr_t>(hProc, (localPlayer + 0x1450), 0);
            
        }

        if (enable_thirdperson)
        {
            Memory::WPM<uintptr_t>(hProc, (localPlayer + 0x103C), 1);
        }
        else
        {
            Memory::WPM<uintptr_t>(hProc, (localPlayer + 0x103C), 0);
        }
        
        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}