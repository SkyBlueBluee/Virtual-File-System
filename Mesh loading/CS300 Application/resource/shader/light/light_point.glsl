#pragma include light/material.glsl

#define MAX_POINT_LIGHTS 16
struct PointLight
{
  vec3 position;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

uniform int light_point_numLights;
uniform PointLight light_point_Lights[MAX_POINT_LIGHTS];

vec3 CalculatePointLight(PointLight light, vec3 N, vec3 frag_pos, Material mat, vec2 textCoords)
{
  vec3 result = vec3(0.0);
  
  vec3 viewDir = normalize(-frag_pos);
  vec3 lightDir = light.position - frag_pos;
  float dist = length(lightDir);
  lightDir = normalize(lightDir);
  
  float attenuation = 1.0 / ( constant + 
                              linear * dist + 
                              quadratic * dist * dist);
  attenuation = min(attenuation, 1.0);
  
  // Ambient 
  vec3 ambientVec = light.ambient * mat.ambient;
  result += ambientVec;
  
  // Diffuse
  float diff = max(dot(N , lightDir), 0);
  vec3 diffuseVec = light.diffuse
                    * vec3(0.5, 0.5, 0.5)  
                    * diff;
  result += diffuseVec;
  
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

  result += specularVec;
  return result * attenuation;
}

vec3 CalculatePointLight_NoUVs(PointLight light, vec3 N, vec3 frag_pos, Material mat)
{
  vec3 result = vec3(0.0);
  
  vec3 viewDir = normalize(-frag_pos);
  vec3 lightDir = light.position - frag_pos;
  float dist = length(lightDir);
  lightDir /= dist;
  
  float attenuation = 1.0 / ( constant + 
                              linear * dist + 
                              quadratic * dist * dist);
  attenuation = min(attenuation, 1.0);
  
  // Ambient 
  vec3 ambientVec = light.ambient * mat.ambient;
  result += ambientVec;
  
  // Diffuse
  float diff = max(dot(N , lightDir), 0);
  vec3 diffuseVec = light.diffuse
                    * diff;
  result += diffuseVec;
  
  // Specular
  float shininess = 1;
  
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
  
  float spec = pow(dot_prod_res, shininess) * 0.5;
  
  vec3 specularVec = light.specular 
                     * spec;

  result += specularVec;
  return result * attenuation;
}