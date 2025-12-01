#version 330 core

out vec4 FragColor;
uniform vec4 u_color;


void main() {
    FragColor = vec4(1.0-u_color.rbg,1.0);
}