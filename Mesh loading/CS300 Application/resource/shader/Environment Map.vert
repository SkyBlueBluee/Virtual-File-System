#version 410 core

layout(location = 0) in vec3  inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUVs;

uniform vec3 cameraPos;
uniform mat4 modelToView;
uniform mat4 normalTransform;
uniform mat4  MVP;

out vec3 world_pos;
out vec3 world_normal;
out vec3 camera_pos;
out vec3 FragPos;
out vec3 normal;
out vec2 UVs;

void main()
{
    vec4 vPos = vec4( inPosition, 1.0 );
    gl_Position = MVP * vPos;
    
    // Output
    FragPos =  vec3(modelToView * vec4(inPosition, 1.0));
    normal = vec3(mat3(normalTransform) * inNormal);
    UVs = inUVs;
    
    world_pos = inPosition;
    world_normal = normalize(inNormal);
    camera_pos = cameraPos;
}
