#version 330 core

// Inputs from vertex shader
in vec3 FragPos;    // Fragment position in world space
in vec3 Normal;     // Interpolated and transformed normal
in vec2 textureCoord;
in vec3 vertexColour;


// Uniforms
uniform vec3 lightPos;      // Light position in world space
uniform vec3 lightColor;    // Color of the light
uniform vec3 objectColor;   // Base color of the object
uniform sampler2D diffuse;

// Output color
out vec4 fragmentColour;

void main()
{

    // 1. Calculate ambient lighting using a fixed strength factor.
    vec3 ambient = 0.1 * lightColor;

    // 2. Calculate diffuse lighting by finding the angle between the
    //    normal and the direction to the light source (Hint: use dot product and consider the -   ve vector “max” function).
    vec3 norm = normalize( Normal );
    vec3 vLightDir = normalize( lightPos - FragPos );
    float fDotProduct = max( 0.0, dot( norm, vLightDir ) );
    vec3 vDiffuseColor = lightColor * fDotProduct; // vDiffuseMaterial has been ommited.

    // 3. Combine ambient and diffuse components to get the final color.
    //    Multiply the combined lighting by the object's base color.
    vec3 colour = ( vDiffuseColor + ambient ) * objectColor;

    // 4. Set the final color output using FragColor.
    fragmentColour = texture(diffuse, textureCoord) * vec4(vertexColour, 1.0)  * vec4( colour, 1.0 );
}
