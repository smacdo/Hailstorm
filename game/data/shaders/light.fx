// light.fx - Functions for calculating light contributions based on their type
// (c) 2013 Scott MacDonald
////////////////////////////////////////////////////////////////////////////////

// The light structure represents a source of light in the rendered scene. It stores information
// required by any of the light types, so not all members will be used for each light source.
struct Light
{
	// Position of the light (ignored for directional lights)
	float3 pos;

	// Direction of the light (ignored for point lights)
	float3 dir;

	// Ambient light emitted by this light source
	float4 ambient;

	// Diffuse light emitted by this light source
	float4 diffuse;

	// Specular light emitted by this light source
	float4 spec;

	// Stores the three attenuation constants in the format (a0, a1, a2). Attenuation constants
	// are onlyu applied to point lights and spotlights.
	float3 att;

	// The exponent used in the spotlight calculation to control the spotlight cone; this value
	// only applies to spotlights.
	float spotPower;

	// The range of the light (ignored for directional lights).
	float range;
};

// The surface info struct stores information at a surface point needed to light that point; that is
// it stores the surface point itself, its normal, and the diffuse and specular material values at
// that point.
struct SurfaceInfo
{
	float3 pos;
	float3 normal;
	float4 diffuse;
	float4 spec;
};

// Outputs the lit surface color given a parallel light source
float3 ParallelLight( SurfaceInfo v, Light L, float3 eyePos )
{
	float3 litColor = float3( 0.0f, 0.0f, 0.0f );

	// The light vector aims opposite the direction the light rays travel.
	float3 lightVec = -L.dir;

	// Add the ambient term
	litColor += v.diffuse * L.ambient;

	// Add diffuse and specular terms, provided the surface is in the line of sight of the light.
	float diffuseFactor = dot( lightVec, v.normal );

	[branch]	// can't we just remove this and do a zero contribution?
	if ( diffuseFactor > 0.0f )
	{
		float specPower  = max( v.spec.a, 1.0f );
		float3 toEye     = normalize( eyePos - v.pos );
		float3 R         = reflect( -lightVec, v.normal );
		float specFactor = pow( max( dot( R, toEye ), 0.0f ), specPower );

		// diffuse and specular terms
		litColor += diffuseFactor * v.diffuse * L.diffuse;
		litColor += specFactor * v.spec * L.spec;
	}

	return litColor;
}

// Output the lit surface color given a point light source
float3 PointLight( SurfaceInfo v, Light L, float eyePos )
{
	float3 litColor = float3( 0.0f, 0.0f, 0.0f );
	
	// The vector from the surface to the light
	float3 lightVec = L.pos - v.pos;

	// The distance from surface to the light
	float d = length( lightVec );

	if ( d > L.range )
	{
		return float3( 0.0f, 0.0f, 0.0f );
	}

	// Normalize the light vector.
	lightVec /= d;

	// Add the ambient light term.
	litColor += v.diffuse * L.ambient;

	// Add the diffuse and specular term, provided the surface is in the line of sight of the light.
	float diffuseFactor = dot( lightVec, v.normal );

	[branch]
	if ( diffuseFactor > 0.0f )
	{
		float specPower    = max( v.spec.a, 1.0f );
		float3 toEye       = normalize( eyePos - v.pos );
		float3 R           = reflect( -lightVec, v.normal );
		float specFactor   = pow( max( dot( R, toEye ), 0.0f ), specPower );

		// diffuse and specular terms
		litColor += diffuseFactor * v.diffuse * L.diffuse;
		litColor += specFactor * v.spec * L.spec;
	}

	// attentuate and return
	return litColor / dot( L.att, float3( 1.0f, d, d * d ) );
}

// Output the lit surface color given a spoitlight light source
float3 Spotlight( SurfaceInfo v, Light L, float3 eyePos )
{
	float3 litColor = PointLight( v, L, eyePos );

	// The vector from the surface to the light.
	float3 lightVec = normalize( L.pos - v.pos );
	float s = pow( max( dot( -lightVec, L.dir ), 0.0f ), L.spotPower );

	// Scale color by the spotlight factor.
	return litColor * s;
}