#version 130

in vec3 positionWorld;
in vec3 normalWorld;
in vec2 texCoordWorld;

uniform vec3 lightPositionWorld;
uniform vec3 diffuseLight;
uniform vec3 specularLight;
uniform vec3 ambientLight;

uniform sampler2D textureSampler;

out vec4 outColor;

void main() {
    // difusse
    vec3 lightVectorWorld = normalize(lightPositionWorld + positionWorld);
    float brightness = dot(lightVectorWorld, normalize(normalWorld));
    vec4 diffuseColor = vec4(diffuseLight * brightness, 1.0);

    // specular
    vec3 reflectedLightVectorWorld = reflect(-lightVectorWorld, normalWorld);
    vec3 eyeVectorWorld = normalize(vec3(0.0, 0.0, 0.0) + positionWorld);
    float specularity = dot(reflectedLightVectorWorld, eyeVectorWorld);
    vec4 specularColor = vec4(specularLight * pow(specularity, 8), 1.0);

    vec4 lightColor = clamp(diffuseColor, 0, 1) + clamp(specularColor, 0, 1) + vec4(ambientLight, 1.0);

	//outColor = texture2D(textureSampler, texCoordWorld);
    outColor = texture2D(textureSampler, texCoordWorld) * lightColor;
}
