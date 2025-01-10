// from https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping

#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;        // Vertex normals
layout (location = 2) in vec3 aVertexColour;  // the colour variable has attribute position 1
layout (location = 3) in vec2 aTextureCoord;  // the texture coordinate variable has attribute position 2

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
    gl_Position = lightSpaceMatrix * model * vec4(aPosition, 1.0);
}