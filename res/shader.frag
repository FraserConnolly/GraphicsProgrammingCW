#version 330 core

in vec3 vertexColour;
in vec2 textureCoord;

uniform sampler2D diffuse;

out vec4 fragmentColour;  

void main()
{
    fragmentColour = texture(diffuse, textureCoord) * vec4(vertexColour, 1.0);
}