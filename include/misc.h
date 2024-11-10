#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstdint>
#include <glm.hpp>

char* read_from_file(const char* path);

void crash(bool condition, const char* msg, int code);

glm::vec3 hsv_to_rgb(float h, float s, float v);
glm::vec3 hue_from_angle(float angle);

float randf(float min, float max);

bool open_file_dialog(std::string& file);