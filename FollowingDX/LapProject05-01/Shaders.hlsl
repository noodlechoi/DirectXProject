float4 VSMain(uint vertexID : SV_VertexID ) : SV_POSITION
{
    float4 output;
    
    // 프리미티브를 구성하는 정점의 인덱스(SV_VertexID)에 따라 정점을 반환한다.
    // 정점의 위치 좌표는 변환이 된 좌표(SV_POSITION)이다. 투영좌표계 좌표이다.
    if (vertexID == 0)
        output = float4(0.0f, 0.5f, 0.5f, 1.0f);
    else if (vertexID == 1) 
        output = float4(0.5f, -0.5f, 0.5f, 1.0f);
    else if (vertexID == 2) 
        output = float4(-0.5f, -0.5f, 0.5f, 1.0f);
            
    return output;
}

float4 PSMain(float4 input : SV_POSITION) : SV_TARGET
{
    return float4(1.0f, 1.0f, 0.0f, 1.0f);
}