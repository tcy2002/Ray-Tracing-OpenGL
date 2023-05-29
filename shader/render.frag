#version 450 core

uniform sampler2D frameBuffer;
uniform int maxFrame;

in vec3 position;
out vec3 FragColor;

void main() {
    vec2 pixel = position.xy * 0.5 + 0.5;
    vec3 color = texture(frameBuffer, pixel).xyz;
//    vec3 color = texture(frameBuffer, pixel).xyz / maxFrame;
    FragColor = pow(color / maxFrame, vec3(1.0 / 2.2)); //伽马校正
//    FragColor = color / maxFrame;
}