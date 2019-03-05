//--------------------------------------------------------------------------------------
// File: Tutorial07.cpp
//
// This application demonstrates texturing
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#define ASSIMP_DLL
#pragma once
#include "Header.h"
#include "CManager.h"
//
#include "Bin/assimp/Compiler/pushpack1.h"
#include "Bin/assimp/Compiler/poppack1.h"
#include "Bin/assimp/Compiler/pstdint.h"
#include "Bin/assimp/postprocess.h"
#include "Bin/assimp/Importer.hpp"
#include "Bin/assimp/scene.h"
//
CManager g_maneger;
CBuffer NeverChangeBuffer;
CBuffer ChangeOnResizeBuffer;
CBuffer ChangeEveryFrameBuffer;
CMesh g_mesh;
//--------------------------------------------------------------------------------------
// Structures
//--------------------------------------------------------------------------------------




//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow );
HRESULT InitDevice();
void CleanupDevice();
LRESULT CALLBACK    WndProc( HWND, UINT, WPARAM, LPARAM );
void Render();


//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
    UNREFERENCED_PARAMETER( hPrevInstance );
    UNREFERENCED_PARAMETER( lpCmdLine );

    if( FAILED( InitWindow( hInstance, nCmdShow ) ) )
        return 0;

    if( FAILED( InitDevice() ) )
    {
        CleanupDevice();
				return 0;
    }

    // Main message loop
    MSG msg = {0};
    while( WM_QUIT != msg.message )
    {
        if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
            Render();
        }
    }

  //  CleanupDevice();

    return ( int )msg.wParam;
}


//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow )
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof( WNDCLASSEX );
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon( hInstance, ( LPCTSTR )IDI_TUTORIAL1 );
    wcex.hCursor = LoadCursor( NULL, IDC_ARROW );
    wcex.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm = LoadIcon( wcex.hInstance, ( LPCTSTR )IDI_TUTORIAL1 );
    if( !RegisterClassEx( &wcex ) )
        return E_FAIL;

    // Create window
    g_maneger.m_hInst = hInstance;
    RECT rc = { 0, 0, 640, 480 };
    AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
    g_maneger.m_hWnd = CreateWindow( L"TutorialWindowClass", L"Direct3D 11 Tutorial 7", WS_OVERLAPPEDWINDOW,
                           CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,
                           NULL );
    if( !g_maneger.m_hWnd )
        return E_FAIL;

    ShowWindow( g_maneger.m_hWnd, nCmdShow );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DX11
