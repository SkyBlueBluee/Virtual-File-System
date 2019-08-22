
struct Material
{
  vec3 ambient;
  sampler2D diffuse;
  sampler2D specular;
};

uniform Material material;