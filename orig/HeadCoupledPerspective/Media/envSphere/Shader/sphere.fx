// ********************************************************************************************************

struct VS_INPUT
{
    float3 Pos      : POSITION; // Projected position
    float3 Norm     : NORMAL;
    float2 Uv       : TEXCOORD0;
};

struct PS_INPUT
{
    float4 Pos      : SV_POSITION;
    float2 Uv       : TEXCOORD0;
};

// ********************************************************************************************************

Texture2D    TEXTURE0 : register( t0 );
SamplerState SAMPLER0 : register( s0 );

// ********************************************************************************************************
cbuffer cbPerModelValues
{
    row_major float4x4 World : WORLD;
    row_major float4x4 WorldViewProjection : WORLDVIEWPROJECTION;
    row_major float4x4 InverseWorld : INVERSEWORLD;
                float4   LightDirection;
                float4   EyePosition;
    row_major float4x4 LightWorldViewProjection;
};

// ********************************************************************************************************
// TODO: Note: nothing sets these values yet
cbuffer cbPerFrameValues
{
    row_major float4x4  View;
    row_major float4x4  Projection;
};

// ********************************************************************************************************
PS_INPUT VSMain( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos  = mul( float4( input.Pos, 1.0f), WorldViewProjection );
    output.Uv   = float2(input.Uv.x, input.Uv.y);
    return output;
}

// ********************************************************************************************************
float4 PSMain( PS_INPUT input ) : SV_Target // Just decal, no lighting
{
    float4 diffuseTexture = TEXTURE0.Sample( SAMPLER0, input.Uv );
    return diffuseTexture;
}
