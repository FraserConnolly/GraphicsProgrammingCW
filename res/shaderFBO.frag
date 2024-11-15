#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform bool invert;
uniform bool grayScale;
uniform sampler2D screenTexture;

void main()
{ 
    vec4 texColor = texture(screenTexture, TexCoords);
    
    // Discard fragments where the colour value is black
    if (texColor.rgb == vec3(0)) {
        discard;
    }
    
    if ( invert ) 
    {
        // invert the colour
        texColor = vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0);
    }

    if ( grayScale )
    {
        float average = 0.2126 * texColor.r + 0.7152 * texColor.g + 0.0722 * texColor.b;                                                          
        texColor = vec4(average, average, average, 1.0);
    }

    FragColor = texColor;
}