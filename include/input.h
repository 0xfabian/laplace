#pragma once

#include <SDL.h>
#include <unordered_map>

enum InputState
{
    NONE = 0,
    DOWN,
    PRESSED,
    UP
};

extern std::unordered_map<SDL_Keycode, InputState> keys;
extern std::unordered_map<int, InputState> buttons;

void update_inputs();

bool is_key_down(SDL_Keycode key);
bool is_key_pressed(SDL_Keycode key);
bool is_key_up(SDL_Keycode key);

bool is_button_down(int button);
bool is_button_pressed(int button);
bool is_button_up(int buttom);