#version 330 core

// Inputs from vertex shader
in VS_OUT {
    vec3 FragPos;               // Fragment position in world space
    vec3 Normal;                // Interpolated and transformed normal
    vec3 vertexColour;  
    vec2 textureCoord;
    vec4 FragPosLightSpace;
} fs_in;

// Uniforms
uniform vec3 lightPos;      // Light position in world space
uniform vec3 lightColor;    // Color of the light
uniform sampler2D diffuse;
uniform sampler2D shadowMap;

// Output color
out vec4 fragmentColour;

// from https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
float ShadowCalculation(vec4 fragPosLightSpace, float bias)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // check whether current frag pos is in shadow
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

    return shadow;
}


void main()
{
    vec3 color = texture(diffuse, fs_in.textureCoord).rgb;
    vec3 normal = normalize(fs_in.Normal);
    
    // ambient
    vec3 ambient = 0.15 * lightColor;
    
    // diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // specular (not implemented)
    // vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    // float spec = 0.0;
    // vec3 halfwayDir = normalize(lightDir + viewDir);  
    // spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    // vec3 specular = spec * lightColor;    

    // calculate shadow bias
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005); 

    // calculate shadow
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace, bias);       
    vec3 lighting = (ambient + (1.0 - shadow) * diffuse) * color;    
    // vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    // with specular
    
    fragmentColour = vec4(lighting, 1.0);

}
