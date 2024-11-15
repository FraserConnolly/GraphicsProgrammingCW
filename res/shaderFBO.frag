#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{ 
    //FragColor = texture(screenTexture, TexCoords);

    vec4 texColor = texture(screenTexture, TexCoords);
    
    // Discard fragments where alpha is 1.0 (fully opaque)
    if (texColor.rgb == vec3(1)) {
        discard;
    }
    
    FragColor = texColor; // Output the color if alpha < 1.0
}