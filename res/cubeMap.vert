#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;

    // w is calculated when we get normalized device coordinates from the projection matrix.
    // w of gl_Position holds the far clipping plane.
    // by setting the z value of gl_Position to w then it is in the furthest possible 
    // distance from the camera.
}  