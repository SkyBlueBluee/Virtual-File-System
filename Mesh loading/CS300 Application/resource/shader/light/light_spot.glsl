#pragma include light/material.glsl

#define MAX_SPOT_LIGHTS 16
struct SpotLight
{
  vec3 position;
  vec3 direction;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  
  float falloff;
  float inner_angle;
  float outer_angle;
};

uniform int light_spot_numLights;
uniform SpotLight light_spot_Lights[MAX_SPOT_LIGHTS];

vec3 CalculateSpotLight(SpotLight light, vec3 N, vec3 frag_pos, Material mat, vec2 textCoords)
{
  vec3 result = vec3(0.0);
  
  vec3 viewDir = normalize(-frag_pos);
  float dist = length(light.direction);
  vec3 lightDir = light.direction / dist;
  vec3 frag_to_light = normalize(light.position - frag_pos);
  
  float theta = dot(frag_to_light, lightDir);
  float epsil = light.inner_angle - light.outer_angle;
  float intensity = clamp((theta - light.outer_angle) / epsil, 0.0, 1.0);
  intensity = pow(intensity, light.falloff);
  
  float attenuation = 1.0 / ( constant + 
                              linear * dist + 
                              quadratic * dist * dist);
  attenuation = min(attenuation, 1.0);
  
  // Ambient (Do not apply spotlight intensity)
  vec3 ambientVec = light.ambient * mat.ambient;
  result += ambientVec;
  
  // Diffuse
  float diff = max(dot(N , lightDir), 0);
  vec3 diffuseVec = light.diffuse
                    * vec3(0.5, 0.5, 0.5) 
                    * diff;
  result += diffuseVec * intensity;
  
  // Specular
  vec3 mat_spec = vec3(0.2, 0.2, 0.2);  
  float shininess = pow(mat_spec.x, 2);
  
  vec3 reflection = vec3(0);
  float dot_prod_res = 0.0;
  // if(diff > 0)
  {
    if(use_blinn)
    {
      reflection = normalize(viewDir + lightDir);
      dot_prod_res = max(dot(N, reflection), 0);
      shininess *= 4;
    }
    else
    {
      reflection = 2 * diff * N - lightDir;
      dot_prod_res = max(dot(viewDir, reflection), 0);
    }
  }
  
  float spec = diff * pow(dot_prod_res, shininess);
  
  vec3 specularVec = light.specular 
                     * mat_spec
                     * spec;

  result += specularVec * intensity;
  return result * attenuation;
}


vec3 CalculateSpotLight_NoUVs(SpotLight light, vec3 N, vec3 frag_pos, Material mat)
{
  vec3 result = vec3(0.0);
  
  vec3 viewDir = normalize(-frag_pos);
  float dist = length(light.direction);
  vec3 lightDir = light.direction / dist;
  vec3 frag_to_light = normalize(light.position - frag_pos);
  
  float theta = dot(frag_to_light, lightDir);
  float epsil = light.inner_angle - light.outer_angle;
  float intensity = clamp((theta - light.outer_angle) / epsil, 0.0, 1.0);
  intensity = pow(intensity, light.falloff);
  
  float attenuation = 1.0 / ( constant + 
                              linear * dist + 
                              quadratic * dist * dist);
  attenuation = min(attenuation, 1.0);
  
  // Ambient (Do not apply spotlight intensity)
  vec3 ambientVec = light.ambient * mat.ambient;
  result += ambientVec;
  
  // Diffuse
  float diff = max(dot(N , lightDir), 0);
  vec3 diffuseVec = light.diffuse
                    * diff;
  result += diffuseVec * intensity;
  
  // Specular
  float shininess = 32;
  
  vec3 reflection = vec3(0);
  float dot_prod_res = 0.0;
  if(diff > 0)
  {
    if(use_blinn)
    {
      reflection = normalize(viewDir + lightDir);
      dot_prod_res = max(dot(N, reflection), 0);
      shininess *= 4;
    }
    else
    {
      reflection = 2 * diff * N - lightDir;
      dot_prod_res = max(dot(viewDir, reflection), 0);
    }
  }
  
  float spec = pow(dot_prod_res, shininess);
  
  vec3 specularVec = light.specular 
                     * spec;

  result += specularVec * intensity;
  return result * attenuation;
}