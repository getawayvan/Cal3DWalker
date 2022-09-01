#version 130

in vec3 inPosition;
in vec3 inNormal;
in vec2 inTexCoord;

uniform mat4 modelToWorldMatrix;
uniform mat4 modelToProjectionMatrix;

out vec3 positionWorld;
out vec3 normalWorld;
out vec2 texCoordWorld;

void main() {
    positionWorld = vec3(modelToWorldMatrix * vec4(inPosition, 1.0));
    normalWorld = vec3(modelToWorldMatrix * vec4(inNormal, 0.0));
    //texCoordWorld = vec2(inTexCoord.x, 1 - inTexCoord.y);
	texCoordWorld = inTexCoord;

    gl_Position = modelToProjectionMatrix * vec4(inPosition, 1.0);
}
