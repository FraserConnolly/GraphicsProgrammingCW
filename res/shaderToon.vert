#version 400

// Inputs
layout (location = 0) in vec3 aPosition;      // Vertex positions
layout (location = 1) in vec3 aNormal;        // Vertex normals
layout (location = 2) in vec3 aVertexColour;  // the colour variable has attribute position 1
layout (location = 3) in vec2 aTextureCoord;  // the texture coordinate variable has attribute position 2

// Uniforms
uniform mat4 model;     // Model matrix
uniform mat4 view;
uniform mat4 projection;

out vec3 v_norm;
out vec4 v_pos; 
out vec3 vertexColour; 
out vec2 textureCoord;

void main()
{
	v_norm = aNormal;
	vertexColour = aVertexColour;
    textureCoord = aTextureCoord;
	mat4 transform = projection * view * model;
	gl_Position = transform * vec4(aPosition, 1.0);
}