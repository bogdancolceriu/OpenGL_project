#version 410 core

layout(location=0) in vec3 vertexPosition;
layout(location=1) in vec3 vertexNormal;
layout(location=2) in vec2 vertexTexCoords;

out vec3 fragNormalView;
out vec4 fragEyePos;
out vec2 fragTexCoords;
out vec4 fragLightSpacePos;
out vec3 fragWorldPosition;
out vec4 fragModelPos;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjMatrix;
uniform mat3 uNormalMatrix;
uniform mat4 uLightMatrix;

void main() 
{
    fragEyePos = uViewMatrix * uModelMatrix * vec4(vertexPosition, 1.0);
    fragNormalView = normalize(uNormalMatrix * vertexNormal);
    fragWorldPosition = vec3(uModelMatrix * vec4(vertexPosition, 1.0));
    fragTexCoords = vertexTexCoords;
    gl_Position = uProjMatrix * uViewMatrix * uModelMatrix * vec4(vertexPosition, 1.0);
    fragLightSpacePos = uLightMatrix * uModelMatrix * vec4(vertexPosition, 1.0);
    fragModelPos = uModelMatrix * vec4(vertexPosition, 1.0);
}
