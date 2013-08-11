#version 140

in vec3 position;
in vec3 normal;
in vec2 texel;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 vertTexel;
out vec4 worldNormal;
out vec4 worldPos;

void main()
{
	worldPos = model * vec4(position, 1.0);
	gl_Position = projection * view * worldPos;
	worldNormal = model * vec4(normal, 0.0);
	vertTexel = texel;
}