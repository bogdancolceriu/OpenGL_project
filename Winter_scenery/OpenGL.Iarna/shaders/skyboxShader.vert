#version 410 core

layout(location = 0) in vec3 position;
out vec3 fragTexCoords;

uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

void main()
{
    vec4 transformedPos = projMatrix * viewMatrix * vec4(position, 1.0);
    gl_Position = vec4(transformedPos.xy, transformedPos.w, transformedPos.w);
    fragTexCoords = position;
}
