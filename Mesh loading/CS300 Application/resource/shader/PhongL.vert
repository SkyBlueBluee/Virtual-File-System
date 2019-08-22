#version 410 core

#pragma include light/light_functions.glsl
#pragma include projector/projector.glsl

layout(location = 0) in vec3  inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUVs;

uniform mat4 modelToView;
uniform mat4 normalTransform;
uniform mat4  MVP;

out vec3 outColor;

void main()
{
  vec3 FragPos =  vec3(modelToView * vec4(inPosition, 1.0));
  vec3 normal = vec3(mat3(normalTransform) * inNormal);
  
  vec3 norm = normalize(normal);
  
  vec2 lightUVs = inUVs;
  if(to_calc_uv)
  {
    lightUVs = CalcUV(input_type == 0 ? inPosition : normalize(inNormal));
  }
  
  vec3 local = no_uvs ? CalcAllLights_NoUVs(norm, FragPos, material) 
                      : CalcAllLights(norm, FragPos, material, lightUVs);
  vec3 final = CalculateFogColor(local, FragPos);
  
  outColor = local;
  vec4 vPos = vec4( inPosition, 1.0 );
  gl_Position = MVP * vPos;
}
