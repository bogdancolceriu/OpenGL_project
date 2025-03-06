#version 410 core

in vec2 fragTexCoords;

out vec4 finalColor;

uniform sampler2D shadowTexture;

void main() 
{
    float depthValue = texture(shadowTexture, fragTexCoords).r;
    finalColor = vec4(vec3(depthValue), 1.0);
}
