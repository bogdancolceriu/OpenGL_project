#version 410 core

out vec4 finalColor;

uniform vec3 baseColor;
uniform float alphaValue;

void main() 
{
    finalColor = vec4(baseColor, alphaValue);
}
