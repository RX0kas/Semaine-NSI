#version 330 core

out vec4 FragColor;

uniform vec4 u_color;
uniform bool inverted;

void main() {
    // FBO Compatible
    if (u_color.a < 0.01) {
        discard;
    }

    if (inverted && u_color.r+u_color.b+u_color.g>=250*3)
        FragColor = vec4(1-u_color.rbg,1.0);
    else
        FragColor = vec4(u_color.rbg,1.0);
}