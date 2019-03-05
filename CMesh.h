#pragma once
#include "Header.h"
#include "CBuffer.h"

class CMesh
{
public:
	CBuffer VertexBuffer;
	CBuffer IndexBuffer;
	CBuffer ChangeEveryFrame;
	CMesh() = default;
	CMesh(D3D11_USAGE Usage, SimpleVertex* VertexArray, WORD* IndexArray, int numRealVertex, int numIndex, D3D11_BIND_FLAG BindFlags, int CPUAccesFlags, int Offset);
	CMesh(int numRealVertex, int numIndex, SimpleVertex* VertexArray, WORD* IndexArray);
	~CMesh();
public:
	CBChangesEveryFrame m_object;
};

