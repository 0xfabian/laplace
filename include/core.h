#pragma once

#include <SDL.h>
#include <glad/glad.h>
#include <ext.hpp>
#include <gtx/rotate_vector.hpp>
#include <gtx/vector_angle.hpp>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_sdl2.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <camera.h>
#include <input.h>
#include <lines.h>

struct Window
{
    SDL_Window* sdl_win;
    SDL_GLContext gl_context;
    int width;
    int height;
    bool is_open;
};

extern Window window;
extern Camera cam;
extern double delta_time;
extern double total_time;
extern bool mouse_control;

namespace core
{
    void init(int width, int height, const char* name);
    void main_loop();
    void render();
    void clean();
}