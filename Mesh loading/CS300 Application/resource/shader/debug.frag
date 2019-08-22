#version 410 core

uniform sampler2D debugTexture;

in vec3 world_pos;

out vec4 fragColor;

void main()
{
  vec2 uv = world_pos.xy/2+0.5;
	fragColor = vec4( vec3(texture(debugTexture, uv)), 1.0);
}
