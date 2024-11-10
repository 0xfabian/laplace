#version 460 core

in vec3 norm;
in vec3 col;

out vec4 FragColor;

uniform vec3 sun;
uniform vec3 eye;
uniform float intensity;

vec3 jet(float t)
{
    return clamp(vec3(1.5) - abs(2.0 * vec3(t) + vec3(-1, 0, 1)), vec3(0), vec3(1));
}

void main()
{
    vec3 n = normalize(norm);

    float t = clamp(col.x * intensity, -1, 1);

    vec3 color = jet(t);

    vec3 diffuse = color * max(abs(dot(n, -sun)), 0.4);
    vec3 spec = vec3(1) * pow(max(dot(reflect(sun, n), -eye), 0.0), 50.0);

    FragColor = vec4(diffuse + spec, 1);
}