#version 400

//layout (location = 0) in vec3 VertexPosition;
//layout (location = 1) in vec2 TextC;

layout (location = 0) in vec3 aPosition;      // the position variable has attribute position 0
layout (location = 1) in vec3 aNormal;        // not currently implemented
layout (location = 2) in vec3 aVertexColour;  // the colour variable has attribute position 1
layout (location = 3) in vec2 aTextureCoord;  // the texture coordinate variable has attribute position 2

//uniform mat4 transform;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 vUv;
out vec3 vN;
out vec4 v_pos; 

void main()
{
	v_pos = projection * view * model * vec4(aPosition, 1.0); // the 1.0 is the w value which is necessary for the matrix multiplication.
	vUv = aTextureCoord;
    gl_Position = v_pos;

	//v_pos = transform * vec4(VertexPosition, 1.0);
	//vUv = TextC;
	//gl_Position = transform * vec4(VertexPosition, 1.0);
}