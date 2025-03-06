#version 410 core

layout(location=0) in vec3 vertexPosition;
layout(location=1) in vec3 vertexNormal;
layout(location=2) in vec2 vertexTexCoords;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main() 
{
    vec4 worldPosition = modelMatrix * vec4(vertexPosition, 1.0);
    gl_Position = projectionMatrix * viewMatrix * worldPosition;
}
