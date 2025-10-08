#version 330 core
in vec2 a_position;

uniform float zoom;
uniform vec2 center;

vec2 applyZoom(vec2 point, vec2 center, float zoom) {
    return (point - center) * zoom + center;
}

void main()
{
    vec2 newPoint = applyZoom(a_position, center, zoom);

    gl_Position = vec4(newPoint, 0.0, 1.0);
}