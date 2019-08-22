#version 410 core

uniform mat4  vertexTransform;
layout(location = 0) in vec3  vPosition;


void main()
{
    vec4 vPos = vec4( vPosition.x, vPosition.y, vPosition.z, 1.0f );
    gl_Position = vertexTransform * vPos;
}
