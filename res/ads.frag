#version 330 core

// Inputs from vertex shader
in vec3 FragPos;    // Fragment position in world space
in vec3 Normal;     // Interpolated and transformed normal

// Uniforms
uniform vec3 lightPos;      // Light position in world space
uniform vec3 lightColor;    // Color of the light
uniform vec3 objectColor;   // Base color of the object

// Output color
out vec4 FragColor;

// Pseudo code for main
void main()
{
    // 1. Calculate ambient lighting using a fixed strength factor.

    // 2. Calculate diffuse lighting by finding the angle between the
    //    normal and the direction to the light source (Hint: use dot product and consider the -   ve vector “max” function).

    // 3. Combine ambient and diffuse components to get the final color.
    //    Multiply the combined lighting by the object's base color.

    // 4. Set the final color output using FragColor.
}
