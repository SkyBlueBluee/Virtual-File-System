#version 410 core

#pragma include light/light_functions.glsl

in vec3 world_pos;
in vec3 world_normal;

in vec3 normal;
in vec3 FragPos;  
in vec2 UVs;

out vec4 FragColor;

void main()
{
    
    vec3 norm = normalize(normal);
    
    vec3 local = CalcAllLights(norm, FragPos, material, UVs);
                        
    // vec3 final = CalculateFogColor(local, FragPos);
    FragColor = vec4(local,  1.0);
}
