#version 420 core

// Inputs
layout (location = 0) in vec3 aPosition;      // Vertex positions
layout (location = 1) in vec3 aNormal;        // Vertex normals
layout (location = 2) in vec3 aVertexColour;  // the colour variable has attribute position 1
layout (location = 3) in vec2 aTextureCoord;  // the texture coordinate variable has attribute position 2

out Vertex_DATA{
    vec2 textureCoord;
    vec3 Normal;
    vec3 Position;
} vsOut;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	vsOut.textureCoord = aTextureCoord;
    vsOut.Position = vec3(model * vec4(aPosition, 1.0));
    vsOut.Normal = mat3(transpose(inverse(model))) * aNormal;
    gl_Position = projection * view * model * vec4(aPosition, 1.0);
}  
