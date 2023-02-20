#version 330

uniform bool finished;
uniform int maxIter;
uniform sampler2D frameBuffer[8];

in vec3 position;
out vec4 FragColor;

void main() {
    vec3 color = vec3(0.0);
    vec2 pixel = position.xy * 0.5 + 0.5;
    if (finished) {
        for (int i = 0; i < maxIter; i++) {
            color += texture2D(frameBuffer[i], pixel).rgb;
        }
        color /= maxIter;
    } else {
        color += texture2D(frameBuffer[0], pixel).rgb;
    }
    color = pow(color, vec3(1.0 / 2.2)); //伽马校正
    FragColor = vec4(color, 1.0);
}