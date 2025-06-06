#include "stdafx.h"

ID3D12Resource* CreateBufferResource(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, void* data, UINT bytes, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_STATES resourceStates, ID3D12Resource** uploadBuffer)
{
	ID3D12Resource* buffer{};

	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = heapType;
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProperties.CreationNodeMask = 1;
	heapProperties.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Alignment = 0;
	resourceDesc.Width = bytes;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	D3D12_RESOURCE_STATES resourceInitialState = D3D12_RESOURCE_STATE_COPY_DEST;
	if (heapType == D3D12_HEAP_TYPE_UPLOAD) {
		resourceInitialState = D3D12_RESOURCE_STATE_GENERIC_READ;
	} else if (heapType == D3D12_HEAP_TYPE_READBACK) {
		resourceInitialState = D3D12_RESOURCE_STATE_COPY_DEST;
	}

	ThrowIfFailed(device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		resourceInitialState,
		nullptr,
		__uuidof(ID3D12Resource),
		(void**)&buffer
	));

	if (data) {
		switch (heapType) {
		case D3D12_HEAP_TYPE_DEFAULT:
		{
			// ���ε� ���� ����
			heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
			device->CreateCommittedResource(
				&heapProperties,
				D3D12_HEAP_FLAG_NONE,
				&resourceDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				__uuidof(ID3D12Resource),
				(void**)uploadBuffer
			);

			// ���ε� ���۸� �����Ͽ� �ʱ�ȭ �����͸� ���ۿ� ����
			D3D12_RANGE readRange{ 0, 0 };
			UINT8* bufferDataBegin{};
			(*uploadBuffer)->Map(0, &readRange, (void**)(&bufferDataBegin));
			memcpy(bufferDataBegin, data, bytes);
			(*uploadBuffer)->Unmap(0, nullptr);

			// ���ε� ���� ������ ����Ʈ ���ۿ� ����
			commandList->CopyResource(buffer, *uploadBuffer);

			D3D12_RESOURCE_BARRIER resourceBarrier{};
			resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			resourceBarrier.Transition.pResource = buffer;
			resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
			resourceBarrier.Transition.StateAfter = resourceStates;
			resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			commandList->ResourceBarrier(1, &resourceBarrier);
		}
			break;
		case D3D12_HEAP_TYPE_UPLOAD:
		{
			// ���ε� ���۸� �����Ͽ� �ʱ�ȭ �����͸� ���ۿ� ����
			D3D12_RANGE readRange{ 0, 0 };
			UINT8* bufferDataBegin{};
			buffer->Map(0, &readRange, (void**)(&bufferDataBegin));
			memcpy(bufferDataBegin, data, bytes);
			buffer->Unmap(0, nullptr);
		}
			break;
		default:
			break;
		}
	}

	return buffer;
}
