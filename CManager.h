#pragma once
#include "Header.h"
#include "CMesh.h"
#include "Bin/assimp/cimport.h"
#include "Bin/assimp/postprocess.h"
#include "Bin/assimp/scene.h"

class CManager
{
public:
	CManager();
	~CManager();
public:
	std::vector<CMesh> m_mesh;
	//CMesh m_mesh;
	//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
	HINSTANCE                           m_hInst = NULL;
	HWND                                m_hWnd = NULL;
	D3D_DRIVER_TYPE                     m_driverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL                   m_featureLevel = D3D_FEATURE_LEVEL_11_0;
	ID3D11Device*                       mptr_Device = NULL;
	ID3D11DeviceContext*                mptr_ImmediateContext = NULL;
	IDXGISwapChain*                     mptr_SwapChain = NULL;
	ID3D11RenderTargetView*             mptr_RenderTargetView = NULL;
	ID3D11Texture2D*                    mptr_DepthStencil = NULL;
	ID3D11DepthStencilView*             mptr_DepthStencilView = NULL;
	ID3D11VertexShader*                 mptr_VertexShader = NULL;
	ID3D11PixelShader*                  mptr_PixelShader = NULL;
	ID3D11InputLayout*                  mptr_VertexLayout = NULL;

	ID3D11RenderTargetView*	mptr_RenderTargetView2 = NULL;
	//////////////////////////////////////////////////////////////////////////
	// Buffers
	//////////////////////////////////////////////////////////////////////////

	ID3D11Buffer*                       mptr_CBNeverChanges = NULL;
	ID3D11Buffer*                       mptr_CBChangeOnResize = NULL;
	ID3D11Buffer*                       mptr_CBChangesEveryFrame = NULL;


	ID3D11ShaderResourceView*           mptr_TextureRV = NULL;
	ID3D11SamplerState*                 mptr_SamplerLinear = NULL;
	XMMATRIX                            m_World;
	XMMATRIX                            m_View;
	XMMATRIX                            m_Projection;
	XMFLOAT4                            m_vMeshColor;
public: 
	void GetMeshs(const char * file);
};

