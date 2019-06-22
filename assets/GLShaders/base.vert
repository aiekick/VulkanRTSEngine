#version 420

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uvs;
layout(location = 2) in vec3 normal;

out vec3 normalOut;
out vec2 uvsOut;

layout (std140, binding = 0) uniform ObjectMatricesAdapter
{
	mat4 Model;
	mat4 ModelView;
	mat4 MVP;
};

void main() 
{
    gl_Position = mvp * vec4(position, 1.0);
    normalOut = normalize(Model * vec4(normal,0)).xyz;
    uvsOut = uvs;
}
