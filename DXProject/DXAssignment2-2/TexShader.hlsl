struct VS_INPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
    float2 tex : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 tex : TEXCOORD;
};

cbuffer gameObjectInfo : register(b0)
{
float4x4 worldMatrix : packoffset(c0);
};

cbuffer CameraInfo : register(b1)
{
    float4x4 viewMatrix : packoffset(c0);
    float4x4 projectionMatrix : packoffset(c4);
};

Texture2D texDiffuse : register(t0);
SamplerState sample: register(s0);

VS_OUTPUT VSMain(VS_INPUT input)
{
    VS_OUTPUT output;
    
    //정점의 위치 벡터는 투영좌표계로 표현되어 있으므로 변환하지 않고 그대로 출력한다. 
    output.position = float4(input.position, 1.0f);
    
    //입력되는 픽셀의 색상(래스터라이저 단계에서 보간하여 얻은 색상)을 그대로 출력한다. 
    output.color = input.color;
    
    output.tex = input.tex;
    
    return (output);
}

float4 PSMain(VS_OUTPUT input) : SV_TARGET
{
    return texDiffuse.Sample(sample, input.tex);
}