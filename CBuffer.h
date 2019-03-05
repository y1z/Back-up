#pragma once
#include "Header.h"

class CBuffer
{
public:
	// Normal Constructor (DEcoration)
	CBuffer() = default;
	// Default Vertex Description
	CBuffer(int numRealVertex, SimpleVertex* VertexArray);
	// Complete Vertex Description
	CBuffer(D3D11_USAGE Usage, SimpleVertex* VertexArray, int numRealVertex, D3D11_BIND_FLAG BindFlags, int CPUAccesFlags, UINT Offset);
	
	// Default Index Description
	CBuffer(int numIndex, WORD* IndexArray);
	// Complete Index Description
	CBuffer(D3D11_USAGE Usage, WORD* IndexArray, int numIndex, D3D11_BIND_FLAG BindFlags, int CPUAccesFlags);

	// Complete Constant Constructor
	CBuffer(D3D11_USAGE Usage, int Size, D3D11_BIND_FLAG BindFlags, int CPUAccesFlags);
	// Default Constant Buffer
	CBuffer(int Size);

	~CBuffer();
public:// variables 
	D3D11_BUFFER_DESC m_Desc;
	D3D11_SUBRESOURCE_DATA m_InitData;
	ID3D11Buffer* m_Buffer;
	UINT m_stride ;
	UINT m_offset ;
public:
	D3D11_BUFFER_DESC GetDescription();
	D3D11_BUFFER_DESC* GetDescriptionPtr();
	D3D11_SUBRESOURCE_DATA GetInitData();
	ID3D11Buffer* GetBuffer();
	UINT GetStride();
	UINT GetOffset();
};

