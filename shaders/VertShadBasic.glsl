#version 130

in vec3 inPosition;
in vec3 inColor;

uniform mat4 modelToProjectionMatrix;

out vec3 fragColor;

void main() {
    fragColor = inColor;
    gl_Position = modelToProjectionMatrix * vec4(inPosition, 1.0);
}
