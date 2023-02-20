#version 330

layout (location = 1) in vec3 aPosition;

out vec3 position;

void main() {
    position = aPosition;
    gl_Position = vec4(aPosition, 1.0);
}