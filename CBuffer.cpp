#pragma once
#include "CBuffer.h"



CBuffer::CBuffer(int numRealVertex, SimpleVertex* VertexArray)
{
	ZeroMemory(&m_Desc, sizeof(m_Desc));
	m_Desc.Usage = D3D11_USAGE_DEFAULT;
	m_Desc.ByteWidth = sizeof(SimpleVertex) * numRealVertex;
	m_Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_Desc.CPUAccessFlags = 0;
	ZeroMemory(&m_InitData, sizeof(m_InitData));
	m_InitData.pSysMem = VertexArray;

	m_stride = sizeof(SimpleVertex);
	m_offset = 0;
	m_Buffer = NULL;
}

CBuffer::CBuffer(D3D11_USAGE Usage, SimpleVertex* VertexArray, int numRealVertex, D3D11_BIND_FLAG BindFlags, int CPUAccesFlags, UINT Offset)
{
	ZeroMemory(&m_Desc, sizeof(m_Desc));
	m_Desc.Usage = Usage;
	m_Desc.ByteWidth = sizeof(SimpleVertex) * numRealVertex;
	m_Desc.BindFlags = BindFlags;
	m_Desc.CPUAccessFlags = CPUAccesFlags;
	ZeroMemory(&m_InitData, sizeof(m_InitData));
	m_InitData.pSysMem = VertexArray;

	m_stride = sizeof(SimpleVertex);
	m_offset = Offset;
	m_Buffer = NULL;
}

CBuffer::CBuffer(int numIndex, WORD* IndexArray)
{
	ZeroMemory(&m_Desc, sizeof(m_Desc));
	m_Desc.Usage = D3D11_USAGE_DEFAULT;
	m_Desc.ByteWidth = sizeof(WORD) * numIndex;
	m_Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_Desc.CPUAccessFlags = 0;
	ZeroMemory(&m_InitData, sizeof(m_InitData));
	m_InitData.pSysMem = IndexArray;
	m_Buffer = NULL;
}

CBuffer::CBuffer(D3D11_USAGE Usage, WORD* IndexArray, int numIndex, D3D11_BIND_FLAG BindFlags, int CPUAccesFlags)
{
	ZeroMemory(&m_Desc, sizeof(m_Desc));
	m_Desc.Usage = Usage;
	m_Desc.ByteWidth = sizeof(WORD) * numIndex;
	m_Desc.BindFlags = BindFlags;
	m_Desc.CPUAccessFlags = CPUAccesFlags;
	ZeroMemory(&m_InitData, sizeof(m_InitData));
	m_InitData.pSysMem = IndexArray;
	m_Buffer = NULL;
}

CBuffer::CBuffer(int Size)
{
	ZeroMemory(&m_Desc, sizeof(m_Desc));
	m_Desc.Usage = D3D11_USAGE_DEFAULT;
	m_Desc.ByteWidth = Size;
	m_Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	m_Desc.CPUAccessFlags = 0;

	m_Buffer = NULL;
}

CBuffer::CBuffer(D3D11_USAGE Usage, int Size, D3D11_BIND_FLAG BindFlags, int CPUAccesFlags)
{
	ZeroMemory(&m_Desc, sizeof(m_Desc));
	m_Desc.Usage = Usage;
	m_Desc.ByteWidth = Size;
	m_Desc.BindFlags = BindFlags;
	m_Desc.CPUAccessFlags = CPUAccesFlags;
	m_Buffer = NULL;
}


CBuffer::~CBuffer()
{
}


D3D11_BUFFER_DESC CBuffer::GetDescription()		{		return m_Desc;		}
D3D11_BUFFER_DESC* CBuffer::GetDescriptionPtr()		{ return &m_Desc; }
D3D11_SUBRESOURCE_DATA CBuffer::GetInitData()	{		return m_InitData;	}
ID3D11Buffer* CBuffer::GetBuffer()				{		return m_Buffer;	}
UINT CBuffer::GetStride()						{		return m_stride;	}
UINT CBuffer::GetOffset()						{		return m_offset;	}
