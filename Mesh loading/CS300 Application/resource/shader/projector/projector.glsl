uniform bool to_calc_uv;

uniform int input_type;
uniform int uv_type;

uniform vec3 min_bounding_box;
uniform vec3 max_bounding_box;

vec3 GetScale()
{
  vec3 scale = max_bounding_box - min_bounding_box;
	if (scale.x == 0)
		scale.x = 1.0;

	if (scale.y == 0)
		scale.y = 1.0;

	if (scale.z == 0)
		scale.z = 1.0;

	return scale;
}

vec3 GetCentroid()
{
  return min_bounding_box + max_bounding_box * 0.5;
}

vec3 NormalizeInput(vec3 projector_input)
{
  vec3 delta = GetScale();
  vec3 center = GetCentroid();
  vec3 normalized_input = projector_input;
  
  // Normalize position
  if(input_type == 0)
    normalized_input = ((projector_input - center) + 1.0) / delta;
  
  return normalized_input;
}

vec2 CalcUV_Planar(vec3 projector_input)
{
  vec2 uv = vec2(0);
  vec3 normalized_input = NormalizeInput(projector_input);
  
  uv.x = normalized_input.x;
  uv.y = normalized_input.y;
  
  return uv;
}

vec2 CalcUV_Cylindrical(vec3 projector_input)
{
  vec2 uv = vec2(0);
  
  float theta = 0.0;
	float z = 0.0;

	theta = degrees(atan(projector_input.y, projector_input.x));
	z = projector_input.z;
  
	uv.x = theta / 360.0;
  uv.y = z;
  
  return uv;
}

vec2 CalcUV_Spherical(vec3 projector_input)
{
  vec2 uv = vec2(0);
  
  float theta = 0.0;
	float z = 0.0;
  float phi = 0.0;
	theta = degrees(atan(projector_input.y, projector_input.x));
	z = projector_input.z;
  phi = degrees(acos(z / length(projector_input)));
  
	uv.x = theta / 360.0;
  uv.y = 1.0 - (phi / 180.0);
  
  return uv;
}

bool floatCompare(float a, float b)
{
  return abs(a - b) < 0.001;
}

vec2 CalcUV_Cube(vec3 projector_input)
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
		}
		else
		{
			uv.x = -projector_input.z;
			uv.y = projector_input.y;
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
		}
		else
		{
			uv.x = projector_input.x;
			uv.y = -projector_input.z;
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
		}
		else
		{
			uv.x = projector_input.x;
			uv.y = projector_input.y;
		}

		maxAxis = absZ;
	}

	// Convert range from -1 to 1 to 0 to 1
	uv.x = 0.5 * (uv.x / maxAxis + 1.0);
	uv.y = 0.5 * (uv.y / maxAxis + 1.0);

	return uv;
}

vec2 CalcUV(vec3 projector_input)
{
  if(uv_type == 0)
  {
    return CalcUV_Planar(projector_input);
  }
  
  else if(uv_type == 1)
  {
    return CalcUV_Cylindrical(projector_input);
  }
  
  else if(uv_type == 2)
  {
    return CalcUV_Spherical(projector_input);
  }
  
  else if(uv_type == 3)
  {
    return CalcUV_Cube(projector_input);
  }
}