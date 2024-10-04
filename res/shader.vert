#version 330 core

// see Shader consts
layout (location = 0) in vec3 aPosition;      // the position variable has attribute position 0
layout (location = 1) in vec3 aNormal;        // not currently implemented
layout (location = 2) in vec3 aVertexColour;  // the colour variable has attribute position 1
layout (location = 3) in vec2 aTextureCoord;  // the texture coordinate variable has attribute position 2
  
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 vertexColour; // output a colour to the fragment shader
out vec2 textureCoord;

void main()
{
    gl_Position = projection * view * model * vec4(aPosition, 1.0); // the 1.0 is the w value which is necessary for the matrix multiplication.
    vertexColour = aVertexColour;
    textureCoord = aTextureCoord;
}  