#version 330 core

out vec4 FragColor;

uniform vec4 u_color;
uniform bool inverted;

void main() {
    if (u_color.a < 0.01)
        discard;

    if (inverted && (u_color.r + u_color.g + u_color.b) >= .75*3.0)
        FragColor = vec4(1.0 - u_color.rgb, 1.0);
    else
        FragColor = vec4(u_color.rgb, 1.0);
}