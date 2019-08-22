// Global ambient and others
uniform vec3 global_ambient;
uniform vec3 material_emissive;

// Use Blinn
uniform bool use_blinn;
uniform bool no_uvs;

// Attenuation constants
uniform float constant;
uniform float linear;
uniform float quadratic;

#pragma include projector/projector.glsl
#pragma include light/light_dir.glsl
#pragma include light/light_point.glsl
#pragma include light/light_spot.glsl
#pragma include light/light_fog.glsl

vec3 CalcAllLights(vec3 N, vec3 frag_pos, Material mat, vec2 textCoords)
{
    vec3 total_dir = vec3(0.0);
    vec3 total_point = vec3(0.0);
    vec3 total_spot = vec3(0.0);
    
    for(int i = 0; i < light_dir_numLights; ++i)
    {
      total_dir += CalculateDirLight(light_dir_Lights[i], N, frag_pos, mat, textCoords);
    }
    
    for(int i = 0; i < light_point_numLights; ++i)
    {
      total_point += CalculatePointLight(light_point_Lights[i], N, frag_pos, mat, textCoords);
    }
    
    for(int i = 0; i < light_spot_numLights; ++i)
    {
      total_spot += CalculateSpotLight(light_spot_Lights[i], N, frag_pos, mat, textCoords);
    }
    
    // Calculate global ambient
    vec3 A = global_ambient * mat.ambient;
    vec3 local = material_emissive + A + total_dir + total_point + total_spot;
    return local;
}

vec3 CalcAllLights_NoUVs(vec3 N, vec3 frag_pos, Material mat)
{
    vec3 total_dir = vec3(0.0);
    vec3 total_point = vec3(0.0);
    vec3 total_spot = vec3(0.0);
    
    for(int i = 0; i < light_dir_numLights; ++i)
    {
      total_dir += CalculateDirLight_NoUVs(light_dir_Lights[i], N, frag_pos, mat);
    }
    
    for(int i = 0; i < light_point_numLights; ++i)
    {
      total_point += CalculatePointLight_NoUVs(light_point_Lights[i], N, frag_pos, mat);
    }
    
    for(int i = 0; i < light_spot_numLights; ++i)
    {
      total_spot += CalculateSpotLight_NoUVs(light_spot_Lights[i], N, frag_pos, mat);
    }
    
    // Calculate global ambient
    vec3 ambient = global_ambient * mat.ambient;
    vec3 local = material_emissive + ambient + total_dir + total_point + total_spot;
    return local;
}