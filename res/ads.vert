#version 330 core

// Inputs
layout (location = 0) in vec3 VertexPosition; // Vertex positions
layout (location = 2) in vec3 aNormal;        // Vertex normals

// Uniforms
uniform mat4 model;     // Model matrix
uniform mat4 transform; // Transformation matrix (MVP)

// Outputs to fragment shader
out vec3 FragPos;       // Fragment position in world space
out vec3 Normal;        // Transformed normal for lighting calculations

// Pseudo code for main
void main()
{
    // 1. Transform vertex position to world space using the model matrix
    //    and store it in FragPos.

    // 2. Transform the vertex normal and store it in Normal.
    //    (Hint: The normal transformation needs to account for the model matrix.)

    // 3. Use the transform matrix to project the vertex position to clip space
    //    and store it in gl_Position.
}