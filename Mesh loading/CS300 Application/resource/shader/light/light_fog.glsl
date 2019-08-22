uniform vec3 fog_color;
uniform float z_near;
uniform float z_far;

vec3 CalculateFogColor(vec3 local_col, vec3 viewVec)
{
  float S = max((z_far - length(viewVec)) / (z_far - z_near), 0.0);
  
  return local_col * S + (1 - S) * fog_color;
}