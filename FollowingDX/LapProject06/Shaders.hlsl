float4 VSMain(uint vertexID : SV_VertexID ) : SV_POSITION
{
    float4 output;
    
    // ������Ƽ�긦 �����ϴ� ������ �ε���(SV_VertexID)�� ���� ������ ��ȯ�Ѵ�.
    // ������ ��ġ ��ǥ�� ��ȯ�� �� ��ǥ(SV_POSITION)�̴�. ������ǥ�� ��ǥ�̴�.
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