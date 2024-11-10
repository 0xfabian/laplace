#include <input.h>

std::unordered_map<SDL_Keycode, InputState> keys;
std::unordered_map<int, InputState> buttons;

void update_inputs()
{
    for (auto pair : keys)
        if (pair.second == InputState::DOWN)
            keys[pair.first] = InputState::PRESSED;
        else if (pair.second == InputState::UP)
            keys[pair.first] = InputState::NONE;

    for (auto pair : buttons)
        if (pair.second == InputState::DOWN)
            buttons[pair.first] = InputState::PRESSED;
        else if (pair.second == InputState::UP)
            buttons[pair.first] = InputState::NONE;
}

bool is_key_down(SDL_Keycode key)
{
    return keys[key] == InputState::DOWN;
}

bool is_key_pressed(SDL_Keycode key)
{
    return keys[key] == InputState::PRESSED;
}

bool is_key_up(SDL_Keycode key)
{
    return keys[key] == InputState::UP;
}

bool is_button_down(int button)
{
    return buttons[button] == InputState::DOWN;
}

bool is_button_pressed(int button)
{
    return buttons[button] == InputState::PRESSED;
}

bool is_button_up(int button)
{
    return buttons[button] == InputState::UP;
}