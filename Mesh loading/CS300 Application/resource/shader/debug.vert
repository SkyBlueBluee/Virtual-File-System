#version 410 core

layout(location = 0) in vec3  inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUVs;

uniform mat4  MVP;
uniform mat4  model;
out vec3 world_pos;
void main()
{
	vec4 vPos = vec4( inPosition, 1.0 );
  gl_Position = MVP * vPos;
  
  world_pos = inPosition;//vec3(model * vPos);
}
