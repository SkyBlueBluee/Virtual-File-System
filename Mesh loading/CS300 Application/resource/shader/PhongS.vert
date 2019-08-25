#version 410 core

layout(location = 0) in vec3  inPosition;
layout(location = 1) in vec3  inNormal;
layout(location = 2) in vec2  inUVs;
layout(location = 3) in ivec4 inBoneIDs;
layout(location = 4) in vec4  inWeights;

uniform mat4 modelToView;
uniform mat4 normalTransform;
uniform mat4  MVP;
uniform mat4  Bones[100];

out vec3 world_pos;
out vec3 world_normal;
out vec3 FragPos;
out vec3 normal;
out vec2 UVs;

void main()
{
    mat4 BoneTransform = Bones[inBoneIDs[0]] * inWeights[0];
    BoneTransform += Bones[inBoneIDs[1]] * inWeights[1];
    BoneTransform += Bones[inBoneIDs[2]] * inWeights[2];
    BoneTransform += Bones[inBoneIDs[3]] * inWeights[3];
    
    
    vec4 vPos = BoneTransform * vec4( inPosition, 1.0 );
    gl_Position = MVP * vPos;
    
    // Output
    FragPos =  vec3(modelToView * vec4(inPosition, 1.0));
    vec3 N = vec3(BoneTransform * vec4(inNormal, 0.0));
    normal = mat3(normalTransform) * N;
    UVs = inUVs;
    
    world_pos = inPosition;
    world_normal = normalize(N);
}
