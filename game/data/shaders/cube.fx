// cube.fx - Shader for a cube
// (c) 20102 Scott MacDonald
////////////////////////////////////////////////////////////////////////////////
cbuffer cbPerObject
{
	float4x4 gWVP;
};

void VS( float3 pos        : POSITION,
         float4 color      : COLOR,
		 out float4 oPos   : SV_POSITION,
		 out float4 oColor : COLOR )
{
		// Transform vertex to homogenous clip space
		oPos = mul( float4( pos, 1.0f ), gWVP );
		
		// No need to do anything with color, pass it to the pixel shader
		oColor = color;
}

float4 PS( float4 pos   : SV_POSITION,
           float4 color : COLOR ) : SV_Target
{
	return color;
}

technique10 DefaultCubeTechnique
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS() ) );
	}
}