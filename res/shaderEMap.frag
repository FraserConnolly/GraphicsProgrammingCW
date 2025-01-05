#version 420 core

// Inputs from vertex shader
in  Vertex_DATA{
    vec2 textureCoord;
    vec3 Normal;
    vec3 Position;
} vertInData;

uniform vec3 cameraPos;
uniform float reflectivity = 0.2f;
uniform samplerCube skybox;
layout (binding = 1) uniform sampler2D diffuse;

// Output color
out vec4 fragmentColour;

void main ( )
{
    // Calculate the vector from the camera to the fragment.
    vec3 I = vertInData.Position - cameraPos;
    
    // Calculate the reflection vector using the vertex normal as the angle of incident.
    vec3 R = reflect(I, normalize(vertInData.Normal)); //calculate the reflection direction for the incident vector

    // Use this vector to read the rgb value from the samplerCube (our skybox)
    vec4 enviornmentColour = vec4(texture(skybox, R).rgb, 1.0);

    // Now read the rgb values from our texture2D and mix the result with “Colour”. 
    // You will need to use “glGetUniformLocation” application side, we covered this with bump mapping.
    vec4 texColour = texture(diffuse, vertInData.textureCoord);
    
    fragmentColour = mix(enviornmentColour, texColour, reflectivity);
}
