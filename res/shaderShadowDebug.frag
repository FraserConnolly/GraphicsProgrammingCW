#version 330 core
out vec4 FragColor;
  
in vec3 vertexColour; // output a colour to the fragment shader
in vec2 textureCoord;

uniform sampler2D depthMap;

void main()
{             
    float depthValue = texture(depthMap, textureCoord).r;
    FragColor = vec4(vec3(depthValue), 1.0);
}  