//--------------------------------------------------------------------------------------
HRESULT CompileShaderFromFile( WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut )
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DX11CompileFromFile( szFileName, NULL, NULL, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL );
    if( FAILED(hr) )
    {
        if( pErrorBlob != NULL )
            OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() );
        if( pErrorBlob ) pErrorBlob->Release();
        return hr;
    }
    if( pErrorBlob ) pErrorBlob->Release();

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
HRESULT InitDevice()
{
    HRESULT hr = S_OK;

    RECT rc;
    GetClientRect( g_maneger.m_hWnd, &rc );
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = ARRAYSIZE( driverTypes );

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    UINT numFeatureLevels = ARRAYSIZE( featureLevels );

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory( &sd, sizeof( sd ) );
    sd.BufferCount = 1;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = g_maneger.m_hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
    {
        g_maneger.m_driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain( NULL, g_maneger.m_driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
                                            D3D11_SDK_VERSION, &sd, &g_maneger.mptr_SwapChain, &g_maneger.mptr_Device, &g_maneger.m_featureLevel, &g_maneger.mptr_ImmediateContext );
				if (SUCCEEDED(hr))
            break;
    }
		if (FAILED(hr))
        return hr;

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = NULL;
    hr = g_maneger.mptr_SwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer );
		if (FAILED(hr))
        return hr;

		/*ID3D11Texture2D* pBackBuffer2 = NULL;
		hr = g_maneger.mptr_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer2);
		if (FAILED(hr))
			return hr;*/

    hr = g_maneger.mptr_Device->CreateRenderTargetView( pBackBuffer, NULL, &g_maneger.mptr_RenderTargetView );
    pBackBuffer->Release();
		if (FAILED(hr))
        return hr;

	/*	hr = g_maneger.mptr_Device->CreateRenderTargetView(pBackBuffer2, NULL, &g_maneger.mptr_RenderTargetView2);
		pBackBuffer2->Release();
		if (FAILED(hr))
			return hr;*/

    // Create depth stencil texture
    D3D11_TEXTURE2D_DESC descDepth;
    ZeroMemory( &descDepth, sizeof(descDepth) );
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    hr = g_maneger.mptr_Device->CreateTexture2D( &descDepth, NULL, &g_maneger.mptr_DepthStencil );
    if( FAILED( hr ) )
        return hr;

    // Create the depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory( &descDSV, sizeof(descDSV) );
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr = g_maneger.mptr_Device->CreateDepthStencilView( g_maneger.mptr_DepthStencil, &descDSV, &g_maneger.mptr_DepthStencilView );
    if( FAILED( hr ) )
        return hr;

    g_maneger.mptr_ImmediateContext->OMSetRenderTargets( 1, &g_maneger.mptr_RenderTargetView2, g_maneger.mptr_DepthStencilView );
		//g_manager.m_pImmediateContext->OMSetRenderTargets(2, &g_manager.m_pRenderTargetView, g_manager.m_pDepthStencilView);
    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    g_maneger.mptr_ImmediateContext->RSSetViewports( 1, &vp );

    // Compile the vertex shader
    ID3DBlob* pVSBlob = NULL;
    hr = CompileShaderFromFile( L"Tutorial07.fx", "VS", "vs_4_0", &pVSBlob );
    if( FAILED( hr ) )
    {
        MessageBox( NULL,
                    L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK );
        return hr;
    }

    // Create the vertex shader
    hr = g_maneger.mptr_Device->CreateVertexShader( pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &g_maneger.mptr_VertexShader );
    if( FAILED( hr ) )
    {    
        pVSBlob->Release();
        return hr;
    }

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = ARRAYSIZE( layout );

    // Create the input layout
    hr = g_maneger.mptr_Device->CreateInputLayout( layout, numElements, pVSBlob->GetBufferPointer(),
                                          pVSBlob->GetBufferSize(), &g_maneger.mptr_VertexLayout );
    pVSBlob->Release();
    if( FAILED( hr ) )
        return hr;

    // Set the input layout
    g_maneger.mptr_ImmediateContext->IASetInputLayout( g_maneger.mptr_VertexLayout );

    // Compile the pixel shader
    ID3DBlob* pPSBlob = NULL;
    hr = CompileShaderFromFile( L"Tutorial07.fx", "PS", "ps_4_0", &pPSBlob );
    if( FAILED( hr ) )
    {
        MessageBox( NULL,
                    L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.",
									 L"Error", MB_OK );
        return hr;
		}

    // Create the pixel shader
    hr = g_maneger.mptr_Device->CreatePixelShader( pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &g_maneger.mptr_PixelShader );
    pPSBlob->Release();
    if( FAILED( hr ) )
        return hr;

    // Create vertex buffer
    SimpleVertex vertices[] ={
        { XMFLOAT3( -1.0f, 1.0f, -1.0f ), XMFLOAT2( 0.0f, 0.0f ) },
        { XMFLOAT3( 1.0f, 1.0f, -1.0f ), XMFLOAT2( 1.0f, 0.0f ) },
        { XMFLOAT3( 1.0f, 1.0f, 1.0f ), XMFLOAT2( 1.0f, 1.0f ) },
        { XMFLOAT3( -1.0f, 1.0f, 1.0f ), XMFLOAT2( 0.0f, 1.0f ) },

        { XMFLOAT3( -1.0f, -1.0f, -1.0f ), XMFLOAT2( 0.0f, 0.0f ) },
        { XMFLOAT3( 1.0f, -1.0f, -1.0f ), XMFLOAT2( 1.0f, 0.0f ) },
        { XMFLOAT3( 1.0f, -1.0f, 1.0f ), XMFLOAT2( 1.0f, 1.0f ) },
        { XMFLOAT3( -1.0f, -1.0f, 1.0f ), XMFLOAT2( 0.0f, 1.0f ) },

        { XMFLOAT3( -1.0f, -1.0f, 1.0f ), XMFLOAT2( 0.0f, 0.0f ) },
        { XMFLOAT3( -1.0f, -1.0f, -1.0f ), XMFLOAT2( 1.0f, 0.0f ) },
        { XMFLOAT3( -1.0f, 1.0f, -1.0f ), XMFLOAT2( 1.0f, 1.0f ) },
        { XMFLOAT3( -1.0f, 1.0f, 1.0f ), XMFLOAT2( 0.0f, 1.0f ) },

        { XMFLOAT3( 1.0f, -1.0f, 1.0f ), XMFLOAT2( 0.0f, 0.0f ) },
        { XMFLOAT3( 1.0f, -1.0f, -1.0f ), XMFLOAT2( 1.0f, 0.0f ) },
        { XMFLOAT3( 1.0f, 1.0f, -1.0f ), XMFLOAT2( 1.0f, 1.0f ) },
        { XMFLOAT3( 1.0f, 1.0f, 1.0f ), XMFLOAT2( 0.0f, 1.0f ) },

        { XMFLOAT3( -1.0f, -1.0f, -1.0f ), XMFLOAT2( 0.0f, 0.0f ) },
        { XMFLOAT3( 1.0f, -1.0f, -1.0f ), XMFLOAT2( 1.0f, 0.0f ) },
        { XMFLOAT3( 1.0f, 1.0f, -1.0f ), XMFLOAT2( 1.0f, 1.0f ) },
        { XMFLOAT3( -1.0f, 1.0f, -1.0f ), XMFLOAT2( 0.0f, 1.0f ) },

        { XMFLOAT3( -1.0f, -1.0f, 1.0f ), XMFLOAT2( 0.0f, 0.0f ) },
        { XMFLOAT3( 1.0f, -1.0f, 1.0f ), XMFLOAT2( 1.0f, 0.0f ) },
        { XMFLOAT3( 1.0f, 1.0f, 1.0f ), XMFLOAT2( 1.0f, 1.0f ) },
        { XMFLOAT3( -1.0f, 1.0f, 1.0f ), XMFLOAT2( 0.0f, 1.0f ) },
    };

    WORD indices[] ={
        3,1,0,
        2,1,3,

        6,4,5,
        7,4,6,

        11,9,8,
        10,9,11,

        14,12,13,
        15,12,14,

        19,17,16,
        18,17,19,

        22,20,21,
        23,20,22
    };

	// Create vertex buffer
		SimpleVertex Cubevertices[] = {
			{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
			{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
			{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
			{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },

			{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
			{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
			{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
			{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },

			{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
			{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
			{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
			{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },

			{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
			{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
			{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
			{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },

			{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
			{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
			{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
			{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },

			{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
			{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
			{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
			{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		};

		WORD Cubeindices[] = {
			3,1,0,
			2,1,3,

			6,4,5,
			7,4,6,

			11,9,8,
			10,9,11,

			14,12,13,
			15,12,14,

			19,17,16,
			18,17,19,

			22,20,21,
			23,20,22
		};

	SimpleVertex TriVertex[] = {
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
	};

	WORD TriIndex[] = {
		3,1,0
	};

	SimpleVertex SquereVertex[] = {
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
	};

	WORD SquereIndex[] = {
		3,1,0,
		2,1,3,
	};

	// Setting Mesh Cube
	//g_manager.m_mesh.push_back(CMesh(24, 36, Cubevertices, Cubeindices));
	// Setting Mesh Tr
	//g_manager.m_mesh.push_back(CMesh(4, 3, TriVertex, TriIndex));
	// Setting Mesh Square
	//	g_manager.m_mesh.push_back(CMesh(8, 6, SquereVertex, SquereIndex));

	g_maneger.GetMeshs("Dwarf/dwarf.x");

	for (int i = 0; i < g_maneger.m_mesh.size(); i++)
	{

		// Create vertex buffer
		hr = g_maneger.mptr_Device->CreateBuffer(g_maneger.m_mesh[i].VertexBuffer.GetDescriptionPtr(), 
																						 &g_maneger.m_mesh[i].VertexBuffer.m_InitData,
																						 &g_maneger.m_mesh[i].VertexBuffer.m_Buffer);
		if (FAILED(hr))
			return hr;


		//// Set vertex buffer
		g_maneger.mptr_ImmediateContext->IASetVertexBuffers(0, 1, &g_maneger.m_mesh[i].VertexBuffer.m_Buffer,
																												&g_maneger.m_mesh[i].VertexBuffer.m_stride, 
																												&g_maneger.m_mesh[i].VertexBuffer.m_offset);



		// Create index buffer
		hr = g_maneger.mptr_Device->CreateBuffer(g_maneger.m_mesh[i].IndexBuffer.GetDescriptionPtr(), &g_maneger.m_mesh[i].IndexBuffer.m_InitData, &g_maneger.m_mesh[i].IndexBuffer.m_Buffer);
		if (FAILED(hr))
			return hr;

		// Set index buffer
		g_maneger.mptr_ImmediateContext->IASetIndexBuffer(g_maneger.m_mesh[i].IndexBuffer.m_Buffer, DXGI_FORMAT_R16_UINT, 0);

	}
    // Set primitive topology
    g_maneger.mptr_ImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

    // Create the constant buffers
    //////////////////////////////////////////////////////////////////////////
		// Initialize The constant buffers

		GUID test;


		CoCreateGuid(&test);

    
	ChangeEveryFrameBuffer = CBuffer(sizeof(CBChangesEveryFrame));
    hr = g_maneger.mptr_Device->CreateBuffer( ChangeEveryFrameBuffer.GetDescriptionPtr(), NULL, &ChangeEveryFrameBuffer.m_Buffer );
    if( FAILED( hr ) )
        return hr;
		
	//////////////////////////////////////////////////////////////////////////
	NeverChangeBuffer = CBuffer(sizeof(CBNeverChanges));
    hr = g_maneger.mptr_Device->CreateBuffer( NeverChangeBuffer.GetDescriptionPtr(), NULL, &NeverChangeBuffer.m_Buffer );
    if( FAILED( hr ) )
        return hr;
    
	ChangeOnResizeBuffer = CBuffer(sizeof(CBChangeOnResize));
    //bd.ByteWidth = sizeof(CBChangeOnResize);
    hr = g_maneger.mptr_Device->CreateBuffer( ChangeOnResizeBuffer.GetDescriptionPtr(), NULL, &ChangeOnResizeBuffer.m_Buffer );
    if( FAILED( hr ) )
        return hr;
    

    // Load the Texture
    hr = D3DX11CreateShaderResourceViewFromFile( g_maneger.mptr_Device, L"seafloor.dds", NULL, NULL, &g_maneger.mptr_TextureRV, NULL );
    if( FAILED( hr ) )
        return hr;

    // Create the sample state
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory( &sampDesc, sizeof(sampDesc) );
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    hr = g_maneger.mptr_Device->CreateSamplerState( &sampDesc, &g_maneger.mptr_SamplerLinear );
    if( FAILED( hr ) )
        return hr;

    // Initialize the world matrices
    g_maneger.m_World = XMMatrixIdentity();

    // Initialize the view matrix
    XMVECTOR Eye = XMVectorSet( 0.0f, 3.0f, -6.0f, 0.0f );
    XMVECTOR At = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
    XMVECTOR Up = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
    g_maneger.m_View = XMMatrixLookAtLH( Eye, At, Up );

    CBNeverChanges cbNeverChanges;
    cbNeverChanges.mView = XMMatrixTranspose( g_maneger.m_View );
    g_maneger.mptr_ImmediateContext->UpdateSubresource( NeverChangeBuffer.m_Buffer, 0, NULL, &cbNeverChanges, 0, 0 );

    // Initialize the projection matrix
    g_maneger.m_Projection = XMMatrixPerspectiveFovLH( XM_PIDIV4, width / (FLOAT)height, 0.01f, 100.0f );
    
    CBChangeOnResize cbChangesOnResize;
    cbChangesOnResize.mProjection = XMMatrixTranspose( g_maneger.m_Projection );
    g_maneger.mptr_ImmediateContext->UpdateSubresource( ChangeOnResizeBuffer.m_Buffer, 0, NULL, &cbChangesOnResize, 0, 0 );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
void CleanupDevice()
{
    if( g_maneger.mptr_ImmediateContext ) g_maneger.mptr_ImmediateContext->ClearState();
    if(g_maneger.mptr_SamplerLinear ) g_maneger.mptr_SamplerLinear->Release();
    if( g_maneger.mptr_TextureRV ) g_maneger.mptr_TextureRV->Release();

    if( g_maneger.mptr_CBNeverChanges ) g_maneger.mptr_CBNeverChanges->Release();
    if( g_maneger.mptr_CBChangeOnResize ) g_maneger.mptr_CBChangeOnResize->Release();
    if( g_maneger.mptr_CBChangesEveryFrame ) g_maneger.mptr_CBChangesEveryFrame->Release();

	for (int i = 0; i < g_maneger.m_mesh.size(); i++)
	{
		if( g_maneger.m_mesh[i].VertexBuffer.m_Buffer ) g_maneger.m_mesh[i].VertexBuffer.m_Buffer->Release();
		if( g_maneger.m_mesh[i].IndexBuffer.m_Buffer )  g_maneger.m_mesh[i].IndexBuffer.m_Buffer->Release();
	}

    if( g_maneger.mptr_VertexLayout ) g_maneger.mptr_VertexLayout->Release();
    if( g_maneger.mptr_VertexShader ) g_maneger.mptr_VertexShader->Release();
    if( g_maneger.mptr_PixelShader ) g_maneger.mptr_PixelShader->Release();
    if( g_maneger.mptr_DepthStencil ) g_maneger.mptr_DepthStencil->Release();
    if( g_maneger.mptr_DepthStencilView ) g_maneger.mptr_DepthStencilView->Release();
    if( g_maneger.mptr_RenderTargetView ) g_maneger.mptr_RenderTargetView->Release();
		if (g_maneger.mptr_RenderTargetView2) g_maneger.mptr_RenderTargetView2->Release();
    if( g_maneger.mptr_SwapChain ) g_maneger.mptr_SwapChain->Release();
    if( g_maneger.mptr_ImmediateContext ) g_maneger.mptr_ImmediateContext->Release();
    if( g_maneger.mptr_Device ) g_maneger.mptr_Device->Release();
		
}


//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch( message )
    {
        case WM_PAINT:
            hdc = BeginPaint( hWnd, &ps );
            EndPaint( hWnd, &ps );
            break;

        case WM_DESTROY:
            PostQuitMessage( 0 );
            break;

        default:
            return DefWindowProc( hWnd, message, wParam, lParam );
    }

    return 0;
}


//--------------------------------------------------------------------------------------
// Render a frame
//--------------------------------------------------------------------------------------
void Render()
{
    // Update our time
    static float t = 0.0f;
    if( g_maneger.m_driverType == D3D_DRIVER_TYPE_REFERENCE )
    {
        t += ( float )XM_PI * 0.0125f;
    }
    else
    {
        static DWORD dwTimeStart = 0;
        DWORD dwTimeCur = GetTickCount();
        if( dwTimeStart == 0 )
            dwTimeStart = dwTimeCur;
        t = ( dwTimeCur - dwTimeStart ) / 1000.0f;
    }

    g_maneger.m_World = XMMatrixRotationY( t );

    // Modify the color
    g_maneger.m_vMeshColor.x = ( sinf( t * 1.0f ) + 1.0f ) * 0.5f;
    g_maneger.m_vMeshColor.y = ( cosf( t * 3.0f ) + 1.0f ) * 0.5f;
    g_maneger.m_vMeshColor.z = ( sinf( t * 5.0f ) + 1.0f ) * 0.5f;

    //
    // Clear the back buffer
    //
    float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red, green, blue, alpha
		float RED[4] = { 0.7f,0.1,0.1,0 };
    g_maneger.mptr_ImmediateContext->ClearRenderTargetView( g_maneger.mptr_RenderTargetView, ClearColor );
    //
		//g_maneger.mptr_ImmediateContext->ClearRenderTargetView(g_maneger.mptr_RenderTargetView2, RED);
    // Clear the depth buffer to 1.0 (max depth)
    //
    g_maneger.mptr_ImmediateContext->ClearDepthStencilView( g_maneger.mptr_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );

    //
    // Update variables that change once per frame
    //
    //CBChangesEveryFrame cb;
    
	//
    // Render the cube
    //
    g_maneger.mptr_ImmediateContext->VSSetShader( g_maneger.mptr_VertexShader, NULL, 0 );
    g_maneger.mptr_ImmediateContext->VSSetConstantBuffers( 0, 1, &NeverChangeBuffer.m_Buffer );
    g_maneger.mptr_ImmediateContext->VSSetConstantBuffers( 1, 1, &ChangeOnResizeBuffer.m_Buffer );
    g_maneger.mptr_ImmediateContext->VSSetConstantBuffers( 2, 1, &ChangeEveryFrameBuffer.m_Buffer);
    g_maneger.mptr_ImmediateContext->PSSetShader( g_maneger.mptr_PixelShader, NULL, 0 );
    g_maneger.mptr_ImmediateContext->PSSetConstantBuffers( 2, 1, &ChangeEveryFrameBuffer.m_Buffer);
    g_maneger.mptr_ImmediateContext->PSSetShaderResources( 0, 1, &g_maneger.mptr_TextureRV );
    g_maneger.mptr_ImmediateContext->PSSetSamplers( 0, 1, &g_maneger.mptr_SamplerLinear );

		
    g_maneger.mptr_ImmediateContext->DrawIndexed( 250, 0, 0 );
	XMMATRIX move4 = XMMATRIX(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.f, 0.0f, 0.0f, 1.0f
	);
	for (int i = 0; i < g_maneger.m_mesh.size(); i++)
	{
		g_maneger.m_mesh[i].m_object.mWorld = XMMatrixTranspose( g_maneger.m_World );
		g_maneger.m_mesh[i].m_object.vMeshColor = g_maneger.m_vMeshColor;
		g_maneger.mptr_ImmediateContext->UpdateSubresource( ChangeEveryFrameBuffer.m_Buffer, 0, NULL, &g_maneger.m_mesh[i].m_object, 0, 0 );
		if(i == 0)
		g_maneger.mptr_ImmediateContext->DrawIndexed(239, 0, 0 );
		else
		{
			g_maneger.mptr_ImmediateContext->DrawIndexed(1761, 0, 0);
		}

	/*	g_manager.m_mesh[1].m_object.mWorld = XMMatrixTranspose(g_manager.m_World);
		g_manager.m_mesh[1].m_object.vMeshColor = g_manager.m_vMeshColor;
		g_manager.m_pImmediateContext->UpdateSubresource(ChangeEveryFrameBuffer.m_Buffer, 0, NULL, &g_manager.m_mesh[1].m_object, 0, 0);
		g_manager.m_pImmediateContext->DrawIndexed(3, 0, 0);*/

	}


    //
    // Present our back buffer to our front buffer
    //
    g_maneger.mptr_SwapChain->Present( 0, 0 );
}
