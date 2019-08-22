#version 410 core

#pragma include light/light_functions.glsl

uniform sampler2D skybox[6];
uniform int env_mode;
uniform float refraction_ratio;
uniform float chromatic_delta;
uniform float phong_interpolant;
uniform bool addPhong;

in vec3 world_pos;
in vec3 world_normal;
in vec3 camera_pos;

in vec3 normal;
in vec3 FragPos;
in vec2 UVs;

out vec4 FragColor;

vec2 CalcUV_Cube_Axis(vec3 projector_input, inout int axis)
{
  vec2 uv = vec2(0);

  float absX = abs(projector_input.x);
	float absY = abs(projector_input.y);
	float absZ = abs(projector_input.z);

	float maxAxis;
	// X AXIS
	if ((absX > absY || floatCompare(absX, absY)) && (absX >= absZ || floatCompare(absX, absZ)))
	{
		if (projector_input.x < 0)
		{
			uv.x = projector_input.z;
			uv.y = projector_input.y;
      
      axis = 0;
		}
		else
		{
			uv.x = -projector_input.z;
			uv.y = projector_input.y;
      axis = 1;
		}

		maxAxis = absX;
	}
	// Y AXIS
	else if ((absY > absX || floatCompare(absY, absX)) && (absY >= absZ || floatCompare(absY, absZ)))
	{
		if (projector_input.y < 0)
		{
			uv.x = projector_input.x;
			uv.y = projector_input.z;
      axis = 2;
		}
		else
		{
			uv.x = projector_input.x;
			uv.y = -projector_input.z;
      axis = 3;
		}

		maxAxis = absY;
	}
	// Z AXIS
	else if ((absZ > absX || floatCompare(absZ, absX)) && (absZ >= absY || floatCompare(absZ, absY)))
	{
		if (projector_input.z < 0)
		{
			uv.x = -projector_input.x;
			uv.y = projector_input.y;
      axis = 4;
		}
		else
		{
			uv.x = projector_input.x;
			uv.y = projector_input.y;
      axis = 5;
		}

		maxAxis = absZ;
	}

	// Convert range from -1 to 1 to 0 to 1
	uv.x = 0.5 * (uv.x / maxAxis + 1.0);
	uv.y = 0.5 * (uv.y / maxAxis + 1.0);

	return uv;
}

vec3 my_reflect(vec3 I, vec3 N)
{
    float dot_prod = max(dot(N , I), 0);
    vec3 reflection = vec3(0);
    if(dot_prod > 0)
    {
      reflection = 2 * dot_prod * N - I;
    }
    return reflection;
}

vec3 my_refract(vec3 I, vec3 N, float eta)
{
    float k = 1.0 - eta * eta * (1.0 - dot(N, I) * dot(N, I));
    if (k < 0.0)
      return vec3(0.0);
    else
      return eta * I - (eta * dot(N, I) + sqrt(k)) * N;
}

vec3 fresnel(vec3 reflection_v, vec3 refraction_v, vec3 I, vec3 N, float eta)
{
    const float FresnelPower = 5.0;
    float F = ((1.0-eta) * (1.0-eta)) / ((1.0+eta) * (1.0+eta));
    
    float Ratio = F + (1.0 - F) * pow((1.0 - dot(-I, N)), FresnelPower);
    
    return mix(refraction_v, reflection_v, Ratio);
}

void main()
{
    vec3 norm = normalize(world_normal);
    int sample_axis = -1;
    
    vec3 viewDir = normalize(camera_pos - world_pos);
    
    vec3 uv_input = vec3(0.0);
    
   
    
    // Reflection Only
    if(env_mode == 0)
    {
      uv_input = my_reflect(viewDir, norm);
      vec2 reflect_uv = CalcUV_Cube_Axis(uv_input, sample_axis);
      vec3 local = vec3(texture(skybox[sample_axis], reflect_uv));
    
      FragColor = vec4( local, 1.0);
    }
    // Refraction Only
    else if( env_mode == 1)
    {
      // Refraction
      vec3 refract_v_r = my_refract(-viewDir, norm, refraction_ratio - chromatic_delta);
      vec3 refract_v_g = my_refract(-viewDir, norm, refraction_ratio + chromatic_delta);
      vec3 refract_v_b = my_refract(-viewDir, norm, refraction_ratio + chromatic_delta * 2.0);
      
      vec2 refract_uv_r = CalcUV_Cube_Axis(refract_v_r, sample_axis);
      refract_uv_r.x = 1.0 - refract_uv_r.x;
      float refract_col_r = texture(skybox[sample_axis], refract_uv_r).r;
      
      vec2 refract_uv_g = CalcUV_Cube_Axis(refract_v_g, sample_axis);
      refract_uv_g.x = 1.0 - refract_uv_g.x;
      float refract_col_g = texture(skybox[sample_axis], refract_uv_g).g;
      
      vec2 refract_uv_b = CalcUV_Cube_Axis(refract_v_b, sample_axis);
      refract_uv_b.x = 1.0 - refract_uv_b.x;
      float refract_col_b = texture(skybox[sample_axis], refract_uv_b).b;
      
      vec3 refract_col = vec3(refract_col_r, refract_col_g, refract_col_b);
      FragColor = vec4( refract_col, 1.0);
    }
    // Reflection + Refraction (Fresnel's with Chromatic Abberation)
    else if( env_mode == 2)
    {
      // Reflection
      vec3 reflect_v = my_reflect( viewDir, norm);
      vec2 reflect_uv = CalcUV_Cube_Axis(reflect_v, sample_axis);
      vec3 reflect_col = vec3(texture(skybox[sample_axis], reflect_uv));
      
      // Refraction
      vec3 refract_v_r = my_refract(-viewDir, norm, refraction_ratio - chromatic_delta);
      vec3 refract_v_g = my_refract(-viewDir, norm, refraction_ratio + chromatic_delta);
      vec3 refract_v_b = my_refract(-viewDir, norm, refraction_ratio + chromatic_delta * 2.0);
      
      vec2 refract_uv_r = CalcUV_Cube_Axis(refract_v_r, sample_axis);
      refract_uv_r.x = 1.0 - refract_uv_r.x;
      float refract_col_r = texture(skybox[sample_axis], refract_uv_r).r;
      
      vec2 refract_uv_g = CalcUV_Cube_Axis(refract_v_g, sample_axis);
      refract_uv_g.x = 1.0 - refract_uv_g.x;
      float refract_col_g = texture(skybox[sample_axis], refract_uv_g).g;
      
      vec2 refract_uv_b = CalcUV_Cube_Axis(refract_v_b, sample_axis);
      refract_uv_b.x = 1.0 - refract_uv_b.x;
      float refract_col_b = texture(skybox[sample_axis], refract_uv_b).b;
      
      vec3 refract_col = vec3(refract_col_r, refract_col_g, refract_col_b);
      
      
      FragColor = vec4(fresnel(reflect_col, refract_col, -viewDir, norm, refraction_ratio), 1.0);

      if(addPhong)
      {
        // Calculate Phong
        vec2 lightUVs = CalcUV(world_pos);
        vec3 local = CalcAllLights(norm, FragPos, material, lightUVs);
        vec3 final = CalculateFogColor(local, FragPos);
        FragColor = vec4(mix(vec3(FragColor), final, phong_interpolant), 1.0);
        // FragColor = vec4(phong_interpolant, 0, 0, 1.0);
      }
    }
}
