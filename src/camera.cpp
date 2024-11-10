#include <camera.h>

using namespace glm;

mat4 Camera::matrix(const vec3& offset)
{
    mat4 perpective_mat = mat4(0);

    float scale_y = tanf(radians(fov) / 2.0f);

    perpective_mat[0][0] = 1 / (aspect_ratio * scale_y);
    perpective_mat[1][1] = 1 / (scale_y);
    perpective_mat[2][2] = (far + near) / (far - near);
    perpective_mat[2][3] = 1;
    perpective_mat[3][2] = -2 * far * near / (far - near);

    mat4 view_mat = lookAt(offset + pos, offset + pos - forward, up);

    return perpective_mat * view_mat;
}

void Camera::set_aspect(int width, int height)
{
    aspect_ratio = width / (float)height;
}
