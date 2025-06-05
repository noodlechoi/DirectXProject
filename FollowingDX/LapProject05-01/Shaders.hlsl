float4 VSMain(uint vertexID : SV_VertexID ) : SV_POSITION
{
    float4 output = (float4)0;
    
    if (vertexID == 0)
        output = float4(-1.0f, 1.0f, 0.0f, 1.0f);
    else if (vertexID == 1)
        output = float4(1.0f, 1.0f, 0.0f, 1.0f);
    else if (vertexID == 2)
        output = float4(1.0f, -1.0f, 0.0f, 1.0f);
    else if (vertexID == 3)
        output = float4(-1.0f, 1.0f, 0.0f, 1.0f);
    else if (vertexID == 4)
        output = float4(1.0f, -1.0f, 0.0f, 1.0f);
    else if (vertexID == 5)
        output = float4(-1.0f, -1.0f, 0.0f, 1.0f);
    
    return output;
}

#define FRAME_BUFFER_WIDTH 800.0f
#define FRAME_BUFFER_HEIGHT 600.0f

float RegularPolygon(float2 f2NDC, float fSides, float fRadius)
{
    float fAngle = atan(f2NDC.y / f2NDC.x);
    float fSlices = (2.0f * 3.14159f) / fSides;
    // 각 섹션의 반올림에 중심각과 현재 픽셀의 각도 차이를 계산(도형 안에 있는지 확인)
    float fShape = step(cos(floor((fAngle / fSlices) + 0.5f) * fSlices - fAngle) * length(f2NDC), fRadius);
    return (fShape);
}

float4 PSMain(float4 input : SV_POSITION) : SV_TARGET
{
    float4 cColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float2 f2NDC = float2(input.x / FRAME_BUFFER_WIDTH, input.y / FRAME_BUFFER_HEIGHT) - 0.5f; // (0, 1) : (-0.5, 0.5)
    f2NDC.x *= (FRAME_BUFFER_WIDTH / FRAME_BUFFER_HEIGHT);

    cColor.b = RegularPolygon(f2NDC - float2(-0.3f, -0.1f), 8.0f, 0.2f); //4, 6, 8, ...
    cColor.r = RegularPolygon(f2NDC - float2(+0.3f, +0.2f), 4.0f, 0.15f);
    return (cColor);
}