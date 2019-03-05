#pragma once
#include "CMesh.h"


#include "Bin/assimp/cimport.h"
#include "Bin/assimp/postprocess.h"
#include "Bin/assimp/scene.h"

CMesh::CMesh(D3D11_USAGE Usage, SimpleVertex* VertexArray, WORD* IndexArray, int numRealVertex, int numIndex, D3D11_BIND_FLAG BindFlags, int CPUAccesFlags, int Offset)
{
	// Vertex buffer
	VertexBuffer = CBuffer(Usage, VertexArray, numRealVertex, BindFlags, CPUAccesFlags, Offset);
	IndexBuffer = CBuffer(Usage, IndexArray, numIndex, BindFlags, CPUAccesFlags);
}

CMesh::CMesh(int numRealVertex, int numIndex, SimpleVertex* VertexArray, WORD* IndexArray)
{

	VertexBuffer = CBuffer(numRealVertex, VertexArray);
	IndexBuffer = CBuffer(numIndex, IndexArray);
	ChangeEveryFrame = CBuffer(sizeof(CBChangesEveryFrame));
}

CMesh::~CMesh()
{
}
