#include <core.h>

using namespace std;
using namespace glm;

Window window;
Camera cam;
double delta_time;
double total_time = 0;

Uint64 this_frame;
Uint64 last_frame;

bool mouse_control;

Shader line_shader;

void core::init(int width, int height, const char* name)
{
    window.width = width;
    window.height = height;
    window.is_open = true;

    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);

    window.sdl_win = SDL_CreateWindow
    (
        name,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window.width,
        window.height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED
    );

    window.gl_context = SDL_GL_CreateContext(window.sdl_win);
    SDL_GL_MakeCurrent(window.sdl_win, window.gl_context);

    gladLoadGLLoader(SDL_GL_GetProcAddress);

    glEnable(GL_DEPTH_TEST);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL(window.sdl_win, window.gl_context);
    ImGui_ImplOpenGL3_Init("#version 330");

    Uint64 this_frame = SDL_GetPerformanceCounter();
    Uint64 last_frame = 0;

    cam.set_aspect(window.width, window.height);
    cam.pos = vec3(0, 0, -10);
    cam.forward = vec3(0, 0, 1);
    cam.up = vec3(0, 1, 0);

    line_shader = Shader("line");
}

void core::clean()
{
    line_shader.destroy();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(window.gl_context);
    SDL_DestroyWindow(window.sdl_win);
    SDL_Quit();
}

void handle_window_event(const SDL_Event& event)
{
    if (event.window.event == SDL_WINDOWEVENT_RESIZED)
    {
        window.width = event.window.data1;
        window.height = event.window.data2;

        glViewport(0, 0, window.width, window.height);
        cam.set_aspect(window.width, window.height);
    }
}

void handle_events()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL2_ProcessEvent(&event);

        switch (event.type)
        {
        case SDL_QUIT:              window.is_open = false;                                 break;
        case SDL_KEYDOWN:           keys[event.key.keysym.sym] = InputState::DOWN;          break;
        case SDL_KEYUP:             keys[event.key.keysym.sym] = InputState::UP;            break;
        case SDL_MOUSEBUTTONDOWN:   buttons[event.button.button] = InputState::DOWN;        break;
        case SDL_MOUSEBUTTONUP:     buttons[event.button.button] = InputState::UP;          break;
        case SDL_WINDOWEVENT:       handle_window_event(event);                             break;
        }
    }
}

void move_camera()
{
    if (is_key_down(SDLK_TAB))
    {
        mouse_control = !mouse_control;

        SDL_ShowCursor(!mouse_control);
        SDL_SetRelativeMouseMode((SDL_bool)mouse_control);
        SDL_WarpMouseInWindow(window.sdl_win, window.width / 2, window.height / 2);
    }

    vec3 right = normalize(cross(cam.up, cam.forward));

    if (mouse_control)
    {
        ivec2 mouse;
        SDL_GetMouseState(&mouse.x, &mouse.y);

        float sens = 0.045f * M_PI / 180.0;
        float rot_x = sens * (mouse.x - window.width / 2);
        float rot_y = sens * (mouse.y - window.height / 2);

        SDL_WarpMouseInWindow(window.sdl_win, window.width / 2, window.height / 2);

        vec3 new_forward = rotate(cam.forward, rot_y, right);

        if (dot(right, cross(cam.up, new_forward)) > 0)
            cam.forward = new_forward;

        cam.forward = rotate(cam.forward, rot_x, cam.up);
    }

    vec3 input = vec3(0);

    if (is_key_pressed(SDLK_w))         input += vec3(0, 0, 1);
    if (is_key_pressed(SDLK_s))         input += vec3(0, 0, -1);
    if (is_key_pressed(SDLK_a))         input += vec3(-1, 0, 0);
    if (is_key_pressed(SDLK_d))         input += vec3(1, 0, 0);
    if (is_key_pressed(SDLK_LCTRL))     input += vec3(0, -1, 0);
    if (is_key_pressed(SDLK_SPACE))     input += vec3(0, 1, 0);

    if (length(input) > 0)
    {
        vec3 norm_input = normalize(input);

        float speed = is_key_pressed(SDLK_LSHIFT) ? 50 : 10;

        cam.pos += (norm_input.x * right + norm_input.y * cam.up + norm_input.z * cam.forward) * speed * (float)delta_time;
    }
}

void core::main_loop()
{
    last_frame = this_frame;
    this_frame = SDL_GetPerformanceCounter();

    delta_time = (this_frame - last_frame) / (double)SDL_GetPerformanceFrequency();
    total_time += delta_time;

    update_inputs();
    handle_events();
    move_camera();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void core::render()
{
    line_shader.bind();
    line_shader.upload_mat4("cam_mat", cam.matrix(vec3(0)));

    draw_lines();

    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(window.sdl_win);
}