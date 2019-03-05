#include "CManager.h"



CManager::CManager()
{
}


//!does same thing as "CleanupDevice" function 
CManager::~CManager()
{
	if (this->mptr_ImmediateContext) this->mptr_ImmediateContext->ClearState();

	if (this->mptr_SamplerLinear) this->mptr_SamplerLinear->Release();
	if (this->mptr_TextureRV) this->mptr_TextureRV->Release();

	if (this->mptr_CBNeverChanges) this->mptr_CBNeverChanges->Release();
	if (this->mptr_CBChangeOnResize) this->mptr_CBChangeOnResize->Release();
	if (this->mptr_CBChangesEveryFrame) this->mptr_CBChangesEveryFrame->Release();

	for (int i = 0; i < this->m_mesh.size(); i++)
	{
		if (this->m_mesh[i].VertexBuffer.m_Buffer) this->m_mesh[i].VertexBuffer.m_Buffer->Release();
		if (this->m_mesh[i].IndexBuffer.m_Buffer)  this->m_mesh[i].IndexBuffer.m_Buffer->Release();
	}

	if (this->mptr_VertexLayout) this->mptr_VertexLayout->Release();
	if (this->mptr_VertexShader) this->mptr_VertexShader->Release();
	if (this->mptr_PixelShader) this->mptr_PixelShader->Release();
	if (this->mptr_DepthStencil) this->mptr_DepthStencil->Release();
	if (this->mptr_DepthStencilView) this->mptr_DepthStencilView->Release();
	if (this->mptr_RenderTargetView) this->mptr_RenderTargetView->Release();
	if (this->mptr_RenderTargetView2) this->mptr_RenderTargetView2->Release();
	if (this->mptr_SwapChain) this->mptr_SwapChain->Release();
	if (this->mptr_ImmediateContext) this->mptr_ImmediateContext->Release();
	if (this->mptr_Device) this->mptr_Device->Release();
}


void CManager::GetMeshs(const char *file)
{
	const	aiScene * Scene = aiImportFile(file, aiProcessPreset_TargetRealtime_MaxQuality);


	for (int i = 0; i < Scene->mNumMeshes; ++i)
	{
		int NumVert = Scene->mMeshes[i]->mNumVertices;
	
		int NumFaces = Scene->mMeshes[i]->mNumFaces;
		SimpleVertex *Vert = new SimpleVertex[NumVert];
		// restart pointer
		void *ResetPtrVert = Vert;

		WORD *Indeces = new WORD[NumFaces * 3];
		// restart pointer
		void *ResetPtrIndeces = Indeces;

		for (int j = 0; j < NumVert; ++j)
		{
			Vert[j].Pos.x = Scene->mMeshes[i]->mVertices[j].x;
			Vert[j].Pos.y = Scene->mMeshes[i]->mVertices[j].y;
			Vert[j].Pos.z = Scene->mMeshes[i]->mVertices[j].z;
		}

		for (int k = 0; k < NumFaces; ++k)
		{

			/**Indeces = Scene->mMeshes[i]->mFaces[k].mIndices[1];
			Indeces++;
			*Indeces = Scene->mMeshes[i]->mFaces[k].mIndices[2];
			Indeces++;*/
			for (int w = 0; w < Scene->mMeshes[i]->mFaces[k].mNumIndices; ++w)
			{
				*Indeces = Scene->mMeshes[i]->mFaces[k].mIndices[w];
				Indeces++;
			}

		}

		Indeces = (WORD*)ResetPtrIndeces;
		Vert = (SimpleVertex*)ResetPtrVert;

		CBuffer TempVertex(NumVert,Vert);
		CBuffer TempIndex(NumFaces, Indeces);

		CMesh ResultMesh;

		ResultMesh.VertexBuffer = TempVertex;
		ResultMesh.IndexBuffer = TempIndex;
		ResultMesh.ChangeEveryFrame = CBuffer(sizeof(CBChangesEveryFrame));
		m_mesh.emplace_back(ResultMesh);

		delete[]Indeces;
		delete[]Vert;
	}


}