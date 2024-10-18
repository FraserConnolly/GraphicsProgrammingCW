#version 400
out vec4 FragColor;
in vec3 v_norm;
in vec4 v_pos; 
in vec3 vertexColour;
in vec2 textureCoord;

uniform vec3 fogColor;

uniform float maxDist; //fog max distance
uniform float minDist; //fog min distance

uniform sampler2D diffuse;

void main() 
{
	float dist = abs( v_pos.z );
	float fogFactor = (maxDist - dist)/(maxDist - minDist);
	fogFactor = clamp( fogFactor, 0.0, 1.0 );
	vec3 lightColor = vec3(0.8,0.8,0.8);
	vec4 texColor = texture(diffuse, textureCoord) * vec4(vertexColour, 1.0) * vec4(fogColor, 1.0);
	vec4 color = mix( texColor , vec4(lightColor, 1.0), 1-fogFactor);

	FragColor = color;
}
