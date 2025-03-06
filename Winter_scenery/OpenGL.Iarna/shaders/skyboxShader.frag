#version 410 core

in vec3 fragTexCoords;
out vec4 outputColor;

uniform samplerCube environmentMap;

void main()
{
    outputColor = texture(environmentMap, fragTexCoords);
}
