#version 410 core

layout(location=0) in vec3 vertexPosition;
layout(location=1) in vec3 vertexNormal;
layout(location=2) in vec2 vertexTexCoords;

out vec2 fragTexCoords;

void main() 
{
    fragTexCoords = vertexTexCoords;
    gl_Position = vec4(vertexPosition, 2.0);
}
