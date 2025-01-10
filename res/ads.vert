#version 330 core

// Inputs
layout (location = 0) in vec3 aPosition;      // Vertex positions
layout (location = 1) in vec3 aNormal;        // Vertex normals
layout (location = 2) in vec3 aVertexColour;  // the colour variable has attribute position 1
layout (location = 3) in vec2 aTextureCoord;  // the texture coordinate variable has attribute position 2

// Uniforms
uniform mat4 model;     // Model matrix
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

// Outputs to fragment shader
out VS_OUT {
    vec3 FragPos;       // Fragment position in world space
    vec3 Normal;        // Transformed normal for lighting calculations
    vec3 vertexColour;  // output a colour to the fragment shader
    vec2 textureCoord;
    vec4 FragPosLightSpace;
} vs_out;

void main()
{

    // 1. Transform vertex position to world space using the model matrix
    //    and store it in FragPos.
    vs_out.FragPos = vec3(model * vec4(aPosition, 1.0));

    // 2. Transform the vertex normal and store it in Normal.
    //    (Hint: The normal transformation needs to account for the model matrix.)
    vs_out.Normal = mat3(transpose(inverse(model))) * aNormal;

    // 3. Use the transform matrix to project the vertex position to clip space
    //    and store it in gl_Position.
    gl_Position = projection * view * vec4(vs_out.FragPos, 1.0); //* model * vec4(aPosition, 1.0); // the 1.0 is the w value which is necessary for the matrix multiplication.

    vs_out.vertexColour = aVertexColour;
    vs_out.textureCoord = aTextureCoord;
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);

}