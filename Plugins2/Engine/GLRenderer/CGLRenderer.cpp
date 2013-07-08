///============================================================================
/// \note   Elemental Engine
///         Copyright (C) 2013 Social Systems Technology, Inc.
///
///         This code is redistributable under the terms of the EE License.
///
///         This code is distributed without warranty or implied warranty of
///			merchantability or fitness for a particular purpose. See the 
///			EE License for more details.
///
///         You should have received a copy of the EE License along with this
///			code; If not, write to Social Systems Technology, Inc.,
///			109 East 17th Street Suite 4210 Cheyenne, WY 82001 USA
///============================================================================

#include "Stdafx.h"

#include "CGLVertexBufferObject.h"
#include "CGLIndexBuffer.h"
#include "CGLShader.h"
#include "ILightObject.h"
#include "CGLRenderContext.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define RENDERER_BACKBUFFERCOUNT 1
#define RENDERER_SWAPEFFECT D3DSWAPEFFECT_DISCARD
#define RENDERER_PRESENTATIONINTERVAL D3DPRESENT_INTERVAL_IMMEDIATE
#define MAX_CONST_REGISTER_PS_30 255

#define RENDERER_DEFAULTMINZ .0f
#define RENDERER_DEFAULTMAXZ  1.f;//10000.f

REGISTER_COMPONENT_SINGLETON( CGLRenderer );
REGISTER_MESSAGE_HANDLER(GetRendererInterface, OnGetRendererInterface, CGLRenderer);
REGISTER_MESSAGE_HANDLER(SetDirectories, OnSetDirectories, CGLRenderer );
REGISTER_MESSAGE_HANDLER(GetRayFromMouseCoords, OnGetRayFromMouseCoords, CGLRenderer);
REGISTER_MESSAGE_HANDLER(TransformWorldToScreen, OnTransformWorldToScreen, CGLRenderer);

CGLRenderer::CGLRenderer()
{
	m_pDefaultEffect = NULL;
	m_PostProcessBuffer = NULL;
	m_LockedRenderBufferSurface = NULL;
	m_BackBufferDepthSurface = NULL;
	m_SetTextures = NULL;
	m_SetVertexTextures = NULL;
	m_PSConstants = NULL;
	m_VSConstants = NULL;

    m_RenderTarget = NULL;

	m_bInitialized = false;
	m_ExecutableDirectory = "";
	m_ResourceDirectory = "";
	Reset();
	//D3DXMatrixIdentity( &m_ModelViewMatrix );
	//D3DXMatrixIdentity( &m_ProjectionMatrix );
	//D3DXMatrixIdentity( &m_WorldMatrix );
	m_pOverrideRenderTarget = NULL;
	m_bWireFrameMode = false;
	m_APIGUID = INVALID_API_GUID;
	RenderBufferSurfaceLocked = false;

	memset( m_CurrentRenderState, -1, sizeof(m_CurrentRenderState) );
	memset( m_CurrentTextureStageState, -1, sizeof(m_CurrentTextureStageState) );
	memset( m_CurrentSamplerState, -1, sizeof(m_CurrentSamplerState) );


	m_hszPostProcessBufferName.Init( _T("CGLRenderer_PostProcessBuffer") );
}

/// \brief	Function to get this component.  This is a singleton
/// \param	void
/// \return	IComponent*: This Manager
IComponent *CGLRenderer::Create(int nArgs, va_list argptr)
{
	return SINGLETONINSTANCE( CGLRenderer );
}


DWORD  CGLRenderer::OnGetRendererInterface( DWORD size, void *params)
{
	GETRENDERERINTERFACEPARAMS * rInterfaceParams;

	rInterfaceParams = ((GETRENDERERINTERFACEPARAMS *)params);
	VERIFY_MESSAGE_SIZE(sizeof(GETRENDERERINTERFACEPARAMS), size);

	rInterfaceParams->m_RendererInterface = (IRenderer*)this;
    
	return MSG_HANDLED_PROCEED;
}


DWORD   CGLRenderer::OnSetDirectories( DWORD size, void * params )
{
	SETWORKINGDIRECTORIESPARAMS * rSetDirectoriesParams;

	rSetDirectoriesParams = ((SETWORKINGDIRECTORIESPARAMS *)params);
	VERIFY_MESSAGE_SIZE(sizeof(SETWORKINGDIRECTORIESPARAMS), size );
	if( rSetDirectoriesParams->BaseDirectory )
	{
		m_ExecutableDirectory = *rSetDirectoriesParams->BaseDirectory;
	}
	if(  rSetDirectoriesParams->ResourceDirectory )
	{
		m_ResourceDirectory = *rSetDirectoriesParams->ResourceDirectory;
	}

	EngineGetToolBox()->SetDirectories(&m_ExecutableDirectory, &m_ResourceDirectory);

	return MSG_HANDLED_PROCEED;
}

DWORD CGLRenderer::OnGetRayFromMouseCoords( DWORD size, void * params )
{
	// verify and cast the message
	VERIFY_MESSAGE_SIZE( size, sizeof(RAYFROMMOUSEPARAMS) );
	RAYFROMMOUSEPARAMS *raymouseparams = (RAYFROMMOUSEPARAMS *)params;

	// Get the trace position and direction
	Vec3 orig(0,0,0);
	Vec3 dir(0,1,0);
	Vec3 CamPos(0,0,0);
	CONTEXTCAMVECTORPARAMS ccvp;
	ccvp.pRC = NULL;
	ccvp.vdata = &CamPos;

	static DWORD msgHash_GetActiveCameraPosition = CHashString(_T("GetActiveCameraPosition")).GetUniqueID();
	if( EngineGetToolBox()->SendMessage(msgHash_GetActiveCameraPosition, sizeof( CONTEXTCAMVECTORPARAMS), &ccvp, NULL, NULL ) == MSG_HANDLED )
	{
		TransformScreenToWorld( raymouseparams->mouseX, raymouseparams->mouseY, dir );

		// output pos and dir
//		raymouseparams->rayPos->Set( orig );
		raymouseparams->rayPos->Set( CamPos );
		raymouseparams->rayDir->Set( dir );
		return MSG_HANDLED_STOP;
	}
	return MSG_ERROR;
}

DWORD CGLRenderer::OnTransformWorldToScreen(DWORD size, void *params)
{
	// verify and cast the message
	VERIFY_MESSAGE_SIZE( size, sizeof(WORLDTOSCREENCOORDPARAMS) );
	WORLDTOSCREENCOORDPARAMS *message = (WORLDTOSCREENCOORDPARAMS *)params;

	Vec3 *worldCoords = message->worldXYZ;

	if (message->cameraName == NULL)
	{
		CHashString currentCam;	
		CONTEXTCAMPARAMS ccp;
		ccp.pRC = NULL;
		ccp.szCameraName = &currentCam;
		static DWORD msgHash_GetActiveCamera = CHashString(_T("GetActiveCamera")).GetUniqueID();
		EngineGetToolBox()->SendMessage(msgHash_GetActiveCamera, sizeof(CONTEXTCAMPARAMS), &ccp);		

		if (ccp.szCameraName != NULL)
		{
			TransformWorldToScreen( *worldCoords, message->screenX, message->screenY, message->screenZ, &currentCam );
			return MSG_HANDLED_STOP;
		}
		else
		{
			EngineGetToolBox()->Log(LOGERROR, _T("GLRenderer: Could not get camera name in transform world to screen!\n"));	
			return MSG_ERROR;
		}
	}
	else
	{
		TransformWorldToScreen( *worldCoords, message->screenX, message->screenY, message->screenZ, message->cameraName );
		return MSG_HANDLED_STOP;
	}
	return MSG_ERROR;
}

void CGLRenderer::OnDestroyRenderer()
{
	SetVertexShader( NULL );
	SetPixelShader( NULL );

}

bool CGLRenderer::Initialize( HWND window,  bool fullscreen, const int width, const int height,
						   const int depthbits_in, const int colorbits_in )
{
	//HRESULT hr;
	//D3DDISPLAYMODE displayMode; // Structure to hold display mode info, like the resolution
	//UINT depthbits = depthbits_in;
	//UINT colorbits = colorbits_in;
	if( !m_bInitialized )
	{
		srand(15);
		/////
		//Initialize the OpenGL Device
		////////////////////////////////////////////////
        /// %%%% TODO %%%% ///

		// Initialize engine
        //displayMode.Width = width; // Set the screen width to the window width
        //displayMode.Height = height; // Set the screen height to the window height
        //displayMode.RefreshRate = 0; // Set the refresh rate to default
        //displayMode.Format = EEGLFormatFromColorBits( colorbits ); //

		//memset( &m_presentParameters, 0, sizeof( D3DPRESENT_PARAMETERS ) ); // We need to fill it with 0's first

		if ( !fullscreen  )
		{ 
			//m_presentParameters.Windowed   = TRUE; // Then set windowed to true
		}
		else // Otherwise
		{
			//m_presentParameters.Windowed   = FALSE; // Set windowed to false
		} 
		
		// Create the device

		//if ( FAILED( hr ) ) // If it failed, return failure
		//{
		//	EngineGetToolBox()->Log(LOGERROR, _T("GLRenderer: Failed to create device with resolution %dx%d!\n"), width, height);
		//	return false;
		//}

		// default clear the backbuffer to remove any noise
		SetColorMask( true, true, true, true );
		UINT r=150, g=150, b=150, a=255;
		SetBackgroundColor(r, g, b, a);
		ClearScreen( true, true );

		BuildConfigMap();

		//Init Shader Arrays
		CHashString maxVCs(_T("MAX_VERTEX_SHADER_CONSTANTS"));
		CHashString maxPCs(_T("MAX_PIXEL_SHADER_CONSTANTS"));
		m_VSConstants = new CGLRenderer_ShaderConstant[ (UINT)m_ConfigNameMap[maxVCs.GetUniqueID()] ];
		m_PSConstants = new CGLRenderer_ShaderConstant[ (UINT)m_ConfigNameMap[maxPCs.GetUniqueID()] ];
		

		//Init max texture and statemanager
		CHashString maxTexStagesName(_T("MAX_TEXTURE_STAGES"));
		int MaxTextures = m_ConfigNameMap[ maxTexStagesName.GetUniqueID() ];
		m_SetTextures = new IBaseTextureObject*[ MaxTextures ];
		m_iMaxTextures = MaxTextures;
		m_SetVertexTextures = new IBaseTextureObject*[ MaxTextures ];
		for( int i = 0; i < MaxTextures; i++ )
		{
			m_SetTextures[ i ] = NULL;
			m_SetVertexTextures [ i ] = NULL;
		}
		
		//Initialize the shader manager
		static DWORD msgHash_InitializeShaderManager = CHashString(_T("InitializeShaderManager")).GetUniqueID();
		EngineGetToolBox()->SendMessage(msgHash_InitializeShaderManager, 0, 0);

		m_iWindowWidth = 0;
		m_iWindowHeight = 0;
		m_Width = width;
		m_Height = height;

        // set default render states
#if 0
		m_pDevice->SetRenderState( D3DRS_ZFUNC,         D3DCMP_LESSEQUAL );
		m_pDevice->SetRenderState(D3DRS_STENCILENABLE,FALSE);	 	

		SetGLRenderState( RENDERSTATE_CULLMODE,         RENDERSTATEPARAM_CULLCCW  );		
		SetGLRenderState( RENDERSTATE_ZENABLE,         RENDERSTATEPARAM_ZTRUE  );// D3DZB_TRUE );
		SetGLRenderState( RENDERSTATE_ZWRITEENABLE,         RENDERSTATEPARAM_ZTRUE );// TRUE );
	       
		SetGLRenderState( RENDERSTATE_ALPHAREF, 64 ); 
		m_pDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL ); 

		SetGLRenderState(RENDERSTATE_SRCBLEND,RENDERSTATEPARAM_SRCALPHA);
		SetGLRenderState(RENDERSTATE_DESTBLEND,RENDERSTATEPARAM_INVSRCALPHA);
		SetGLRenderState(RENDERSTATE_ALPHABLENDENABLE,RENDERSTATEPARAM_TRUE);

		SetGLTextureStageState( 0, TEXTURESTAGESTATE_COLOROP, TEXTURESTAGEOP_MODULATE ); 
		SetGLTextureStageState( 0, TEXTURESTAGESTATE_COLORARG1, TEXTURESTAGEARG_TEXTURE );
		SetGLTextureStageState( 0, TEXTURESTAGESTATE_COLORARG2, TEXTURESTAGEARG_DIFFUSE );
		SetGLTextureStageState( 0, TEXTURESTAGESTATE_ALPHAOP, TEXTURESTAGEOP_SELECTARG2 ); 
		SetGLTextureStageState( 0, TEXTURESTAGESTATE_ALPHAARG2, TEXTURESTAGEARG_DIFFUSE ); 
		SetGLSamplerStageState( 0,  SAMPLERSTATE_MINFILTER, TEXTURESTAGE_TEXF_LINEAR );
		SetGLSamplerStageState( 0,  SAMPLERSTATE_MAGFILTER, TEXTURESTAGE_TEXF_LINEAR );
#endif
        
		AllocateFullscreenQuadVertexBuffer();

		SetDefaultStates();
		//Initialize enums for mapping hashstrings to renderstate and texture state modes and values
		RegisterTextureStageEnumHashes();
		m_bInitialized = true;

		return true;
	}
	else //already initialized
	{
		return false;
	}
}

void CGLRenderer::DeInitialize()
{
	Reset();
}

void CGLRenderer::Reset()
{
	RemovePostProcessBuffer();

	static DWORD msgHash_RemoveVertexAllocation = CHashString(_T("RemoveVertexAllocation")).GetUniqueID();
	EngineGetToolBox()->SendMessage(msgHash_RemoveVertexAllocation, sizeof(m_FullscreenQuadVBAllocation), &m_FullscreenQuadVBAllocation );
	m_FullscreenQuadVBAllocation.m_Size = 0;
	m_FullscreenQuadVertexBuffer = NULL;

	m_RenderContext = NULL;
	m_Ortho2DScreenSizeSet =false;
	m_bInitialized = false;
	// Release the device

	memset( m_VertexStreams, 0, sizeof( m_VertexStreams ) );
	memset( m_ClearColor, (UINT)150, sizeof( m_ClearColor  ) );
	m_ClearColor[ 0 ] = 0;

	m_VertexDeclSet = NULL;
	m_FVFSet = -1;
	m_IndexStream = NULL;

	if( m_VSConstants != NULL )
		delete [] m_VSConstants;
	m_VSConstants = NULL;

	if( m_PSConstants != NULL )
		delete [] m_PSConstants;
	m_PSConstants = NULL;
	
	// release all vertex declarations
	for (VERTEXDECLARATIONMAP::iterator itr=m_VertexDeclarationList.begin(); itr!=m_VertexDeclarationList.end(); itr++)
	{
		//GLenum vcID = itr->second;
	}
	m_VertexDeclarationList.clear();
	m_VertexFvFList.clear();
	m_PixelShaderSet = NULL;
	m_VertexShaderSet = NULL;
}

void CGLRenderer::UsePBuffers( const int width, const int height )
{
	//Change to RenderTargets
	//TODO:
	
}

void CGLRenderer::ResizeScreen( const int &width, const int &height )
{
	if( m_bInitialized )
	{
		m_Width = width;
		m_Height = height;
		//resize display
		//m_presentParameters.BackBufferHeight = height;
		//m_presentParameters.BackBufferWidth = width;
		//m_pDevice->Reset( &m_presentParameters );
	}
}

void CGLRenderer::ClearScreen( bool clearDepth, bool clearTarget )
{
	SetColorMask(true, true, true, true);
    
	GLbitfield clearFlags = 0;
	if ( clearDepth )
        clearFlags  |= GL_DEPTH_BUFFER_BIT;
	if ( clearTarget )
        clearFlags |= GL_COLOR_BUFFER_BIT;

    glClearColor((GLfloat)m_ClearColor[1] / 255.0f, (GLfloat)m_ClearColor[2] / 255.0f, (GLfloat)m_ClearColor[3] / 255.0f, (GLfloat)m_ClearColor[0] / 255.0f);
    glClearDepth(1.0f);
	// Clear the screen
    glClear(clearFlags);
    CheckAndLogGLError();
}

void CGLRenderer::BeginScene( bool defaultBufferRender, ITextureObject *pOverrideRenderTarget )
{
	//HRESULT hr;
	if( m_bInitialized )
	{
		if (!defaultBufferRender)
		{
			if (!pOverrideRenderTarget)
				pOverrideRenderTarget = m_RenderTarget;
			if (!pOverrideRenderTarget)
				pOverrideRenderTarget = m_PostProcessBuffer;
		}

		if (pOverrideRenderTarget)
		{
			ApplyRenderTarget( pOverrideRenderTarget, 0);
			m_pOverrideRenderTarget = pOverrideRenderTarget;
		}

		//hr = m_pDevice->BeginScene();
		//assert(	SUCCEEDED(hr) );
	}
}

void CGLRenderer::ApplyRenderTarget( ITextureObject * tex, UINT target = 0 )
{
#if 0
	HRESULT hr;
	//grab current target
	hr = m_pDevice->GetRenderTarget( target, &m_OldTargets[ target ] );
	if (hr != D3D_OK)
	{
		m_OldTargets[ target ] = NULL;
	}
//	assert( SUCCEEDED(hr) );
	map< IBaseTextureObject *, LPDIRECT3DSURFACE9 >::iterator iter = m_TextureSurfs.find( tex );
	if( iter == m_TextureSurfs.end() )
	{
		CGLTextureObject * GLRT = dynamic_cast< CGLTextureObject * >(tex);
		if( GLRT )
		{
			LPDIRECT3DSURFACE9 pSurface = GLRT->GetRenderTargetSurface();
			if (pSurface != NULL)
				hr = m_pDevice->SetRenderTarget( target, pSurface );
			m_TextureSurfs[tex] = pSurface;
		}
	}
	else
	{
		LPDIRECT3DSURFACE9 pSurface = iter->second;
		if (pSurface != NULL)
		{
			hr = m_pDevice->SetRenderTarget( target, iter->second );
			assert( SUCCEEDED(hr) );
		}
	}
    
#endif
}

bool CGLRenderer::SetDepthTarget( IBaseTextureObject * texture )
{
	//HRESULT hr;
	//check format first
	CGLTextureObject * GLRT = dynamic_cast< CGLTextureObject * >(texture);
	if( GLRT)
	{
#if 0
		hr = m_pDevice->GetDepthStencilSurface( &m_BackBufferDepthSurface );
		assert( SUCCEEDED(hr) );
		LPDIRECT3DTEXTURE9 DepthTarget = (LPDIRECT3DTEXTURE9)GLRT->GetAPITexture();
		LPDIRECT3DSURFACE9 pSurface;
		hr = DepthTarget->GetSurfaceLevel( 0 , &pSurface );
		assert( SUCCEEDED(hr) );
		hr = m_pDevice->SetDepthStencilSurface( pSurface );
		assert( SUCCEEDED(hr) );
		pSurface->Release();
		return ( hr == D3D_OK );
#endif
        return true;
	}
	else
		if ( !GLRT )
	{
		//restore previous depth surface
		if( m_BackBufferDepthSurface )
		{
#if 0
			LPDIRECT3DSURFACE9 pSurface;
			hr = m_pDevice->GetDepthStencilSurface( &pSurface );
			assert( SUCCEEDED(hr) );
			hr = m_pDevice->SetDepthStencilSurface( NULL );
			assert( SUCCEEDED(hr) );
			hr = D3DXLoadSurfaceFromSurface( m_BackBufferDepthSurface,
				NULL,
				NULL,
				pSurface, 
				NULL,
				NULL,
				D3DX_FILTER_NONE, 
				0 ); 
			assert( SUCCEEDED(hr) );
			pSurface->Release();
			hr = m_pDevice->SetDepthStencilSurface( m_BackBufferDepthSurface );
			assert( SUCCEEDED(hr) );
			m_BackBufferDepthSurface->Release();
			m_BackBufferDepthSurface = NULL;
#endif
		}
	}

	return false;
}

bool CGLRenderer::CopyDepthTarget( IBaseTextureObject * texture )
{
#if 0
	HRESULT hr;
	//check format first
	CGLTextureObject * GLRT = dynamic_cast< CGLTextureObject * >(texture);
	if( m_pDevice &&
		GLRT)
	{
		LPDIRECT3DSURFACE9 backBufferDepth;
		hr = m_pDevice->GetDepthStencilSurface( &backBufferDepth );
		assert( SUCCEEDED(hr) );
		LPDIRECT3DTEXTURE9 DepthTarget = (LPDIRECT3DTEXTURE9)GLRT->GetAPITexture();
		LPDIRECT3DSURFACE9 pSurface;
		hr = DepthTarget->GetSurfaceLevel( 0 , &pSurface );
		assert( SUCCEEDED(hr) );
		hr = m_pDevice->StretchRect( backBufferDepth, NULL, pSurface, NULL, D3DTEXF_NONE );
		assert( SUCCEEDED(hr) );
		pSurface->Release();
		backBufferDepth->Release();
		return ( hr == D3D_OK );
	}
#endif//XBOX

	return false;
}

void CGLRenderer::EndScene()
{
	if( m_bInitialized )
	{
#if 0
        HRESULT hr;
		hr = m_pDevice->EndScene();
		assert( SUCCEEDED(hr) );
#endif
		if (m_pOverrideRenderTarget)
		{
			UnApplyRenderTarget( m_pOverrideRenderTarget, 0  );
			m_pOverrideRenderTarget->GenerateMips();
			m_pOverrideRenderTarget = NULL;
		}

		// can't have these set to a device when we lock them during the next Update
		SetVertexStream( 0, NULL );
		SetIndices( NULL );
	}
}

void CGLRenderer::UnApplyRenderTarget( ITextureObject * tex, UINT target)
{
	if (m_TextureSurfs.find( tex ) == m_TextureSurfs.end())
		return;

	m_TextureSurfs.erase( tex );

	CGLTextureObject * GLRT = dynamic_cast< CGLTextureObject * >(tex);

#if 0
	HRESULT hr = m_pDevice->SetRenderTarget( target, m_OldTargets[ target ] );
	assert( SUCCEEDED(hr) );
#endif
	GLRT->ReleaseRenderTargetSurface();
}

bool CGLRenderer::RenderToContext( IRenderContext * context )
{
	CGLRenderContext * CGLcontext = dynamic_cast< CGLRenderContext * >( context );
	{
		m_RenderContext = CGLcontext;
	}
	return false;
}

IRenderContext * CGLRenderer::CreateNewContext( HWND window, const int width, 
											 const int height, const int depthbits, 
											 const int colorbits )
{
	CGLRenderContext * CGLcontext = new CGLRenderContext();
	CGLcontext->CreateContext( window, width, height, depthbits, colorbits );	
	return CGLcontext;
}

void CGLRenderer::SetWindowDimensions( const int width, const int height )
{
	m_iWindowWidth = width;
	m_iWindowHeight = height;
}

void CGLRenderer::SetViewport( const int xpos, const int ypos, const int width, const int height )
{
	if( m_bInitialized )
	{
#if 0
		D3DVIEWPORT9 view;
		view.Height = height;
		view.Width = width;
		view.MaxZ = RENDERER_DEFAULTMAXZ;
		view.MinZ = RENDERER_DEFAULTMINZ;
		view.X = xpos;
		view.Y = ypos;
		m_pDevice->SetViewport( &view );
		m_Width = width;
		m_Height = height;
#endif
	}
}

void CGLRenderer::GetViewDimensions( int &width, int &height )
{
	if( m_RenderTarget )
	{
		width = m_RenderTarget->GetWidth();
		height = m_RenderTarget->GetHeight();
	}
	else
	{
		width = m_Width;
		height = m_Height;
	}
}

void CGLRenderer::GetWindowDimensions( int &width, int &height )
{
	if (m_iWindowWidth == 0 || m_iWindowHeight == 0)
	{
		GetViewDimensions(width, height);
	}
	else
	{
		width = m_iWindowWidth;
		height = m_iWindowHeight;
	}
}

void CGLRenderer::Draw2DMask(const float xpos, const float ypos, const float width, const float height, const StdString &texture)
{
	//TODO:
}

void CGLRenderer::Draw2DLine(const float startX, const float startY, const float endX, const float endY, const int r, const int g, const int b, const float width)
{
#if 0
	static const int sizeBuffer = 2;
	static CGLRenderer_VertexRHW	vPoints[ sizeBuffer ];

    SetGLTextureStageState( 0, TEXTURESTAGESTATE_COLOROP, TEXTURESTAGEOP_SELECTARG2 ); 
    SetGLTextureStageState( 0, TEXTURESTAGESTATE_COLORARG1, TEXTURESTAGEARG_TEXTURE ); 
    SetGLTextureStageState( 0, TEXTURESTAGESTATE_COLORARG2, TEXTURESTAGEARG_DIFFUSE); 
    SetGLTextureStageState( 0, TEXTURESTAGESTATE_ALPHAOP, TEXTURESTAGEOP_SELECTARG2 ); 
    SetGLTextureStageState( 0, TEXTURESTAGESTATE_ALPHAARG1, TEXTURESTAGEARG_TEXTURE ); 
    SetGLTextureStageState( 0, TEXTURESTAGESTATE_ALPHAARG2, TEXTURESTAGEARG_DIFFUSE); 
    SetGLTextureStageState( 1, TEXTURESTAGESTATE_COLOROP, TEXTURESTAGEOP_DISABLE ); 
    SetGLTextureStageState( 1, TEXTURESTAGESTATE_ALPHAOP, TEXTURESTAGEOP_DISABLE ); 

	D3DCOLOR color = D3DCOLOR_ARGB( 255, r, g, b );
	vPoints[ 0 ].pos[0] = startX;
	vPoints[ 0 ].pos[1] = startY;
	vPoints[ 0 ].pos[2] = 0;
	vPoints[ 0 ].color = color;
	vPoints[ 1 ].pos[0] = endX;
	vPoints[ 1 ].pos[1] = endY;
	vPoints[ 1 ].pos[2] = 0;
	vPoints[ 1 ].color = color;

	//requires ortho projection!
	SetFVF( CGLRENDERER_2DVERTEX_FVF );
	SetPixelShader( NULL );
	SetVertexShader( NULL );
	SetBlendMode( BLEND_NORMAL );
	SetVertexStream( 0, NULL );
	SetIndices( NULL );
	
	//this function checks for redundancies
	PushMatrix( PROJECTION_MATRIX );
	PushMatrix( WORLD_MATRIX );
	PushMatrix( VIEW_MATRIX );
	SetOrtho2DScreenSize();
		
	{
		DrawPrimUp( D3DPT_LINELIST,
								1,
								vPoints,
								sizeof( CGLRenderer_VertexRHW  ));

	}
	PopMatrix( PROJECTION_MATRIX );
	PopMatrix( WORLD_MATRIX );
	PopMatrix( VIEW_MATRIX );
#endif
}

void CGLRenderer::Draw3DLine(const float startX, const float startY, const float startZ, const float endX, const float endY, const float endZ, const int r, const int g, const int b, const float width, bool bOverrideTransform)
{
#if 0
	static Matrix4x4 matworld;
	static const int sizeBuffer = 2;
	static CGLRenderer_VertexRHW	vPoints[ sizeBuffer ];

	if (bOverrideTransform)
	{
		PushMatrix( WORLD_MATRIX );
		matworld.SetIdentity();
		SetMatrix( WORLD_MATRIX, (CONST FLOAT*)matworld.GetMatrix() );
	}

	D3DCOLOR color = D3DCOLOR_ARGB( 255, r, g, b );

	if (width == 1.0f || (m_pD3DXLine == NULL))
	{
		SetGLTextureStageState( 0, TEXTURESTAGESTATE_COLOROP, TEXTURESTAGEOP_SELECTARG2 ); 
		SetGLTextureStageState( 0, TEXTURESTAGESTATE_COLORARG1, TEXTURESTAGEARG_TEXTURE ); 
		SetGLTextureStageState( 0, TEXTURESTAGESTATE_COLORARG2, TEXTURESTAGEARG_DIFFUSE); 
		SetGLTextureStageState( 0, TEXTURESTAGESTATE_ALPHAOP, TEXTURESTAGEOP_SELECTARG2 ); 
		SetGLTextureStageState( 0, TEXTURESTAGESTATE_ALPHAARG1, TEXTURESTAGEARG_TEXTURE ); 
		SetGLTextureStageState( 0, TEXTURESTAGESTATE_ALPHAARG2, TEXTURESTAGEARG_DIFFUSE); 
		SetGLTextureStageState( 1, TEXTURESTAGESTATE_COLOROP, TEXTURESTAGEOP_DISABLE ); 
		SetGLTextureStageState( 1, TEXTURESTAGESTATE_ALPHAOP, TEXTURESTAGEOP_DISABLE ); 

		vPoints[ 0 ].pos[0] = startX;
		vPoints[ 0 ].pos[1] = startY;
		vPoints[ 0 ].pos[2] = startZ;
		vPoints[ 0 ].color = color;
		vPoints[ 1 ].pos[0] = endX;
		vPoints[ 1 ].pos[1] = endY;
		vPoints[ 1 ].pos[2] = endZ;
		vPoints[ 1 ].color = color;

		SetFVF( CGLRENDERER_2DVERTEX_FVF );
		SetPixelShader( NULL );
		SetVertexShader( NULL );
		SetBlendMode( BLEND_NORMAL );
		SetVertexStream( 0, NULL );
		SetIndices( NULL );

		DrawPrimUp( D3DPT_LINELIST,
								1,
								vPoints,
								sizeof( CGLRenderer_VertexRHW  ));
	}
	else if (m_pD3DXLine)
	{
		D3DXMATRIX modelViewProj;
		D3DXMatrixMultiply( &modelViewProj, &m_ModelViewMatrix, &m_ProjectionMatrix );
		D3DXMatrixMultiply( &modelViewProj, &m_WorldMatrix, &modelViewProj );

		D3DXVECTOR3 vVertexList[2];
		vVertexList[0].x = startX;
		vVertexList[0].y = startY;
		vVertexList[0].z = startZ;
		vVertexList[1].x = endX;
		vVertexList[1].y = endY;
		vVertexList[1].z = endZ;
		m_pD3DXLine->SetWidth( width );
		m_pD3DXLine->Begin();
		m_pD3DXLine->DrawTransform( vVertexList, 2, &modelViewProj, color );
		m_pD3DXLine->End();
	}

	if (bOverrideTransform)
	{
		PopMatrix( WORLD_MATRIX );
	}
#endif
}

void CGLRenderer::DrawPoint(const float xpos, const float ypos, const float zpos, const int r, const int g, const int b, const float size)
{
#if 0
	static Matrix4x4 matworld;
	static CGLRenderer_VertexRHW	vPoints;

	PushMatrix( WORLD_MATRIX );
	matworld.SetIdentity();
	SetMatrix( WORLD_MATRIX, (CONST FLOAT*)matworld.GetMatrix() );
	
	SetGLTextureStageState( 0, TEXTURESTAGESTATE_COLOROP, TEXTURESTAGEOP_SELECTARG2 ); 
    SetGLTextureStageState( 0, TEXTURESTAGESTATE_COLORARG1, TEXTURESTAGEARG_TEXTURE ); 
    SetGLTextureStageState( 0, TEXTURESTAGESTATE_COLORARG2, TEXTURESTAGEARG_DIFFUSE); 
    SetGLTextureStageState( 0, TEXTURESTAGESTATE_ALPHAOP, TEXTURESTAGEOP_SELECTARG2 ); 
    SetGLTextureStageState( 0, TEXTURESTAGESTATE_ALPHAARG1, TEXTURESTAGEARG_TEXTURE ); 
    SetGLTextureStageState( 0, TEXTURESTAGESTATE_ALPHAARG2, TEXTURESTAGEARG_DIFFUSE); 
    SetGLTextureStageState( 1, TEXTURESTAGESTATE_COLOROP, TEXTURESTAGEOP_DISABLE ); 
    SetGLTextureStageState( 1, TEXTURESTAGESTATE_ALPHAOP, TEXTURESTAGEOP_DISABLE ); 

	D3DCOLOR color = D3DCOLOR_ARGB( 255, r, g, b );
	vPoints.pos[0] = xpos;
	vPoints.pos[1] = ypos;
	vPoints.pos[2] = zpos;
	vPoints.color = color;

	SetFVF( CGLRENDERER_2DVERTEX_FVF );
	SetPixelShader( NULL );
	SetVertexShader( NULL );
	SetBlendMode( BLEND_NORMAL );
	SetVertexStream( 0, NULL );
	SetIndices( NULL );
	
	//Make sure you understand why the casting is like this before you change anything:
	SetGLRenderState(RENDERSTATE_POINTSIZE, *((DWORD*)&size));

	{
		DrawPrimUp( D3DPT_POINTLIST,
								1,
								&vPoints,
								sizeof( CGLRenderer_VertexRHW  ));
	}
	PopMatrix( WORLD_MATRIX );
#endif
}

void CGLRenderer::DrawBoundingBox( Vec3 &in_min, Vec3 &in_max, int colorR, int colorG, int colorB, int colorA )
{
#if 0
	static const int sizeBuffer = 24;
	static Matrix4x4 matworld;
	static CGLRenderer_VertexRHW	vPoints[ sizeBuffer ];
	
	PushMatrix( WORLD_MATRIX );
	matworld.SetIdentity();
	SetMatrix( WORLD_MATRIX, (CONST FLOAT*)matworld.GetMatrix() );
	//Set color to derive from the vertex
    SetGLTextureStageState( 0, TEXTURESTAGESTATE_COLOROP, TEXTURESTAGEOP_SELECTARG2 ); 
    SetGLTextureStageState( 0, TEXTURESTAGESTATE_COLORARG1, TEXTURESTAGEARG_TEXTURE ); 
    SetGLTextureStageState( 0, TEXTURESTAGESTATE_COLORARG2, TEXTURESTAGEARG_DIFFUSE); 
    SetGLTextureStageState( 0, TEXTURESTAGESTATE_ALPHAOP, TEXTURESTAGEOP_SELECTARG2 ); 
    SetGLTextureStageState( 0, TEXTURESTAGESTATE_ALPHAARG1, TEXTURESTAGEARG_TEXTURE ); 
    SetGLTextureStageState( 0, TEXTURESTAGESTATE_ALPHAARG2, TEXTURESTAGEARG_DIFFUSE); 
    SetGLTextureStageState( 1, TEXTURESTAGESTATE_COLOROP, TEXTURESTAGEOP_DISABLE ); 
    SetGLTextureStageState( 1, TEXTURESTAGESTATE_ALPHAOP, TEXTURESTAGEOP_DISABLE ); 

	vPoints[0].pos[0] = in_min.x;
	vPoints[0].pos[1] = in_min.y;
	vPoints[0].pos[2] = in_min.z;

	vPoints[1].pos[0] = in_max.x;
	vPoints[1].pos[1] = in_min.y;
	vPoints[1].pos[2] = in_min.z;

	vPoints[3].pos[0] = in_max.x;
	vPoints[3].pos[1] = in_max.y;
	vPoints[3].pos[2] = in_min.z;

	vPoints[5].pos[0] = in_min.x;
	vPoints[5].pos[1] = in_max.y;
	vPoints[5].pos[2] = in_min.z;

	vPoints[8].pos[0] = in_min.x;
	vPoints[8].pos[1] = in_min.y;
	vPoints[8].pos[2] = in_max.z;

	vPoints[9].pos[0] = in_max.x;
	vPoints[9].pos[1] = in_min.y;
	vPoints[9].pos[2] = in_max.z;

	vPoints[11].pos[0] = in_max.x;
	vPoints[11].pos[1] = in_max.y;
	vPoints[11].pos[2] = in_max.z;

	vPoints[13].pos[0] = in_min.x;
	vPoints[13].pos[1] = in_max.y;
	vPoints[13].pos[2] = in_max.z;

	D3DCOLOR color = D3DCOLOR_ARGB( colorA, colorR, colorG, colorB );
	//Not using indices, but this is the order of the line list:
	//0,1, 2,3, 4,5, 6,7  8,9  10,11,12,13,...
	//static const for optimization during compile time
	static const unsigned short vIndices[24] = 
	{ 0,1, 1,3, 3,5, 5,0,  8,9, 9,11, 11,13, 13,8,  0,8, 1,9, 3,11, 5,13 };
	for( int i = 0; i < sizeBuffer; i++ )
	{
		vPoints[ i ].color = color;
		if( vIndices[ i ] != i )
		{
			vPoints[ i ] = vPoints[ vIndices[ i ] ];
		}
	}
	
	SetFVF( CGLRENDERER_2DVERTEX_FVF );
	SetPixelShader( NULL );
	SetVertexShader( NULL );
	SetBlendMode( BLEND_NORMAL );
	SetVertexStream( 0, NULL );
	SetIndices( NULL );

	{
		DrawPrimUp( D3DPT_LINELIST,
							12, 
							vPoints, 
							sizeof(CGLRenderer_VertexRHW) );
	}
	PopMatrix( WORLD_MATRIX );
#endif
}

void CGLRenderer::DrawOrientedBoundingBox( Vec3 &in_min, Vec3 &in_max, Matrix3x3 &in_rot, Vec3 &in_pos, int colorR, int colorG, int colorB )
{
#if 0
	static const int sizeBuffer = 24;
	static Matrix4x4 matworld;
	static CGLRenderer_VertexRHW	vPoints[ sizeBuffer ];
	
	PushMatrix( WORLD_MATRIX );
	matworld.SetIdentity();
	matworld.SetFrom3x3( in_rot.GetMatrix() );
	matworld.SetTranslation( in_pos );
	SetMatrix( WORLD_MATRIX, (CONST FLOAT*)matworld.GetMatrix() );
	//Set color to derive from the vertex
    SetGLTextureStageState( 0, TEXTURESTAGESTATE_COLOROP, TEXTURESTAGEOP_SELECTARG2 ); 
    SetGLTextureStageState( 0, TEXTURESTAGESTATE_COLORARG1, TEXTURESTAGEARG_TEXTURE ); 
    SetGLTextureStageState( 0, TEXTURESTAGESTATE_COLORARG2, TEXTURESTAGEARG_DIFFUSE); 
    SetGLTextureStageState( 0, TEXTURESTAGESTATE_ALPHAOP, TEXTURESTAGEOP_SELECTARG2 ); 
    SetGLTextureStageState( 0, TEXTURESTAGESTATE_ALPHAARG1, TEXTURESTAGEARG_TEXTURE ); 
    SetGLTextureStageState( 0, TEXTURESTAGESTATE_ALPHAARG2, TEXTURESTAGEARG_DIFFUSE); 
    SetGLTextureStageState( 1, TEXTURESTAGESTATE_COLOROP, TEXTURESTAGEOP_DISABLE ); 
    SetGLTextureStageState( 1, TEXTURESTAGESTATE_ALPHAOP, TEXTURESTAGEOP_DISABLE ); 

	vPoints[0].pos[0] = in_min.x;
	vPoints[0].pos[1] = in_min.y;
	vPoints[0].pos[2] = in_min.z;

	vPoints[1].pos[0] = in_max.x;
	vPoints[1].pos[1] = in_min.y;
	vPoints[1].pos[2] = in_min.z;

	vPoints[3].pos[0] = in_max.x;
	vPoints[3].pos[1] = in_max.y;
	vPoints[3].pos[2] = in_min.z;

	vPoints[5].pos[0] = in_min.x;
	vPoints[5].pos[1] = in_max.y;
	vPoints[5].pos[2] = in_min.z;

	vPoints[8].pos[0] = in_min.x;
	vPoints[8].pos[1] = in_min.y;
	vPoints[8].pos[2] = in_max.z;

	vPoints[9].pos[0] = in_max.x;
	vPoints[9].pos[1] = in_min.y;
	vPoints[9].pos[2] = in_max.z;

	vPoints[11].pos[0] = in_max.x;
	vPoints[11].pos[1] = in_max.y;
	vPoints[11].pos[2] = in_max.z;

	vPoints[13].pos[0] = in_min.x;
	vPoints[13].pos[1] = in_max.y;
	vPoints[13].pos[2] = in_max.z;

	D3DCOLOR color = D3DCOLOR_ARGB( 255, colorR, colorG, colorB );
	//Not using indices, but this is the order of the line list:
	//0,1, 2,3, 4,5, 6,7  8,9  10,11,12,13,...
	//static const for optimization during compile time
	static const unsigned short vIndices[24] = 
	{ 0,1, 1,3, 3,5, 5,0,  8,9, 9,11, 11,13, 13,8,  0,8, 1,9, 3,11, 5,13 };
	for( int i = 0; i < sizeBuffer; i++ )
	{
		vPoints[ i ].color = color;
		if( vIndices[ i ] != i )
		{
			vPoints[ i ] = vPoints[ vIndices[ i ] ];
		}
	}
	
	SetFVF( CGLRENDERER_2DVERTEX_FVF );
	SetPixelShader( NULL );
	SetVertexShader( NULL );
	SetBlendMode( BLEND_NORMAL );
	SetVertexStream( 0, NULL );
	SetIndices( NULL );
	SetDepthWrite( true );
	SetDepthTest( true );

	

	{
		DrawPrimUp( D3DPT_LINELIST,
							12, 
							vPoints, 
							sizeof(CGLRenderer_VertexRHW) );
	}
	PopMatrix( WORLD_MATRIX );
#endif
}

void CGLRenderer::DrawAxis( float &in_length, Matrix3x3 &in_rot, Vec3 &in_pos, int colorXR, int colorXG, int colorXB , int colorYR, int colorYG, int colorYB, int colorZR, int colorZG, int colorZB )
{
#if 0
	static const int sizeBuffer = 6;
	static Matrix4x4 matworld;
	static CGLRenderer_VertexRHW vPoints[ sizeBuffer ];
	
	PushMatrix( WORLD_MATRIX );
	matworld.SetIdentity();
	matworld.SetFrom3x3( in_rot.GetMatrix() );
	matworld.SetTranslation( in_pos );
	SetMatrix( WORLD_MATRIX, (CONST FLOAT*)matworld.GetMatrix() );
	//Set color to derive from the vertex
    SetGLTextureStageState( 0, TEXTURESTAGESTATE_COLOROP, TEXTURESTAGEOP_SELECTARG2 ); 
    SetGLTextureStageState( 0, TEXTURESTAGESTATE_COLORARG1, TEXTURESTAGEARG_TEXTURE ); 
    SetGLTextureStageState( 0, TEXTURESTAGESTATE_COLORARG2, TEXTURESTAGEARG_DIFFUSE); 
    SetGLTextureStageState( 0, TEXTURESTAGESTATE_ALPHAOP, TEXTURESTAGEOP_SELECTARG2 ); 
    SetGLTextureStageState( 0, TEXTURESTAGESTATE_ALPHAARG1, TEXTURESTAGEARG_TEXTURE ); 
    SetGLTextureStageState( 0, TEXTURESTAGESTATE_ALPHAARG2, TEXTURESTAGEARG_DIFFUSE); 
    SetGLTextureStageState( 1, TEXTURESTAGESTATE_COLOROP, TEXTURESTAGEOP_DISABLE ); 
    SetGLTextureStageState( 1, TEXTURESTAGESTATE_ALPHAOP, TEXTURESTAGEOP_DISABLE ); 

	D3DCOLOR xcolor = D3DCOLOR_ARGB( 255, colorXR, colorXG, colorXB );
	D3DCOLOR ycolor = D3DCOLOR_ARGB( 255, colorYR, colorYG, colorYB );
	D3DCOLOR zcolor = D3DCOLOR_ARGB( 255, colorZR, colorZG, colorZB );

	vPoints[0].pos[0] = 0;
	vPoints[0].pos[1] = 0;
	vPoints[0].pos[2] = 0;
	vPoints[0].color = xcolor;

	vPoints[1].pos[0] = in_length;
	vPoints[1].pos[1] = 0;
	vPoints[1].pos[2] = 0;
	vPoints[1].color = xcolor;

	vPoints[2].pos[0] = 0;
	vPoints[2].pos[1] = 0;
	vPoints[2].pos[2] = 0;
	vPoints[2].color = ycolor;

	vPoints[3].pos[0] = 0;
	vPoints[3].pos[1] = in_length;
	vPoints[3].pos[2] = 0;
	vPoints[3].color = ycolor;

	vPoints[4].pos[0] = 0;
	vPoints[4].pos[1] = 0;
	vPoints[4].pos[2] = 0;
	vPoints[4].color = zcolor;

	vPoints[5].pos[0] = 0;
	vPoints[5].pos[1] = 0;
	vPoints[5].pos[2] = in_length;
	vPoints[5].color = zcolor;

	SetFVF( CGLRENDERER_2DVERTEX_FVF );
	SetPixelShader( NULL );
	SetVertexShader( NULL );
	SetBlendMode( BLEND_NORMAL );
	SetVertexStream( 0, NULL );
	SetIndices( NULL );
	SetDepthWrite( false );
	SetDepthTest( false );

	DrawPrimUp( D3DPT_LINELIST, 3,  vPoints,  sizeof(CGLRenderer_VertexRHW) );
	PopMatrix( WORLD_MATRIX );
#endif
}



void CGLRenderer::DrawSphere( Vec3 &position, float radius, int colorR, int colorG, int colorB, int iResolution )
{
#if 0
	int n;
	float a;
	float b;
	static Matrix4x4 matworld;
	
	PushMatrix( WORLD_MATRIX );
	matworld.SetIdentity();
	matworld.SetTranslation( position );
	SetMatrix( WORLD_MATRIX, (CONST FLOAT*)matworld.GetMatrix() );
	//Set color to derive from the vertex
    SetGLTextureStageState( 0, TEXTURESTAGESTATE_COLOROP, TEXTURESTAGEOP_SELECTARG2 ); 
    SetGLTextureStageState( 0, TEXTURESTAGESTATE_COLORARG1, TEXTURESTAGEARG_TEXTURE ); 
    SetGLTextureStageState( 0, TEXTURESTAGESTATE_COLORARG2, TEXTURESTAGEARG_DIFFUSE); 
    SetGLTextureStageState( 0, TEXTURESTAGESTATE_ALPHAOP, TEXTURESTAGEOP_SELECTARG2 ); 
    SetGLTextureStageState( 0, TEXTURESTAGESTATE_ALPHAARG1, TEXTURESTAGEARG_TEXTURE ); 
    SetGLTextureStageState( 0, TEXTURESTAGESTATE_ALPHAARG2, TEXTURESTAGEARG_DIFFUSE); 
    SetGLTextureStageState( 1, TEXTURESTAGESTATE_COLOROP, TEXTURESTAGEOP_DISABLE ); 
    SetGLTextureStageState( 1, TEXTURESTAGESTATE_ALPHAOP, TEXTURESTAGEOP_DISABLE ); 

	n = 0;
	const int space = iResolution;
	const int VertexCount = (180 / space ) * (360 / space) * 4;
	CGLRenderer_VertexRHW * verts = new CGLRenderer_VertexRHW[VertexCount];
	D3DCOLOR color = D3DCOLOR_ARGB( 150, colorR, colorG, colorB );
	for( b = 0; b <= 180 - space ; b+=space )
	{
		for( a = 0; a <= 360 - space; a+=space)
		{

			verts[n].pos[0] = radius * sin((a) / 180 * PI) * sin((b) / 180 * PI);
			verts[n].pos[1] = radius * cos((a) / 180 * PI) * sin((b) / 180 * PI);
			verts[n].pos[2] = radius * cos((b) / 180 * PI);
			verts[n].color = color;
			n++;

			verts[n].pos[0] = radius * sin((a) / 180 * PI) * sin((b + space) / 180 * PI);
			verts[n].pos[1] = radius * cos((a) / 180 * PI) * sin((b + space) / 180 * PI);
			verts[n].pos[2] = radius * cos((b + space) / 180 * PI);
			verts[n].color = color;
			n++;

			verts[n].pos[0] = radius * sin((a + space) / 180 * PI) * sin((b) / 180 * PI);
			verts[n].pos[1] = radius * cos((a + space) / 180 * PI) * sin((b) / 180 * PI);
			verts[n].pos[2] = radius * cos((b) / 180 * PI);
			verts[n].color = color;
			n++;

			verts[n].pos[0] = radius * sin((a + space) / 180 * PI) * sin((b + space) / 180 * PI);
			verts[n].pos[1] = radius * cos((a + space) / 180 * PI) * sin((b + space) / 180 * PI);
			verts[n].pos[2] = radius * cos((b + space) / 180 * PI);
			verts[n].color = color;
			n++;
		}
	}

	SetFVF( CGLRENDERER_2DVERTEX_FVF );
	SetPixelShader( NULL );
	SetVertexShader( NULL );
	SetBlendMode( BLEND_NORMAL );
	SetVertexStream( 0, NULL );
	SetIndices( NULL );

	{
		DrawPrimUp( D3DPT_LINESTRIP,
							VertexCount - 1, 
							verts, 
							sizeof(CGLRenderer_VertexRHW) );
	}
	delete[] verts;
#endif

}

void CGLRenderer::SetMatrix( const MATRIXMODE matmode, const float * pMat )
{
	switch( matmode )
	{
	case PROJECTION_MATRIX:
        {
		m_ProjectionMatrix.SetFrom4x4((float *)pMat);
        m_ProjectionMatrix.SetTranspose();
        glMatrixMode (GL_PROJECTION);
        glLoadMatrixf(m_ProjectionMatrix.GetMatrix());
        
		break;
		}
	case VIEW_MATRIX:
		{
		m_ModelViewMatrix.SetFrom4x4((float *)pMat);
        Matrix4x4 glModelViewMat;
        glModelViewMat = m_WorldMatrix * m_ModelViewMatrix;
        glModelViewMat.SetTranspose();
        glMatrixMode (GL_MODELVIEW);
        glLoadMatrixf(glModelViewMat.GetMatrix());
            
		break;
		}
	case WORLD_MATRIX:
		{
		m_WorldMatrix.SetFrom4x4((float *)pMat);
        Matrix4x4 glModelViewMat;
        glModelViewMat = m_WorldMatrix * m_ModelViewMatrix;
        glModelViewMat.SetTranspose();
        glMatrixMode (GL_MODELVIEW);
        glLoadMatrixf(glModelViewMat.GetMatrix());
        
		break;
		}
	default:
		break;
	};
    
    Matrix4x4 temp;
    temp = m_ModelViewMatrix  * m_ProjectionMatrix;
    temp = m_WorldMatrix * temp;
    temp.SetTranspose();
    SetVertexShaderConstantF( C_WORLD_TO_SCREEN, (const float*)temp.GetMatrix(), 4 );
	m_Ortho2DScreenSizeSet = false;
}

void CGLRenderer::PopMatrix( const MATRIXMODE matmode )
{
	switch( matmode )
	{
	case PROJECTION_MATRIX:
		{
			if( !m_MatrixStack[ PROJECTION_MATRIX ].empty() )
			{
				m_ProjectionMatrix = m_MatrixStack[ PROJECTION_MATRIX ].top();
				m_MatrixStack[ PROJECTION_MATRIX ].pop();
                
                glMatrixMode (GL_PROJECTION);
                glLoadMatrixf(m_ProjectionMatrix.GetMatrix());
			}
			break;
		}
	case VIEW_MATRIX:
		{
			if( !m_MatrixStack[ VIEW_MATRIX ].empty() )
			{
				m_ModelViewMatrix = m_MatrixStack[ VIEW_MATRIX ].top();
				m_MatrixStack[ VIEW_MATRIX ].pop();
                Matrix4x4 glModelViewMat;
                glModelViewMat = m_WorldMatrix * m_ModelViewMatrix;
                glMatrixMode (GL_MODELVIEW);
                glLoadMatrixf(glModelViewMat.GetMatrix());

			}
			break;
		}
	case WORLD_MATRIX:
		{
			if( !m_MatrixStack[ WORLD_MATRIX ].empty() )
			{
				m_WorldMatrix = m_MatrixStack[ WORLD_MATRIX ].top();
				m_MatrixStack[ WORLD_MATRIX ].pop();
                Matrix4x4 glModelViewMat;
                glModelViewMat = m_WorldMatrix * m_ModelViewMatrix;
                glMatrixMode (GL_MODELVIEW);
                glLoadMatrixf(glModelViewMat.GetMatrix());
			}
			break;
		}
	default:
		break;
	};
    
    Matrix4x4 temp;
    temp = m_ModelViewMatrix * m_ProjectionMatrix;
    temp = m_WorldMatrix * temp;
    temp.SetTranspose();
    SetVertexShaderConstantF( C_WORLD_TO_SCREEN, (const float*)temp.GetMatrix(), 4 );
	m_Ortho2DScreenSizeSet =false;

}

void CGLRenderer::PushMatrix( const MATRIXMODE matmode )
{

	switch( matmode )
	{
	case PROJECTION_MATRIX:
		{
			m_MatrixStack[ PROJECTION_MATRIX ].push( m_ProjectionMatrix );
			break;
		}
	case VIEW_MATRIX:
		{
			m_MatrixStack[ VIEW_MATRIX ].push( m_ModelViewMatrix );
			break;
		}
	case WORLD_MATRIX:
		{
			m_MatrixStack[ WORLD_MATRIX ].push( m_WorldMatrix );
			break;
		}
	default:
		break;
	};
	m_Ortho2DScreenSizeSet = false;

}
void CGLRenderer::GetMatrix( const MATRIXMODE matmode,  float * pMat )
{
	if( pMat )
	{
		switch( matmode )
		{
		case PROJECTION_MATRIX:
			memcpy( pMat, m_ProjectionMatrix.GetMatrix(), sizeof( m_ProjectionMatrix) );
			break;
		case VIEW_MATRIX:
			memcpy( pMat, m_ModelViewMatrix.GetMatrix(), sizeof( m_ModelViewMatrix) );
			break;
		case WORLD_MATRIX:
			memcpy( pMat, m_WorldMatrix.GetMatrix(), sizeof( m_WorldMatrix) );
			break;
		default:
			break;
		};
	}
}

void CGLRenderer::TransformScreenToWorld( float mouseX, float mouseY, Vec3 &Pos )
{
	///
	/// Need to add error checking for behind the camera points
	///
	int width, height;
	float GlobalTransform[ 16 ];
	float ProjectionMatrix[ 16 ];
	Vec3 vPos;
	GetViewDimensions( width, height );
	GetMatrix( PROJECTION_MATRIX, ProjectionMatrix );	
	GetMatrix( VIEW_MATRIX, GlobalTransform );
	//this following transform puts the mouse in normalized (0,1) coordinates then
	//maps it to the viewport
	//GL is LH, convert from Previous RH equation to LH equation
	vPos.x = ((2.0f * mouseX) - 1.f) / ProjectionMatrix[0];
	vPos.y =  ((2.0f * mouseY) - 1.f) / -ProjectionMatrix[5];
    vPos.z =  1.0f;
	//viewport -> world now(inverse world)
	Pos.x = vPos.x*GlobalTransform[0]+vPos.y*GlobalTransform[1]+vPos.z*GlobalTransform[2];
	Pos.y = vPos.x*GlobalTransform[4]+vPos.y*GlobalTransform[5]+vPos.z*GlobalTransform[6];
	Pos.z = vPos.x*GlobalTransform[8]+vPos.y*GlobalTransform[9]+vPos.z*GlobalTransform[10];
}

void CGLRenderer::TransformWorldToScreen( const Vec3 &pos, int &screenX, int &screenY, float &screenZ, IHashString *cameraName )
{
	Vec4 vPos;
	int width, height;
	GetViewDimensions( width, height );

	Matrix4x4 GlobalMatrix;
	Matrix4x4 ProjMatrix;
	
	static DWORD msgGetCameraProjectionMatrix = CHashString(_T("GetCameraProjectionMatrix")).GetUniqueID();
	static DWORD msgGetCameraViewMatrix = CHashString(_T("GetCameraViewMatrix")).GetUniqueID();
	static CHashString hsCameraType(_T("CCamera"));

	EngineGetToolBox()->SendMessage(msgGetCameraProjectionMatrix, sizeof(Matrix4x4), &ProjMatrix, cameraName, &hsCameraType);
	EngineGetToolBox()->SendMessage(msgGetCameraViewMatrix, sizeof(Matrix4x4), &GlobalMatrix, cameraName, &hsCameraType);

	vPos.Set( pos.x, pos.y, pos.z, 1.0f );
	vPos = GlobalMatrix * vPos;
	vPos = ProjMatrix * vPos;	
	vPos /= vPos.w; // divide by homogeneous coord

	screenX = (int)( (1.0f + vPos.x)/2.0f * width + 0.5f);
	screenY = (int)( (1.0f - vPos.y)/2.0f * height + 0.5f);
	screenZ = vPos.z; // used to tell if point is in front or behind screen
}

bool CGLRenderer::RenderToTexture( ITextureObject * pTexture )
{
	//try to set the texture to be the rendertexture
	if( pTexture )
	{
		m_RenderTarget = pTexture;
		return true;
	}
	else
	{
		m_RenderTarget = NULL;
	}
	return false;	
}

IHashString * CGLRenderer::GetComponentType() 
{
    static CHashString CRCT(_T("CGLRenderer"));
	return &CRCT;
}

bool CGLRenderer::IsKindOf(IHashString *compType)
{
	return (compType->GetUniqueID() == GetComponentType()->GetUniqueID());
}

void CGLRenderer::SetFullScreen()
{
	if( m_bInitialized )
	{
		//TODO:
	}
}

void CGLRenderer::GetFrameProfileInfo( RENDERERPROFILEINFO * info )
{
	if( info )
	{
		//info->TrianglesPerFrame = 
		//info->UsedTextureMemory = texmemcount;
	}
}

void CGLRenderer::SetBackgroundColor(UINT uRed, UINT uGreen, UINT uBlue, UINT Alpha)
{
	// Set Clear Color
	m_ClearColor[ 0 ] = Alpha;
	m_ClearColor[ 1 ] = uRed;
	m_ClearColor[ 2 ] = uGreen;
	m_ClearColor[ 3 ] = uBlue;

}

void CGLRenderer::SetPolygonZBias( const float fScale, const float fOffset )
{
    glPolygonOffset(fScale, fOffset);
}

void CGLRenderer::SetAlphaTest( bool state )
{
    if( state )
    {
        glEnable(GL_ALPHA_TEST);
    }else
    {
        glDisable(GL_ALPHA_TEST);
    }
	CheckAndLogGLError();
}
void CGLRenderer::SetDepthTest( bool state )
{
    if( state )
    {       
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }
	CheckAndLogGLError();
}
	
void CGLRenderer::SetDepthWrite( bool state )
{
    glDepthMask(state);
	CheckAndLogGLError();
}

void CGLRenderer::CaptureScreen( const TCHAR *filename )
{
	//TODO:
}

IVertexBufferObject * CGLRenderer::CreateVertexBuffer()
{
	IVertexBufferObject * res = (IVertexBufferObject*) new CGLVertexBufferObject();
	return res;
}

void CGLRenderer::DestroyVertexBuffer( IVertexBufferObject *pVertexBufferObject )
{
	SAFE_DELETE( pVertexBufferObject );
}

IIndexBuffer * CGLRenderer::CreateIndexBuffer()
{
	IIndexBuffer * res = (IIndexBuffer*) new CGLIndexBuffer();
	return res;
}

void CGLRenderer::DestroyIndexBuffer( IIndexBuffer *pIndexBufferObject )
{
	SAFE_DELETE( pIndexBufferObject );
}

 bool CGLRenderer::RenderVertexBuffer( IVertexBufferObject * vb,  const UINT offset, const UINT numElements,	
	 const DRAWPRIMITIVETYPE primtype)
{
    CGLVertexBufferObject * cvb = dynamic_cast< CGLVertexBufferObject * >( vb );
    if( !cvb )
    {
        return false;
    }
    // Render base pass
    //set the Vertex stream if it isn't valid, we test for redundancy!
    SetVertexStream( 0, cvb );
    SetFVF( NULL );
    //SetVertexDeclaration( cvb->GetVertexDeclaration() );
    FlushShaderConstants();
    
    if( primtype == DP_TRIANGLELIST )
    {
        //m_pDevice->DrawPrimitive( D3DPT_TRIANGLELIST, offset, numElements);
    }
    else if( primtype == DP_TRIANGLEFAN )
    {
        //m_pDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, offset, numElements);
    }
    else if( primtype == GL_TRIANGLE_STRIP )
    {
        //m_pDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, offset, numElements);
    }
    else if( primtype == DP_LINELIST )
    {
        //m_pDevice->DrawPrimitive( D3DPT_LINELIST, offset, numElements);
    }
    else if( primtype == DP_POINTLIST )
    {
        //m_pDevice->DrawPrimitive( D3DPT_POINTLIST, offset, numElements);
    }
    return true;
}

void CGLRenderer::SetPixelShader( GLenum pshader )
{
#if 0
	if( m_pDevice )
	{
		//check match
		//TODO: Broken right now because of how CGLPixelShaders are applying
		if( m_PixelShaderSet != pshader )
		{
			m_PixelShaderSet = pshader;
			m_pDevice->SetPixelShader( pshader );
		}
	}
#endif
}

void CGLRenderer::SetVertexShader( GLenum vshader )
{
#if 0
	if( m_pDevice )
	{
		//check match
		//TODO: Broken right now because of how CGLVertexShaders are applying
		if( m_VertexShaderSet != vshader )
		{
			m_VertexShaderSet = vshader;
			m_pDevice->SetVertexShader( vshader );
		}
	}
#endif
}
void CGLRenderer::SetVertexStream( UINT stream, CGLVertexBufferObject * cvb )
{
#if 0
	if( m_pDevice )
	{
		if( stream >= 0 && stream < 8 )
		{
			//check match
			if( m_VertexStreams[ stream ] != (BYTE*)cvb )
			{
				m_VertexStreams[ stream ] = (BYTE*)cvb;
				if( cvb ) 
				{
					m_pDevice->SetStreamSource( stream, cvb->GetGLInterface(), 0, cvb->GetStride() );
				}else
				{
					m_pDevice->SetStreamSource( stream, 0, 0, 0 );
				}

			}
		}
	}
#endif
}


void CGLRenderer::SetIndices( CGLIndexBuffer * cib )
{
#if 0
	if( m_pDevice )
	{
		//check match
		if( m_IndexStream != (BYTE*)cib )
		{
			m_IndexStream = (BYTE*)cib;
			if( cib )
			{
				m_pDevice->SetIndices( cib->GetGLInterface() );
			}else
			{
				m_pDevice->SetIndices( NULL );
			}
		}
	}
    
#endif
}
void CGLRenderer::SetTextureArray( IBaseTextureObject ** textures, const UINT numTextures )
{
	
	if( textures )
	{
		for( int i = 0; i < (int)numTextures; i++ )
		{
			if( textures[ i ] )
				SetTexture( i, textures[ i ] );
			else
			{
				//Log error, invalid texture
			}
		}
		
	}
}

 bool CGLRenderer::RenderIndexBuffer( IIndexBuffer * ib, IVertexBufferObject * vb,  const UINT offset, 
	 const UINT numElements, const DRAWPRIMITIVETYPE primtype )
{
#if 0
    if( m_pDevice )
	{
		CGLIndexBuffer * cib = dynamic_cast< CGLIndexBuffer * >(ib);
		CGLVertexBufferObject * cvb = dynamic_cast< CGLVertexBufferObject * >( vb );
		if( !cvb ||
			!cib )
			return false;

		// Render base pass
		//set the Vertex stream if it isn't valid, we test for redundancy!
		SetVertexStream( 0, cvb );
		SetIndices( cib );
		//SetVertexDeclaration( cvb->GetVertexDeclaration() );
		FlushShaderConstants();

		if( numElements > 0 )
		{
			if( primtype == DP_TRIANGLELIST )
			{
				m_pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, cvb->GetSize(), offset, numElements );
			}else if( primtype == DP_TRIANGLEFAN )
			{
				m_pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLEFAN, 0, 0, cvb->GetSize(), offset, numElements );
			}else if( primtype == DP_TRIANGLESTRIP )
			{
				m_pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP, 0, 0, cvb->GetSize(), offset, numElements );
			}else if( primtype == DP_LINELIST )
			{	
				m_pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP, 0, 0, cvb->GetSize(), offset, numElements );			
			}else if( primtype == DP_POINTLIST )
			{
				m_pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP, 0, 0, cvb->GetSize(), offset, numElements );
			}
		}
		return true;
	}
#endif
	return false;
}

 void CGLRenderer::FlushShaderConstants( void )
{
#if 0
	DWORD reg;
	float val[4];

	if (m_pDevice)
	{
		while( !m_VSConstantsChanged.empty() )
		{
			reg = m_VSConstantsChanged.top();
			val[0] = m_VSConstants[reg].vValue.GetX();
			val[1] = m_VSConstants[reg].vValue.GetY();
			val[2] = m_VSConstants[reg].vValue.GetZ();
			val[3] = m_VSConstants[reg].vValue.GetW();
			
			//SetVertexShaderConstantF( reg, val, 1 );
			m_pDevice->SetVertexShaderConstantF( reg, (const float *)val, 1 );
			m_VSConstants[reg].bChanged = false;
			m_VSConstantsChanged.pop();
		}

		while( !m_PSConstantsChanged.empty() )
		{
			reg = m_PSConstantsChanged.top();
			val[0] = m_PSConstants[reg].vValue.GetX();
			val[1] = m_PSConstants[reg].vValue.GetY();
			val[2] = m_PSConstants[reg].vValue.GetZ();
			val[3] = m_PSConstants[reg].vValue.GetW();
			
			//SetPixelShaderConstantF( reg, val, 1 );
			m_pDevice->SetPixelShaderConstantF( reg, (const float *)val, 1 );
			m_PSConstants[reg].bChanged = false;
			m_PSConstantsChanged.pop();
		}
	}
#endif
}

//Internal function since it's used in a lot of places
void CGLRenderer::SetTextureStates( bool vertexlit )
{
#if 0
	SetGLTextureStageState( 0, TEXTURESTAGESTATE_COLORARG1, TEXTURESTAGEARG_TEXTURE );
	SetGLTextureStageState( 0, TEXTURESTAGESTATE_COLORARG2, TEXTURESTAGEARG_DIFFUSE);
	SetGLTextureStageState( 0, TEXTURESTAGESTATE_ALPHAARG1, TEXTURESTAGEARG_TEXTURE );
	SetGLTextureStageState( 0, TEXTURESTAGESTATE_ALPHAARG2, TEXTURESTAGEARG_DIFFUSE);
	SetGLTextureStageState( 1, TEXTURESTAGESTATE_COLOROP, TEXTURESTAGEOP_DISABLE );
	SetGLTextureStageState( 1, TEXTURESTAGESTATE_ALPHAOP, TEXTURESTAGEOP_DISABLE );
	if( !vertexlit )
	{
		SetGLTextureStageState( 0, TEXTURESTAGESTATE_ALPHAOP, TEXTURESTAGEOP_MODULATE );
		SetGLTextureStageState( 0, TEXTURESTAGESTATE_COLOROP, TEXTURESTAGEOP_MODULATE );		
	}else
	{//vertex only color
		SetGLTextureStageState( 0, TEXTURESTAGESTATE_ALPHAOP, TEXTURESTAGEOP_SELECTARG2 );
		SetGLTextureStageState( 0, TEXTURESTAGESTATE_COLOROP, TEXTURESTAGEOP_SELECTARG2 );	
	}
#endif
}

/// \brief this should be the main and only way to draw indexbuffers
bool CGLRenderer::RenderIndexBuffer( IIndexBuffer * ib, IVertexBufferObject * vb, 
	const UINT basevertex, const UINT numverticesused,
	const UINT offset, const UINT numElements,
	 const DRAWPRIMITIVETYPE primtype )
{
#if 0
	CGLIndexBuffer * cib = NULL;
	CGLVertexBufferObject * cvb = NULL;
    if( m_pDevice )
	{
		
		//Grab index buffer interface
		cib = dynamic_cast< CGLIndexBuffer * >( ib );
		cvb = dynamic_cast< CGLVertexBufferObject * >( vb );
		if( !cvb ||
			!cib )
		{
			return false;
		}
		// Render base pass
		//set the Vertex stream if it isn't valid, we test for redundancy!
		SetIndices( cib );
		SetVertexStream( 0, cvb );
		SetFVF( NULL );
		//dx 9 only, no FVFs required, they actually can conflict with Decls
		//SetVertexDeclaration( cvb->GetVertexDeclaration() );
		FlushShaderConstants();
		if( numElements > 0 )
		{
			//Index bufferes are relative to base vertex in this scenario, this is optimal for rendering
			if( primtype == DP_TRIANGLELIST )
			{
				m_pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, basevertex, 0, numverticesused,offset, numElements );
			}else if( primtype == DP_TRIANGLEFAN )
			{
				m_pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLEFAN, basevertex, 0, numverticesused,offset, numElements );
			}else if( primtype == DP_TRIANGLESTRIP )
			{
				m_pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP, basevertex, 0, numverticesused,offset, numElements );
			}else if( primtype == DP_LINELIST )
			{
				m_pDevice->DrawIndexedPrimitive( D3DPT_LINELIST, basevertex, 0, numverticesused,offset, numElements );
			}else if( primtype == DP_POINTLIST )
			{
				m_pDevice->DrawIndexedPrimitive( D3DPT_POINTLIST, basevertex, 0, numverticesused,offset, numElements );
			}else
			{
				return false;
			}
			return true;			
		}
	}
#endif
	return false;
}

void CGLRenderer::UnsetTextures()
{
	if (m_SetTextures)
	{
		for (UINT stage=0; stage < m_iMaxTextures; stage++)
		{
			if (m_SetTextures[stage] != NULL)
			{
                glActiveTexture(GL_TEXTURE0 + stage);
                glBindTexture(GL_TEXTURE_2D, 0);
				m_SetTextures[stage] = NULL;
			}
		}
	}
}

bool CGLRenderer::SetTexture( UINT stage, IBaseTextureObject * texture )
{
	//TODO: check for redundancies here!
	if (stage >= m_iMaxTextures)
		return false;
	if( texture != m_SetTextures[ stage ] )
	{
		m_SetTextures[ stage ] = texture;
		if( texture )
		{
            glActiveTexture(GL_TEXTURE0 + stage);
            GLenum glTextureType = GL_TEXTURE_2D;
            switch (texture->GetTextureType())
            {
                case EE_TEXTUREANIMATED2D:
                case EE_TEXTURE2D:
                    glTextureType = GL_TEXTURE_2D;
                    break;
                case EE_TEXTURECUBE:
                    glTextureType = GL_TEXTURE_CUBE_MAP;
                    break;
            }
            void *apiTexture = texture->GetAPITexture();
            glBindTexture(glTextureType, *((GLuint *)(&apiTexture)));
		}else
		{
            glActiveTexture(GL_TEXTURE0 + stage);
            glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
	return true;
}

bool CGLRenderer::SetVertexTexture( UINT stage, IBaseTextureObject * texture )
{
#if 0
	//TODO: check for redundancies here!
	if (stage >= m_iMaxTextures)
		return false;
	if( texture != m_SetVertexTextures[ stage ] )
	{
		m_SetVertexTextures[ stage ] = texture;
		if( texture )
		{
       		m_pDevice->SetTexture( D3DVERTEXTEXTURESAMPLER0+stage, ( IDirect3DBaseTexture9 *  )texture->GetAPITexture() );
			m_pDevice->SetSamplerState(D3DVERTEXTEXTURESAMPLER0+stage, D3DSAMP_MINFILTER, D3DTEXF_POINT);
			m_pDevice->SetSamplerState(D3DVERTEXTEXTURESAMPLER0+stage, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
			m_pDevice->SetSamplerState(D3DVERTEXTEXTURESAMPLER0+stage, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
			m_pDevice->SetSamplerState(D3DVERTEXTEXTURESAMPLER0+stage, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
			m_pDevice->SetSamplerState(D3DVERTEXTEXTURESAMPLER0+stage, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
		}
		else
		{
			m_pDevice->SetTexture( D3DVERTEXTEXTURESAMPLER0+stage, NULL );
		}
	}
#endif
	return true;
}


//must set textures before this
bool CGLRenderer::DrawQuad( void * verts, CHANNELDESCRIPTORLIST &channels )
{
    SetIndices(NULL);
    SetVertexStream(0, NULL);
    //SetVertexDeclaration( vdecl );
    FlushShaderConstants();
    //Calculate requested stride:
    int totalstride =0;
    //calculate strides
    for( int i = 0; i < (int)channels.size(); i++ )
    {
        ChannelDesc * curStream = &channels[ i ];
        totalstride += curStream->Stride;
    }
    //make sure you have shaders and textures set before this or it wont' render of course!
    DrawPrimUp(GL_TRIANGLE_STRIP, 2, verts, totalstride);
    return true;
}

bool CGLRenderer::Draw2DQuadNoTex( float x, float y,  float width, float height, 
							 DWORD color, float angle, 
							 float uStart, float vStart, 
							 float uEnd, float vEnd)
{
	SetRenderState( RENDERSTATE_CULLMODE, RENDERSTATEPARAM_CULLNONE );
	static CGLRenderer_VertexRHW	sQuad[ 4 ] = 
	{
		{0.f,0.f,0.f,0xFFFFFFFF,0.f,0.f},
		{0.f,0.f,0.f,0xFFFFFFFF,0.f,1.f},
		{0.f,0.f,0.f,0xFFFFFFFF,1.f,0.f},
		{0.f,0.f,0.f,0xFFFFFFFF,1.f,1.f}
	};
    //RHW is in screen coords
	sQuad[ 0 ].pos[ 0 ] = -0.50000f * width;
	sQuad[ 0 ].pos[ 1 ] = -0.50000f * height;
	//sQuad[ 0 ].pos[ 2 ] = z;
	sQuad[ 0 ].color = color;

	sQuad[ 0 ].tu = uStart;
	sQuad[ 0 ].tv = vStart;

	sQuad[ 2 ].pos[ 0 ] = 0.50000f * width;
	sQuad[ 2].pos[ 1 ] = -0.50000f * height;
	//sQuad[ 2 ].pos[ 2 ] = z;
	sQuad[ 2 ].color = color;

	sQuad[ 2 ].tu = uEnd;
	sQuad[ 2 ].tv = vStart;

	sQuad[ 1 ].pos[ 0 ] = -0.50000f * width;
	sQuad[ 1 ].pos[ 1 ] = 0.50000f * height;
	//sQuad[ 1 ].pos[ 2 ] = z;
	sQuad[ 1 ].color = color;
	
	sQuad[ 1 ].tu = uStart;
	sQuad[ 1 ].tv = vEnd;

	sQuad[ 3 ].pos[ 0 ] = 0.50000f * width;
	sQuad[ 3  ].pos[ 1 ] = 0.50000f * height;
	//sQuad[ 3 ].pos[ 2 ] = z;
	sQuad[ 3 ].color = color;
	
	sQuad[ 3 ].tu = uEnd;
	sQuad[ 3 ].tv = vEnd;

    //set it up and render

    //this function checks for redundancies
    PushMatrix( PROJECTION_MATRIX );
    PushMatrix( WORLD_MATRIX );
    PushMatrix( VIEW_MATRIX );

    Matrix4x4 mat;
    MathUtil math;
    EulerAngle rot(0.f, 0.f, angle);//math.DegToRad(angle));
    mat.SetRotation(rot);
    Vec3 trans(x + (0.5f * width), y + (0.5f * height), 0.f);
    mat.SetTranslation(trans);				
    SetOrtho2DScreenSize();
    SetMatrix(WORLD_MATRIX, mat.m);
    
    SetFVF( CGLRENDERER_2DVERTEX_FVF );

    // disable vertex stream for fixed function pipeline
    SetVertexStream( 0, NULL );
    //drawprimup if for some reason the VB is not allocated
    DrawPrimUp( GL_TRIANGLE_STRIP, 2, (void*)sQuad, sizeof( CGLRenderer_VertexRHW ) );
        
    PopMatrix( PROJECTION_MATRIX );
    PopMatrix( WORLD_MATRIX );
    PopMatrix( VIEW_MATRIX );
			
    return true;
}

bool CGLRenderer::Draw2DQuad( float x, float y,  float width, float height, 
							 IBaseTextureObject * texture, 
							 DWORD color, float angle, 
							 float uStart, float vStart, 
							 float uEnd, float vEnd)
{
	
	SetRenderState( RENDERSTATE_CULLMODE, RENDERSTATEPARAM_CULLNONE );
	static CGLRenderer_VertexRHW	sQuad[ 4 ] = 
	{
		{0.f,0.f,0.f,0xFFFFFFFF,0.f,0.f},
		{0.f,0.f,0.f,0xFFFFFFFF,0.f,1.f},
		{0.f,0.f,0.f,0xFFFFFFFF,1.f,0.f},
		{0.f,0.f,0.f,0xFFFFFFFF,1.f,1.f}
	};
    //RHW is in screen coords
	sQuad[ 0 ].pos[ 0 ] = -0.50000f * width;
	sQuad[ 0 ].pos[ 1 ] = -0.50000f * height;
	//sQuad[ 0 ].pos[ 2 ] = z;
	sQuad[ 0 ].color = color;

	sQuad[ 0 ].tu = uStart;
	sQuad[ 0 ].tv = vStart;

	sQuad[ 2 ].pos[ 0 ] = 0.50000f * width;
	sQuad[ 2].pos[ 1 ] = -0.50000f * height;
	//sQuad[ 2 ].pos[ 2 ] = z;
	sQuad[ 2 ].color = color;

	sQuad[ 2 ].tu = uEnd;
	sQuad[ 2 ].tv = vStart;

	sQuad[ 1 ].pos[ 0 ] = -0.50000f * width;
	sQuad[ 1 ].pos[ 1 ] = 0.50000f * height;
	//sQuad[ 1 ].pos[ 2 ] = z;
	sQuad[ 1 ].color = color;
	
	sQuad[ 1 ].tu = uStart;
	sQuad[ 1 ].tv = vEnd;

	sQuad[ 3 ].pos[ 0 ] = 0.50000f * width;
	sQuad[ 3  ].pos[ 1 ] = 0.50000f * height;
	//sQuad[ 3 ].pos[ 2 ] = z;
	sQuad[ 3 ].color = color;
	
	sQuad[ 3 ].tu = uEnd;
	sQuad[ 3 ].tv = vEnd;

	IBaseTextureObject * GLtex = (IBaseTextureObject*)(texture);
    
    //set it up and render

    //this function checks for redundancies
    PushMatrix( PROJECTION_MATRIX );
    PushMatrix( WORLD_MATRIX );
    PushMatrix( VIEW_MATRIX );

    Matrix4x4 mat;
    MathUtil math;
    EulerAngle rot(0.f, 0.f, angle);//math.DegToRad(angle));
    mat.SetRotation(rot);
    Vec3 trans(x + (0.5f * width), y + (0.5f * height), 0.f);
    mat.SetTranslation(trans);				
    SetOrtho2DScreenSize();
    SetMatrix(WORLD_MATRIX, mat.m);

    if ( GLtex )
    {
        SetTexture( 0, GLtex );
        SetTexture( 1, NULL );

        SetGLTextureStageState( 0, TEXTURESTAGESTATE_COLOROP, TEXTURESTAGEOP_MODULATE);
        SetGLTextureStageState( 0, TEXTURESTAGESTATE_COLORARG1, TEXTURESTAGEARG_TEXTURE );
        SetGLTextureStageState( 0, TEXTURESTAGESTATE_COLORARG2, TEXTURESTAGEARG_DIFFUSE );
        SetGLTextureStageState( 0, TEXTURESTAGESTATE_ALPHAOP, TEXTURESTAGEOP_SELECTARG2);
        SetGLTextureStageState( 0, TEXTURESTAGESTATE_ALPHAARG1, TEXTURESTAGEARG_TEXTURE );
        SetGLTextureStageState( 0, TEXTURESTAGESTATE_ALPHAARG2, TEXTURESTAGEARG_DIFFUSE );
        SetGLTextureStageState( 1, TEXTURESTAGESTATE_COLOROP, TEXTURESTAGEOP_DISABLE ); 
        SetGLTextureStageState( 1, TEXTURESTAGESTATE_ALPHAOP, TEXTURESTAGEOP_DISABLE );
    }
    else
    { 
        SetTexture( 0, NULL ); 
        SetGLTextureStageState( 0, TEXTURESTAGESTATE_COLOROP, TEXTURESTAGEOP_SELECTARG2 ); 
        SetGLTextureStageState( 0, TEXTURESTAGESTATE_COLORARG2, TEXTURESTAGEARG_DIFFUSE ); 
        SetGLTextureStageState( 0, TEXTURESTAGESTATE_ALPHAOP, TEXTURESTAGEOP_SELECTARG2 ); 
        SetGLTextureStageState( 1, TEXTURESTAGESTATE_COLOROP, TEXTURESTAGEOP_DISABLE ); 
        SetGLTextureStageState( 1, TEXTURESTAGESTATE_ALPHAOP, TEXTURESTAGEOP_DISABLE ); 
    }
    
    SetFVF( CGLRENDERER_2DVERTEX_FVF );
    
    SetVertexStream( 0, NULL );
    //drawprimup if for some reason the VB is not allocated
    DrawPrimUp( GL_TRIANGLE_STRIP, 2, (void*)sQuad, sizeof( CGLRenderer_VertexRHW ) );
    
    
    PopMatrix( PROJECTION_MATRIX );
    PopMatrix( WORLD_MATRIX );
    PopMatrix( VIEW_MATRIX );
        
    return true;
}

bool CGLRenderer::DrawFullscreenQuad(float left, float right, float bottom, float top)
{

	SetRenderState( RENDERSTATE_CULLMODE, RENDERSTATEPARAM_CULLNONE );

	//this function checks for redundancies
	PushMatrix( PROJECTION_MATRIX );
	PushMatrix( WORLD_MATRIX );
	PushMatrix( VIEW_MATRIX );

	Matrix4x4 identityMatrix;
	identityMatrix.SetIdentity();
	SetMatrix( WORLD_MATRIX, identityMatrix.GetMatrix() );
	SetMatrix( VIEW_MATRIX, identityMatrix.GetMatrix() );
	SetOrtho2DScreenSize( left, right, bottom, top );

	if( m_FullscreenQuadVertexBuffer )
	{
		RenderVertexBuffer(
			m_FullscreenQuadVertexBuffer, 
			m_FullscreenQuadVBAllocation.m_Offset, 
			m_FullscreenQuadVBAllocation.m_Size - 2, // number of primitives
			DP_TRIANGLESTRIP );
	}
	PopMatrix( PROJECTION_MATRIX );
	PopMatrix( WORLD_MATRIX );
	PopMatrix( VIEW_MATRIX );

	return true;
}

UINT GetNumVerticesFromPrim(GLenum prim, UINT numprim)
{
    UINT retPrim = 0;
    switch (prim)
    {
        case GL_LINE_LOOP:
        case GL_POINTS:
            retPrim = numprim;
            break;
        case GL_LINES:
            retPrim = numprim * 2;
            break;
        case GL_LINE_STRIP:
            retPrim = numprim + 1;
            break;
        case GL_TRIANGLE_FAN:
        case GL_TRIANGLE_STRIP:
            retPrim = numprim + 2;
            break;
        case GL_QUADS:
            retPrim = numprim * 4;
            break;
        case GL_QUAD_STRIP:
            retPrim = (numprim * 2) + 2;
            break;
        default:
        {
            EngineGetToolBox()->Log(LOGERROR, _T("Unknown primitive type 0x%x"), prim);
        }
    }
    
    return retPrim;
}

void CGLRenderer::DrawPrimUp ( GLenum prim, UINT numprim, void * stream, UINT stride )
{
    glBegin(prim);
    
    UINT nVerts = GetNumVerticesFromPrim(prim, numprim);
    
    for (int i=0; i<nVerts; i++)
    {
        if (m_FVFSet & GLFVF_XYZ)
        {
            float *fValues = (float *)stream;
            glVertex3f( *fValues++, *fValues++, *fValues++);
            stream = (void *)fValues;
        }
        
        if (m_FVFSet & GLFVF_DIFFUSE)
        {
            DWORD *dValue = (DWORD *)stream;
            DWORD diffuseColor = *dValue++;
            stream = (void *)dValue;
            glColor4b(((diffuseColor >> 16) & 0xff), ((diffuseColor >> 8) & 0xff), ((diffuseColor >> 0) & 0xff), ((diffuseColor >> 24) & 0xff));
        }
        
        if (m_FVFSet & GLFVF_TEX1)
        {
            float *fValues = (float *)stream;
            glTexCoord2f(*fValues++, *fValues++);
            stream = (void *)fValues;
        }
    }
    
    glEnd();
    
	//drawprim up kills this internally, must tell renderer that it is now nul
	m_VertexDeclSet = NULL;
	SetFVF( 0 );
	m_VertexStreams[ 0 ] = NULL;
}

void CGLRenderer::SetFVF( DWORD fvf )
{
	if( m_FVFSet != fvf )
	{
		//m_pDevice->SetFVF( fvf );
		m_FVFSet = fvf;
	}
}

void CGLRenderer::SetWireframeMode( bool val )
{
    if( val )
    {
        m_bWireFrameMode = true;
        SetGLRenderState( RENDERSTATE_FILLMODE, RENDERSTATEPARAM_WIREFRAME );
    }
    else
    {
        m_bWireFrameMode = false;
        SetGLRenderState( RENDERSTATE_FILLMODE, RENDERSTATEPARAM_SOLID  );
    }
}

void CGLRenderer::SetBlendMode( BLENDMODE mode )
{
    if( mode == BLEND_NORMAL )
    {
        SetGLRenderState(RENDERSTATE_SRCBLEND,RENDERSTATEPARAM_SRCALPHA);
        SetGLRenderState(RENDERSTATE_DESTBLEND,RENDERSTATEPARAM_INVSRCALPHA);
    }
    else
    if( mode == BLEND_ADDITIVE )
    {
        SetGLRenderState(RENDERSTATE_SRCBLEND,RENDERSTATEPARAM_SRCALPHA);
        SetGLRenderState(RENDERSTATE_DESTBLEND,RENDERSTATEPARAM_ONE);
    }
}

#if 0
LPDIRECT3DVERTEXDECLARATION9 CGLRenderer::GetVertexDescription( CHANNELDESCRIPTORLIST &channels, UINT streammode )
{
	//calculate FVF:
	//Calculate requested stride:
	static CHashString	vp3(_T("MESH_STREAM_VERTEX_POSITION3"));
	static CHashString	vprhw(_T("MESH_STREAM_VERTEX_POSITIONRHW"));
	static CHashString	tc1(_T("MESH_STREAM_VERTEX_TEXTURECOORD1"));	
	static CHashString	tc2(_T("MESH_STREAM_VERTEX_TEXTURECOORD2"));	
	static CHashString	tc3(_T("MESH_STREAM_VERTEX_TEXTURECOORD3"));	
	static CHashString	tc4(_T("MESH_STREAM_VERTEX_TEXTURECOORD4"));
	static CHashString	cc1(_T("MESH_STREAM_VERTEX_CUBECOORD1"));
	static CHashString	cc2(_T("MESH_STREAM_VERTEX_CUBECOORD2"));
	static CHashString	cc3(_T("MESH_STREAM_VERTEX_CUBECOORD3"));
	static CHashString	cc4(_T("MESH_STREAM_VERTEX_CUBECOORD4"));
	static CHashString  c4(_T("MESH_STREAM_VERTEX_COLORCRGBA"));
	static CHashString  s4(_T("MESH_STREAM_VERTEX_SPECULARCRGBA"));
	static CHashString	n3(_T("MESH_STREAM_VERTEX_NORMAL"));
	static CHashString	t3(_T("MESH_STREAM_VERTEX_TANGENT3"));
	static CHashString	t4(_T("MESH_STREAM_VERTEX_TANGENT4"));
	static CHashString	boneweights(_T("MESH_STREAM_WEIGHTS"));
	static CHashString	boneindices(_T("MESH_STREAM_MATRIX_INDICES"));
	static const D3DVERTEXELEMENT9 vPos = { 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 };
	static const D3DVERTEXELEMENT9 vRhw = { 0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITIONT, 0 };
	static const D3DVERTEXELEMENT9 vTexcoord0 = { 0, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 };
	static const D3DVERTEXELEMENT9 vCubeCoord0 = { 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 };
	static const D3DVERTEXELEMENT9 vDiffuse = { 0, 0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 };
	static const D3DVERTEXELEMENT9 vSpecular = { 0, 0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 1 };
	static const D3DVERTEXELEMENT9 vNormal = { 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 };
	static const D3DVERTEXELEMENT9 vTangent3 = { 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 };
	static const D3DVERTEXELEMENT9 vTangent4 = { 0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 };
	static const D3DVERTEXELEMENT9 vBoneWeight = { 0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0 };
	static const D3DVERTEXELEMENT9 vBoneIndex = { 0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0 };
	static const D3DVERTEXELEMENT9 vEnd = D3DDECL_END();

	//make sure we reset texcoord usage since these are static

	if( m_pDevice )
	{
		LPDIRECT3DVERTEXDECLARATION9	VertexDeclaration = NULL;
		//find the descriptor:
		//check to see if this vertex declaration was already created
		VERTEXDECLARATIONMAP::iterator iter = m_VertexDeclarationList.find( channels );
		if( iter != m_VertexDeclarationList.end() )
		{
			//got a vertex declaration
			VertexDeclaration = (*iter).second;
			VertexDeclaration->AddRef(); // add a reference to the object so it isn't released early
			return VertexDeclaration;
		}
		//None found, create a new declaration
		//Compute FVF and Vertex Declaration simultaneously
		int size = channels.size();
		int curstride = 0;
		D3DVERTEXELEMENT9 * VertConst = new D3DVERTEXELEMENT9[ size + 1];
		for( int i = 0; i < (int)size; i++ )
		{
			ChannelDesc * curStream = &channels[ i ];
			if(curStream->ChannelHash ==  vp3.GetUniqueID() )
			{
				//Position offset
				VertConst[ i ] = vPos;
				VertConst[ i ].Offset = curstride;
			}
			else if(curStream->ChannelHash ==  vprhw.GetUniqueID() )
			{
				//Position offset
				VertConst[ i ] = vRhw;
				VertConst[ i ].Offset = curstride;
			}
			else
			if( curStream->ChannelHash ==  tc1.GetUniqueID() )
			{
				//texture coord 1 offset
				VertConst[ i ] = vTexcoord0;
				VertConst[ i ].Offset = curstride;
				VertConst[ i ].UsageIndex = 0;//increment index usage
			}
			else if( curStream->ChannelHash ==  tc2.GetUniqueID() )
			{
				//texture coord 1 offset
				VertConst[ i ] = vTexcoord0;
				VertConst[ i ].Offset = curstride;
				VertConst[ i ].UsageIndex = 1;//increment index usage
			}
			else if( curStream->ChannelHash ==  tc3.GetUniqueID() )
			{
				//texture coord 1 offset
				VertConst[ i ] = vTexcoord0;
				VertConst[ i ].Offset = curstride;
				VertConst[ i ].UsageIndex = 2;//increment index usage
			}
			else if( curStream->ChannelHash ==  tc4.GetUniqueID() )
			{
				//texture coord 1 offset
				VertConst[ i ] = vTexcoord0;
				VertConst[ i ].Offset = curstride;
				VertConst[ i ].UsageIndex = 3;//increment index usage
			}else
			if( curStream->ChannelHash ==  cc1.GetUniqueID() )
			{
				//texture coord 1 offset
				VertConst[ i ] = vCubeCoord0;
				VertConst[ i ].Offset = curstride;
				VertConst[ i ].UsageIndex = 0;//increment index usage
			}
			else if( curStream->ChannelHash ==  cc2.GetUniqueID() )
			{
				//texture coord 1 offset
				VertConst[ i ] = vCubeCoord0;
				VertConst[ i ].Offset = curstride;
				VertConst[ i ].UsageIndex = 1;//increment index usage
			}
			else if( curStream->ChannelHash ==  cc3.GetUniqueID() )
			{
				//texture coord 1 offset
				VertConst[ i ] = vCubeCoord0;
				VertConst[ i ].Offset = curstride;
				VertConst[ i ].UsageIndex = 2;//increment index usage
			}
			else if( curStream->ChannelHash ==  cc4.GetUniqueID() )
			{
				//texture coord 1 offset
				VertConst[ i ] = vCubeCoord0;
				VertConst[ i ].Offset = curstride;
				VertConst[ i ].UsageIndex = 3;//increment index usage
			}
			else
			if( curStream->ChannelHash ==  n3.GetUniqueID() )
			{
				//normal offset
				VertConst[ i ] = vNormal;
				VertConst[ i ].Offset = curstride;
			}
			else
			if( curStream->ChannelHash ==  t3.GetUniqueID() )
			{
				//normal offset
				VertConst[ i ] = vTangent3;
				VertConst[ i ].Offset = curstride;
			}
			else
			if( curStream->ChannelHash ==  t4.GetUniqueID() )
			{
				//normal offset
				VertConst[ i ] = vTangent4;
				VertConst[ i ].Offset = curstride;
			}
			else
			if( curStream->ChannelHash ==  c4.GetUniqueID() )
			{
				//normal offset
				VertConst[ i ] = vDiffuse;
				VertConst[ i ].Offset = curstride;
			}else
			if( curStream->ChannelHash ==  s4.GetUniqueID() )
			{
				//normal offset
				VertConst[ i ] = vSpecular;
				VertConst[ i ].Offset = curstride;
			}else 
			if( curStream->ChannelHash == boneweights.GetUniqueID() )
			{
				VertConst[ i ] = vBoneWeight;
				VertConst[ i ].Offset = curstride;
				assert( curStream->Stride == 16 );
			}
			else 
			if( curStream->ChannelHash == boneindices.GetUniqueID() )
			{
				VertConst[ i ] = vBoneIndex;
				VertConst[ i ].Offset = curstride;
				assert( curStream->Stride == 16 );
			}
			else//something not used, set to no FVF, might be a custom vertex thing
			{
				delete VertConst;
				return NULL;
			}
			if( streammode != 0 )
			{
				VertConst[ i ].Stream = streammode;
			}
			//temporary hack until streams are integrated into vertex channel descriptions
			if( curStream->Stride == 0 )
			{
				VertConst[ i ].Stream = 1;
			}
			curstride += curStream->Stride;	
		}
		VertConst[ size ] = vEnd;
		if( FAILED( m_pDevice->CreateVertexDeclaration( VertConst, &VertexDeclaration )))
		{
			VertexDeclaration = NULL;
		}
		if( VertexDeclaration )
		{
			m_VertexDeclarationList.insert( VERTEXDECLARATIONMAP_PAIR( channels, VertexDeclaration ) );
			VertexDeclaration->AddRef(); // add a reference count for the renderer's list
		}
		delete VertConst;
		return VertexDeclaration;
	}
	return NULL;
};
#endif

DWORD CGLRenderer::GetVertexFVF( CHANNELDESCRIPTORLIST &channels )
{
    return 0;
#if 0
	//calculate FVF:
	//Calculate requested stride:
	static CHashString	vp3(_T("MESH_STREAM_VERTEX_POSITION3"));
	static CHashString	tc1(_T("MESH_STREAM_VERTEX_TEXTURECOORD1"));	
	static CHashString	tc2(_T("MESH_STREAM_VERTEX_TEXTURECOORD2"));	
	static CHashString	tc3(_T("MESH_STREAM_VERTEX_TEXTURECOORD3"));	
	static CHashString	tc4(_T("MESH_STREAM_VERTEX_TEXTURECOORD4"));
	static CHashString  c4(_T("MESH_STREAM_VERTEX_COLORCRGBA"));
	static CHashString	n3(_T("MESH_STREAM_VERTEX_NORMAL"));
	static CHashString	t3(_T("MESH_STREAM_VERTEX_TANGENT"));
	static CHashString	boneweights(_T("MESH_STREAM_WEIGHTS"));
	static CHashString	boneindices(_T("MESH_STREAM_MATRIX_INDICES"));
	
	//make sure we reset texcoord usage since these are static

	DWORD FvF = 0;
	if( m_pDevice )
	{
		//find the descriptor:
		//check to see if this vertex declaration was already created
		VERTEXFVFMAP::iterator iter = m_VertexFvFList.find( channels );
		if( iter != m_VertexFvFList.end() )
		{
			//got a vertex declaration
			FvF= (*iter).second;
			return FvF;
		}
		//None found, create a new declaration
		//Compute FVF
		int size = channels.size();
		int curstride = 0;
		for( int i = 0; i < (int)size; i++ )
		{
			ChannelDesc * curStream = &channels[ i ];
			if(curStream->ChannelHash ==  vp3.GetUniqueID() )
			{
				//Position offset
				FvF |= D3DFVF_XYZ;
			}
			else
			if( curStream->ChannelHash ==  tc1.GetUniqueID() )
			{
				FvF |= D3DFVF_TEX1;
			}else
			if( curStream->ChannelHash ==  tc2.GetUniqueID())
			{
				FvF |= D3DFVF_TEX2;
			}else
			if( curStream->ChannelHash ==  tc3.GetUniqueID())
			{
				FvF |= D3DFVF_TEX3;
			}else
			if( curStream->ChannelHash ==  tc4.GetUniqueID())
			{
				FvF |= D3DFVF_TEX4;
			}
			else
			if( curStream->ChannelHash ==  n3.GetUniqueID() )
			{
				FvF |= D3DFVF_NORMAL;
			}else
			if( curStream->ChannelHash ==  t3.GetUniqueID() )
			{
				//FvF |= D3DFVF_TANGENT;
			}
			else
			if( curStream->ChannelHash ==  c4.GetUniqueID() )
			{
				FvF |= D3DFVF_DIFFUSE;
			}else 
			if( curStream->ChannelHash == boneweights.GetUniqueID() )
			{
				FvF |= D3DFVF_XYZB4;
				FvF &= ~D3DFVF_XYZB5;
				FvF &= ~D3DFVF_LASTBETA_UBYTE4;
			}
			else 
			if( curStream->ChannelHash == boneindices.GetUniqueID() )
			{
				FvF |= D3DFVF_XYZB5;
				FvF |= D3DFVF_LASTBETA_UBYTE4;
				FvF &= ~D3DFVF_XYZB4;
			}
			else//something not used, set to no FVF, might be a custom vertex thing
			{
				return 0;
			}
			curstride += curStream->Stride;		
		}
		m_VertexFvFList.insert( VERTEXFVFMAP_PAIR( channels, FvF ) );
		return FvF;
	}
#endif
	return NULL;
};

void MatrixOrthoOffCenterLH(Matrix4x4 *mat, float left, float right, float bottom, float top, float nearZ, float farZ)
{

    mat->SetIdentity();
    mat->m[0] = 2.0f / (right-left);
    mat->m[5] = 2.0f / (top-bottom);
    mat->m[10] = 1.0f / (nearZ-farZ);
    
    mat->m[12] = (left + right) / (left - right);
    mat->m[13] = (top + bottom) / (bottom - top);
    mat->m[14] = nearZ / (nearZ - farZ);
}

void CGLRenderer::SetOrtho2DScreenSize(float left, float right, float bottom, float top)
{
	if( !m_Ortho2DScreenSizeSet )
	{
		Matrix4x4 mat;
		Matrix4x4 mat2;
		if ((right == 0) || (top == 0))
		{
			MatrixOrthoOffCenterLH( &mat, left, (float)m_Width,  bottom, (float)m_Height, -1, 1.f );
		}
		else
		{
			MatrixOrthoOffCenterLH( &mat, left, right,  bottom, top, -1, 1.f );
		}
		mat.m[5] = -mat.m[5];
		mat.m[14] = 1;
        mat.SetTranspose();
		SetMatrix( PROJECTION_MATRIX, (const float *)mat.GetMatrix() );
		mat.SetIdentity();
		SetMatrix( VIEW_MATRIX, (const float *)mat.GetMatrix() );
		SetMatrix( WORLD_MATRIX, (const float *)mat.GetMatrix() );
		m_Ortho2DScreenSizeSet = true;
	}
}

GLenum CGLRenderer::GetD3DDepthBufferFormat( UINT &depthbits )
{
	GLenum retval;
	switch(depthbits)
	{
	case 16:
		{
			retval = GL_DEPTH_COMPONENT16;
			break;
		}
	case 24:
		{
			retval = GL_DEPTH_COMPONENT24;
			break;
		}
	case 32:
		{
			retval = GL_DEPTH_COMPONENT32;
			break;
		}
	default:
		{
			retval = GL_DEPTH_COMPONENT16;
			break;
		}
	};
	return retval;
}

IConfig *CGLRenderer::GetConfiguration(IHashString *name)
{
	CONFIGNAMEMAP::iterator cnmIter;

	cnmIter = m_ConfigNameMap.find(name->GetUniqueID());
	if (cnmIter != m_ConfigNameMap.end())
	{
		return &cnmIter->second;
	}

	return NULL;
}

// build the configuration map from DirectX
void CGLRenderer::BuildConfigMap()
{
#if 0
	D3DCAPS9 d3dCaps;
	HRESULT hr;
	m_ConfigNameMap.clear();
	if (m_pDevice != NULL)
	{
		// get capabilities from device
		hr = m_pDevice->GetDeviceCaps(&d3dCaps);
		// make sure we succeeded
		if (hr == D3D_OK)
		{
			CHashString maxTexStagesName(_T("MAX_TEXTURE_STAGES"));
			m_ConfigNameMap[maxTexStagesName.GetUniqueID()] = d3dCaps.MaxTextureBlendStages;
			
			CHashString maxVCs(_T("MAX_VERTEX_SHADER_CONSTANTS"));
			m_ConfigNameMap[maxVCs.GetUniqueID()] = d3dCaps.MaxVertexShaderConst;
			
			//DPG: Is it possible to determine the max constant register count for a Pixel Shader?
			CHashString maxPCs(_T("MAX_PIXEL_SHADER_CONSTANTS"));
			//m_ConfigNameMap[maxPCs.GetUniqueID()] = d3dCaps.MaxPix;
			m_ConfigNameMap[maxPCs.GetUniqueID()] = MAX_CONST_REGISTER_PS_30;
			
		}
	}
#endif
}

void CGLRenderer::SetLightArray( ILightObject * lights[], int numlights, ILightObject * detailLights[], 
		int numDetailLights, float *pAmbientLightTerm )
{

    //for branched shaders
    
    //float color_position[8];
    float color_diffuse[4];
    //float color_specular[4];
    float color_position[4];
    int setLight = 0;
    for( int i = 0; i < numlights; i++ )
    {
        //set up vectors
        if( lights[ i ] )
        {
            //check light type/directional or point, for now mimic directional
            lights[i]->GetColorIntensity( &color_diffuse[0] );
            //lights[i]->GetColorSpecular( &color_specular[0] );
            lights[i]->GetVectorAttenuation( &color_position[0] );				
            //the +1 offset to the index is for the number of lights
            SetPixelShaderConstantF( C_LIGHT_START + setLight*2 + 0, color_diffuse, 1 );
            SetPixelShaderConstantF( C_LIGHT_START + setLight*2 + 1, color_diffuse, 1 ); 
            SetVertexShaderConstantF( C_VERTEX_LIGHT_START + setLight, color_position, 1 );	
        }	
        setLight++;
    }
    //detail light pa
    Matrix4x4 matWorldInv;
    m_WorldMatrix.GetInverse( matWorldInv );
    for( int i = 0; i < numDetailLights; i++ )
    {
        //set up vectors
        if( detailLights[ i ] )
        {
            //check light type/directional or point, for now mimic directional
            detailLights[i]->GetColorIntensity( &color_diffuse[0] );
            //lights[i]->GetColorSpecular( &color_specular[0] );
            detailLights[i]->GetVectorAttenuation( &color_position[0] );				
            //the +1 offset to the index is for the number of lights
            //detail lights have no spec
            //Let's put the light into model space(multiply with inverse world) so as to not make
            //this pixelshader heavy
            Vec3 modelPos;
            modelPos.Set( color_position[0], color_position[1], color_position[2] );
            modelPos = matWorldInv * modelPos;
            color_position[0] = modelPos.x;
            color_position[1] = modelPos.y;
            color_position[2] = modelPos.z;

            SetPixelShaderConstantF( C_LIGHT_START + setLight*2 + 0, color_position, 1 );
            SetPixelShaderConstantF( C_LIGHT_START + setLight*2 + 1, color_diffuse, 1 );
            //
        }
        setLight++;
    }
    
    if (pAmbientLightTerm)
    {
        SetPixelShaderConstantF( C_LIGHT_AMBIENT, pAmbientLightTerm, 1 );
    }
}

void CGLRenderer::EnableScissorClip(bool enable)
{
	if (enable)
		SetGLRenderState(RENDERSTATE_SCISSORTESTENABLE, RENDERSTATEPARAM_TRUE);
	else
		SetGLRenderState(RENDERSTATE_SCISSORTESTENABLE, RENDERSTATEPARAM_FALSE);
}
	
void CGLRenderer::SetScissorRect(int x1, int y1, int x2, int y2)
{

    glScissor(x1, y1, x2-x1, y2-y1);
}

void CGLRenderer::SetColorMask( bool r, bool g, bool b, bool a )
{
    glColorMask(r, g, b, a);
}

void CGLRenderer::EnablePostProcess( bool bEnable )
{
	if (bEnable)
	{
		if (!m_PostProcessBuffer)
			BuildPostProcessBuffer( RENDER_TARGET_COLOR_BUFFER );
	}
	else
		RemovePostProcessBuffer();
}

void CGLRenderer::RemovePostProcessBuffer()
{
	if (m_PostProcessBuffer)
	{
		TEXTUREOBJECTPARAMS tobj;
		tobj.bLoad = false;
		tobj.Name = &m_hszPostProcessBufferName;
		tobj.TextureObjectInterface = NULL;
		static DWORD msgHash_RemoveTexture = CHashString(_T("RemoveTexture")).GetUniqueID();
		EngineGetToolBox()->SendMessage(msgHash_RemoveTexture, sizeof(tobj), &tobj );
		m_PostProcessBuffer = NULL;
	}
}

bool CGLRenderer::BuildPostProcessBuffer( RENDER_TARGET_TYPE renderTargetType )
{
	assert( m_PostProcessBuffer == NULL );
	if( m_PostProcessBuffer == NULL )
	{

		CREATETEXTUREPARAMS createtex;
		createtex.Name = &m_hszPostProcessBufferName;
		int sizeX, sizeY;
		GetBackBufferDimensions( sizeX, sizeY );
		createtex.sizeX = sizeX;
		createtex.sizeY = sizeY;
		createtex.bRenderTargetTexture = renderTargetType;
		createtex.numMips = 0;
		createtex.bAutoGenMipMaps = true;
		static DWORD msgHash_CreateTexture = CHashString(_T("CreateTexture")).GetUniqueID();
		EngineGetToolBox()->SendMessage(msgHash_CreateTexture, sizeof(createtex), &createtex );
		m_PostProcessBuffer = (ITextureObject*)createtex.TextureObjectInterface;
		return (m_PostProcessBuffer != NULL);
	}
	else
		return false;
}

void CGLRenderer::GetBackBufferDimensions( int &width, int &height )
{
	//width = m_presentParameters.BackBufferWidth;
	//height = m_presentParameters.BackBufferHeight;
}

/// renders the buffer back to the intermediate buffer
bool CGLRenderer::RenderPostProcessIntermediate( IEffect * effect, ITextureObject * src, ITextureObject * dest )
{
	//basically draw a screen sized quad
	if( m_PostProcessBuffer )
	{
		BeginScene( false, dest );
		if( !src )
		{
			src = m_PostProcessBuffer;
		}
		if( src != m_PostProcessBuffer )
		{
			ClearScreen( true, true );
		}
		SetEffect( 0, effect );
		SetDepthTest( false );
		SetDepthWrite( false );
		SetAlphaTest(false);
		SetRenderState(RENDERSTATE_ALPHABLENDENABLE, RENDERSTATEPARAM_FALSE);
		SetTexture( 0, src );
		DrawFullscreenQuad(0, 1, 0, 1);
  		SetDepthTest( true );
		SetDepthWrite( true );
		SetAlphaTest(true);
		SetRenderState(RENDERSTATE_ALPHABLENDENABLE, RENDERSTATEPARAM_TRUE);
		EndScene();
		return true;
	}
	return false;
}

bool CGLRenderer::RenderPostProcessFinal( IEffect *effect, IRenderContext *rendercontext )
{

//	m_pDevice->BeginScene();
  	SetDepthTest( false );
	SetDepthWrite( false );
	SetAlphaTest(false);
	SetRenderState(RENDERSTATE_ALPHABLENDENABLE, RENDERSTATEPARAM_FALSE);
  	SetEffect( 0, effect );
	SetTexture( 0, m_PostProcessBuffer );
	DrawFullscreenQuad(0, 1, 0, 1);
  	SetDepthTest( true );
	SetDepthWrite( true );
	SetAlphaTest(true);
	SetRenderState(RENDERSTATE_ALPHABLENDENABLE, RENDERSTATEPARAM_TRUE);
//   	m_pDevice->EndScene();

	return true;
}


/// renders the buffer to the final screen buffer
bool CGLRenderer::Present( IRenderContext * rendercontext )
{
    CGLRenderContext * GLcontext = dynamic_cast< CGLRenderContext *>( rendercontext );
    HRESULT result = EEGLPresent( GLcontext, m_ClearColor );
    return (result == 0);
}

DWORD CGLRenderer::GetColorARGB( int a, int r, int g, int b )
{
	return (a << 24) | (r << 16) | (g << 8) | b ;
}

DWORD CGLRenderer::GetColorARGB( float a, float r, float g, float b )
{
	return GetColorARGB( (int)(a*255.f), (int)(r*255.f), (int)(g*255.f), (int)(b*255.f));
}

DWORD CGLRenderer::GetRendererAPIGUID()
{
#if 0
	if( m_APIGUID == INVALID_API_GUID )
	{
		StdString version = D3DXGetPixelShaderProfile(m_pDevice);
		
		// extract version number, where it's ps_#_#
		StdString justNumber, token;
		version.GetToken(_T("_"), token); // ps...
		version.GetToken(_T("_"), token); // first #
		justNumber = token;
		justNumber += _T(".");
		version.GetToken(_T("_"), token); // second #
		justNumber += token;
		
		const TCHAR* verNum = justNumber;

		m_APIGUID = ConstructAPIGUID("GL", (TCHAR*)verNum, "");
	}
#endif
	return m_APIGUID;
}

bool CGLRenderer::SetMaterial( UINT pass, IMaterial * mat )
{
	if( mat )
	{
		mat->ApplyMaterial( pass, NULL );
	}else
	{
		SetPixelShader( NULL );
		SetVertexShader( NULL );
		SetBlendMode( BLEND_NORMAL );
		
		SetGLTextureStageState( 0, TEXTURESTAGESTATE_COLOROP, TEXTURESTAGEOP_MODULATE );
		SetGLTextureStageState( 0, TEXTURESTAGESTATE_COLORARG1, TEXTURESTAGEARG_TEXTURE );
		SetGLTextureStageState( 0, TEXTURESTAGESTATE_COLORARG2, TEXTURESTAGEARG_DIFFUSE );
		SetGLTextureStageState( 0, TEXTURESTAGESTATE_ALPHAOP, TEXTURESTAGEOP_SELECTARG2 );
		SetGLTextureStageState( 0, TEXTURESTAGESTATE_ALPHAARG2, TEXTURESTAGEARG_DIFFUSE);
		SetGLTextureStageState( 1, TEXTURESTAGESTATE_COLOROP, TEXTURESTAGEOP_DISABLE );
		SetGLTextureStageState( 1, TEXTURESTAGESTATE_ALPHAOP, TEXTURESTAGEOP_DISABLE );
	}
	return true;
}


void CGLRenderer::SetEffect( UINT pass, IEffect * effect )
{
	if( effect )
	{
		effect->Apply( pass, NULL, NULL );
	}
	else
	{
#ifdef DISABLE_FIXEDFUNCTION
		SetBlendMode( BLEND_NORMAL );
		if (!m_pDefaultEffect)
		{
			m_pDefaultEffect = CRenderObject<>::LoadEffect(NULL);
		}
		if (m_pDefaultEffect)
			m_pDefaultEffect->Apply( pass, NULL, NULL );
#else
		SetPixelShader( NULL );
		SetVertexShader( NULL );
		SetBlendMode( BLEND_NORMAL );

		SetGLTextureStageState( 0, TEXTURESTAGESTATE_COLOROP, TEXTURESTAGEOP_MODULATE );
		SetGLTextureStageState( 0, TEXTURESTAGESTATE_COLORARG1, TEXTURESTAGEARG_TEXTURE );
		SetGLTextureStageState( 0, TEXTURESTAGESTATE_COLORARG2, TEXTURESTAGEARG_DIFFUSE );
		SetGLTextureStageState( 0, TEXTURESTAGESTATE_ALPHAOP, TEXTURESTAGEOP_MODULATE ); 
		SetGLTextureStageState( 1, TEXTURESTAGESTATE_COLOROP, TEXTURESTAGEOP_DISABLE );
		SetGLTextureStageState( 1, TEXTURESTAGESTATE_ALPHAOP, TEXTURESTAGEOP_DISABLE );
#endif
	}
}

void CGLRenderer::SetDefaultStates()
{
#if 0
	if( m_pDevice )
	{
		m_pDevice->SetPixelShader( NULL );
		m_pDevice->SetVertexDeclaration( NULL );
		m_pDevice->SetVertexShader( NULL );
		m_pDevice->SetFVF( NULL );
	}
#endif
}

void CGLRenderer::RegisterTextureStageEnumHashes()
{
	CHashString minfilter(_T("minfilter"));
	CHashString magfilter(_T("magfilter"));
	CHashString mipfilter(_T("mipfilter"));
	CHashString umode(_T("umode"));
	CHashString vmode(_T("vmode"));

	m_SamplerStateEnum[ minfilter.GetUniqueID() ] = SAMPLERSTATE_MINFILTER;
	m_SamplerStateEnum[ magfilter.GetUniqueID() ] = SAMPLERSTATE_MAGFILTER;
	m_SamplerStateEnum[ mipfilter.GetUniqueID() ] = SAMPLERSTATE_MIPFILTER;
	m_SamplerStateEnum[ umode.GetUniqueID() ] = SAMPLERSTATE_U;
	m_SamplerStateEnum[ vmode.GetUniqueID() ] = SAMPLERSTATE_V;

	CHashString clamp(_T("clamp"));
	CHashString wrap(_T("wrap"));
	CHashString linear(_T("linear"));
	CHashString bilinear(_T("bilinear"));
	CHashString gaussian(_T("gaussian"));
	CHashString point(_T("point"));
	CHashString mirror(_T("mirror"));
	CHashString mipnone(_T("mipnone"));
	CHashString none(_T("none"));

	m_TextureStageStateEnum.clear();
	m_TextureStageStateEnum[ gaussian.GetUniqueID() ] = TEXTURESTAGE_TEXF_LINEAR;//D3DTEXF_GAUSSIANQUAD;
	m_TextureStageStateEnum[ clamp.GetUniqueID() ] = TEXTURESTAGE_TADDRESS_CLAMP;
	m_TextureStageStateEnum[ wrap.GetUniqueID() ] = TEXTURESTAGE_TADDRESS_WRAP;
	m_TextureStageStateEnum[ linear.GetUniqueID() ] = TEXTURESTAGE_TEXF_LINEAR;
	m_TextureStageStateEnum[ bilinear.GetUniqueID() ] = TEXTURESTAGE_TEXF_LINEAR;
	m_TextureStageStateEnum[ point.GetUniqueID() ] = TEXTURESTAGE_TEXF_POINT;
	m_TextureStageStateEnum[ mirror.GetUniqueID() ] = TEXTURESTAGE_TADDRESS_MIRROR;
	m_TextureStageStateEnum[ mipnone.GetUniqueID() ] = TEXTURESTAGE_TEXF_NONE;
	m_TextureStageStateEnum[ none.GetUniqueID() ] = TEXTURESTAGE_TEXF_NONE;

	CHashString alphablend(_T("alphablend"));
	CHashString ttrue( _T("true"));
	CHashString tfalse(_T("false"));
	CHashString cullnone(_T("cullnone"));
	CHashString cullreverse(_T("cullreverse"));
	CHashString cullmode(_T("cullmode"));
	CHashString depthwrite(_T("depthwrite"));
	CHashString depthtest(_T("depthtest"));
	CHashString ztrue(_T("ztrue"));
	CHashString zfalse(_T("zfalse"));
	CHashString cullregular(_T("cullregular"));
	CHashString alphatest(_T("alphatest"));
	CHashString fillmode(_T("fillmode"));
	CHashString wireframe(_T("wireframe"));
	CHashString solid(_T("solid"));
	CHashString alphatestref(_T("alphatestref"));

	m_RenderStateEnum.clear();
	m_RenderStateEnum[ alphatest.GetUniqueID() ] = RENDERSTATE_ALPHATESTENABLE;
	m_RenderStateEnum[ alphablend.GetUniqueID() ] = RENDERSTATE_ALPHABLENDENABLE;
	m_RenderStateEnum[ alphatestref.GetUniqueID() ] = RENDERSTATE_ALPHAREF;
	m_RenderStateEnum[ cullmode.GetUniqueID() ] = RENDERSTATE_CULLMODE;
	m_RenderStateEnum[ depthwrite.GetUniqueID() ] = RENDERSTATE_ZWRITEENABLE;
	m_RenderStateEnum[ depthtest.GetUniqueID() ] = RENDERSTATE_ZENABLE;
	m_RenderStateEnum[ fillmode.GetUniqueID() ] = RENDERSTATE_FILLMODE;

	m_RenderStateParamEnum.clear();
	m_RenderStateParamEnum[ ttrue.GetUniqueID() ] = RENDERSTATEPARAM_TRUE;
	m_RenderStateParamEnum[ tfalse.GetUniqueID() ] = RENDERSTATEPARAM_FALSE;
	m_RenderStateParamEnum[ cullnone.GetUniqueID() ] = RENDERSTATEPARAM_CULLNONE;
	m_RenderStateParamEnum[ cullregular.GetUniqueID() ] = RENDERSTATEPARAM_CULLCCW;
	m_RenderStateParamEnum[ cullreverse.GetUniqueID() ] = RENDERSTATEPARAM_CULLCW;
	m_RenderStateParamEnum[ ztrue.GetUniqueID() ] = RENDERSTATEPARAM_ZTRUE;
	m_RenderStateParamEnum[ zfalse.GetUniqueID() ] = RENDERSTATEPARAM_ZFALSE;
	m_RenderStateParamEnum[ wireframe.GetUniqueID() ] = RENDERSTATEPARAM_WIREFRAME;
	m_RenderStateParamEnum[ solid.GetUniqueID() ] = RENDERSTATEPARAM_SOLID;
}

HRESULT CGLRenderer::SetGLTextureStageState( UINT stage, ENUMTEXTURESTAGESTATE state, UINT value )
{
#if 0
	assert( stage < MAX_BUFFERED_CHANNELS );
	assert( state < TEXTURESTAGESTATE_COUNT );

	HRESULT result = S_OK;
	UINT *pCurrentValue = NULL;
	if( m_RenderContext )
	{
		pCurrentValue = &m_RenderContext->m_CurrentTextureStageState[stage][state];
	}
	else
	{
		pCurrentValue = &m_CurrentTextureStageState[stage][state];
	}
    
	if (*pCurrentValue != value)
	{
		result = m_pDevice->SetTextureStageState( stage, m_D3DTextureStageStateEnum[state], m_D3DTextureStageParamEnum[value] );
		*pCurrentValue = value;
	}
	return result;
#else
	return false;
#endif
}

HRESULT CGLRenderer::SetGLSamplerStageState( UINT stage, ENUMSAMPLERSTATE state, UINT value )
{
#if 0
	assert( stage < MAX_BUFFERED_CHANNELS );
	assert( state < SAMPLERSTATE_COUNT );

	HRESULT result = S_OK;
	UINT *pCurrentValue = NULL;
	if( m_RenderContext )
	{
		pCurrentValue = &m_RenderContext->m_CurrentSamplerState[stage][state];
	}
	else
	{
		pCurrentValue = &m_CurrentSamplerState[stage][state];
	}
	if (*pCurrentValue != value)
	{
		result = m_pDevice->SetSamplerState( stage, m_D3DSamplerStateEnum[state], m_D3DTextureStageParamEnum[value] );
		*pCurrentValue = value;
	}
	return result;
#else
    return 0;
#endif
}

HRESULT CGLRenderer::SetGLRenderState( ENUMRENDERSTATE state, UINT value )
{
#if 0
	assert( state < RENDERSTATE_COUNT );

	if (state != RENDERSTATE_POINTSIZE && 
		state != RENDERSTATE_DEPTHBIAS && 
		state != RENDERSTATE_SLOPESCALEDEPTHBIAS && 
		state != RENDERSTATE_ALPHAREF && 
		state != RENDERSTATE_COLORWRITEENABLE
		)
	{
		// translate value to d3d enum
		value = m_D3DRenderStateParamEnum[value];
	}

	HRESULT result = S_OK;
	UINT *pCurrentValue = NULL;
	if( m_RenderContext )
	{
		pCurrentValue = &m_RenderContext->m_CurrentRenderState[state];
	}
	else
	{
		pCurrentValue = &m_CurrentRenderState[state];
	}
	if (*pCurrentValue != value)
	{
		// if we're supposed to render everything in wireframe and something else wants to render solid,
		// don't change the state
		if ((state == RENDERSTATE_FILLMODE) && (m_bWireFrameMode))
		{
			return result;
		}
		else if (state == RENDERSTATE_ALPHAREF)
		{
			value = min( value, 255 );
			value = max( value, 0 );
		}
		result = m_pDevice->SetRenderState( m_D3DRenderStateEnum[state], value );

		*pCurrentValue = value;
	}
	return result;
#else
    return 0;
#endif
}

ENUMRENDERSTATE CGLRenderer::StringToRenderState( DWORD uniqueID )
{
	return m_RenderStateEnum[ uniqueID ];
}

ENUMRENDERSTATEPARAM CGLRenderer::StringToRenderStateParam( DWORD uniqueID )
{
	return m_RenderStateParamEnum[ uniqueID ];
}

ENUMSAMPLERSTATE CGLRenderer::StringToSamplerState( DWORD uniqueID )
{
	return m_SamplerStateEnum[ uniqueID ];
}

TEXTURESTAGEPARAM CGLRenderer::StringToTextureStageParam( DWORD uniqueID )
{
	return m_TextureStageStateEnum[ uniqueID ];
}

void CGLRenderer::SetSamplerState( UINT stage, ENUMSAMPLERSTATE mode, TEXTURESTAGEPARAM val )
{
	SetGLSamplerStageState( stage, mode, val );
}

void CGLRenderer::SetRenderState( ENUMRENDERSTATE mode, UINT val )
{
	SetGLRenderState( mode, val );
}

DWORD CGLRenderer::ConstructAPIGUID( const TCHAR * api, const TCHAR * shaderversion, const TCHAR * misc )
{
	StdString concat = "";
	concat += api;
	concat += shaderversion;
	concat += misc;
	CHashString hcon((const TCHAR * )concat );
	return hcon.GetUniqueID();
}

bool CGLRenderer::SetStreamSource( UINT stream, IVertexBufferObject * vb, UINT offset, UINT stride )
{
#if 0
    CGLVertexBufferObject * cvb = dynamic_cast< CGLVertexBufferObject * >( vb );
    if( cvb )
    {
        //check match
        //Not sure why this is Byte..? faster compare or something?
        if( m_VertexStreams[ stream ] != (BYTE*)cvb )
        {
            m_VertexStreams[ stream ] = (BYTE*)cvb;
            m_pDevice->SetStreamSource( stream, cvb->GetGLInterface(), offset, stride );
        }
    }
    return true;
#endif
    return true;
}

void CGLRenderer::PushRenderState( ENUMRENDERSTATE mode, UINT val )
{
#if 0
	if( m_pDevice )
	{
		DWORD d3dmode = m_RenderStateEnum[ mode ];
		DWORD d3dval;
		HRESULT ret = m_pDevice->GetRenderState(( D3DRENDERSTATETYPE )d3dmode, &d3dval);
		if (ret == D3D_OK)
		{
			m_RenderStateStackMap[mode].push(d3dval);
            SetRenderState(mode, val);
		}
		else
		{
			EngineGetToolBox()->Log(LOGERROR, _T("GLRenderer: could not get render state in push render state!\n"));
		}
	}
#endif
}
	
CGLRenderer::~CGLRenderer()
{
	Reset();
	if( m_SetTextures )
	{
		delete [] m_SetTextures;
		m_SetTextures = NULL;
	}
	if( m_SetVertexTextures )
	{
		delete [] m_SetVertexTextures;
		m_SetVertexTextures = NULL;
	}
}

void CGLRenderer::PopRenderState( ENUMRENDERSTATE mode )
{
    if (m_RenderStateStackMap[mode].size() > 0)
    {
        UINT val = m_RenderStateStackMap[mode].top();
        m_RenderStateStackMap[mode].pop();
        SetRenderState( mode, val );
    }
}

void CGLRenderer::BuildColorRemappingTable( D3DCOLORTABLETYPE *GammaRamp, float fBrightness, float fContrast, float fGamma )
{
#if 0
	static const float fTableEntryScale    = (float)(0xffffffff >> (32 - sizeof(D3DCOLORTABLETYPE) * 8));
	static const float fMinLookupValue     = 2.0f / 256.0f; // min value for geforce bug

	// Clamp values to a valid range
	fBrightness    = __max( 0.0f, __min( 1.0f,   fBrightness ) );
	fContrast      = __max( 0.0f, __min( 0.999f, fContrast ) );
	fGamma         = 1.0f / __max( 0.1f, __min( 10.0f, fGamma ) );

	// Build gamma ramp
	float fScaling  = tan(fContrast * PI / 2.0f);
	float fOffset   = fBrightness * 2.0f - 1.0f;
	for ( int i = 0; i < 256; ++i )
	{
		float fInput    = i/255.0f;
		float fValue    = pow( fInput * fScaling, fGamma );

		GammaRamp[i] = (D3DCOLORTABLETYPE)(__max( fMinLookupValue, __min( 1.0f, fValue + fOffset ) ) * fTableEntryScale + 0.5f);
	}
#endif
}

void CGLRenderer::SetColorCorrection( float fBrightness, float fContrast, float fGamma )
{
	COLORCORRECTION cc;
	cc.fBrightness = fBrightness;
	cc.fContrast = fContrast;
	cc.fGamma = fGamma;
	SetColorCorrection( cc, cc, cc );
}

void CGLRenderer::SetColorCorrection( COLORCORRECTION red, COLORCORRECTION green, COLORCORRECTION blue )
{
#if 0
	if (!m_pDevice)
		return;

	D3DGAMMARAMP GammaRamp;

	BuildColorRemappingTable( GammaRamp.red,   red.fBrightness,   red.fContrast,     red.fGamma     );
	BuildColorRemappingTable( GammaRamp.green, green.fBrightness, green.fContrast,   green.fGamma   );
	BuildColorRemappingTable( GammaRamp.blue,  blue.fBrightness,  blue.fContrast,    blue.fGamma    );

	m_pDevice->SetGammaRamp( 0, D3DSGR_CALIBRATE, &GammaRamp );
#endif
}

void CGLRenderer::BuildPerspectiveMatrix( float fovy, float aspect, float zn, float zf, Matrix4x4 &outMatrix )
{
	float radiansAngle = fovy*3.1415927f/180.f;
    float yScale = 1.0f / tan(radiansAngle / 2.0f);
    float xScale = yScale / aspect;
    outMatrix.SetIdentity();
    outMatrix.m[0] = xScale;
    outMatrix.m[5] = yScale;
    outMatrix.m[10] = zf / (zf - zn);
    outMatrix.m[11] = 1.0f;
    outMatrix.m[14] = -zn * zf / (zf - zn);
    outMatrix.m[15] = 0.0f;    
}

void CGLRenderer::BuildLookAtMatrix( const Vec3 &eye, const Vec3 &at, const Vec3 &up, Matrix4x4 &outMatrix )
{
    Vec3 zaxis = (at - eye).Normalize();
    Vec3 xaxis = up.Cross(zaxis).Normalize();
    Vec3 yaxis = zaxis.Cross(xaxis);
    Vec3 translation(-xaxis.Dot(eye), -yaxis.Dot(eye), -zaxis.Dot(eye));

	outMatrix.Set(xaxis.x, yaxis.x, zaxis.x, 0.0f,
                  xaxis.y, yaxis.y, zaxis.y, 0.0f,
                  xaxis.z, yaxis.z, zaxis.z, 0.0f,
                  translation.x, translation.y, translation.z, 1.0f);
}

void CGLRenderer::BuildOrthoMatrix( float l, float r, float b, float t, float zn, float zf, Matrix4x4 &outMatrix )
{
	MatrixOrthoOffCenterLH( &outMatrix, l, r, b, t, zn, zf );
}

void CGLRenderer::AllocateFullscreenQuadVertexBuffer()
{
	// using a grid of quads rather than a single quad can be faster
	// because of rasterization optimizations
	const int iGridX = 8;
	const int iGridY = 1;
	const int iVertexCount = (iGridX*2 + 2); // tristripe since gridY is 1 and d3d doesn't have a quads prim

	CHANNELDESCRIPTORLIST VertexFormatCGLRenderer2D;
	//In fixed function FVF format, color precedes texture coords
	VertexFormatCGLRenderer2D.push_back( MESH_STREAM_VERTEX_POSITION3 );
	VertexFormatCGLRenderer2D.push_back( MESH_STREAM_VERTEX_COLORCRGBA );
	VertexFormatCGLRenderer2D.push_back( MESH_STREAM_VERTEX_TEXTURECOORD1 );

	ALLOCATERENDERBUFFERMSG msg;	
	msg.ElementsToAllocate = iVertexCount;
	msg.SizeVertexFormat = sizeof( CGLRenderer_VertexRHW );
	msg.VertexFormat = VertexFormatCGLRenderer2D;
	msg.Dynamic = false;
	static DWORD msgHash_AllocateVertices = CHashString(_T("AllocateVertices")).GetUniqueID();
	DWORD res = EngineGetToolBox()->SendMessage(msgHash_AllocateVertices, sizeof( msg ), &msg, NULL, NULL );
	if( res == MSG_HANDLED )
	{
		m_FullscreenQuadVBAllocation.m_Offset = msg.m_ReturnOffset;
		m_FullscreenQuadVBAllocation.m_AllocationHandle = msg.m_ReturnHandle;
		m_FullscreenQuadVBAllocation.m_InterfaceHandle = msg.m_ReturnInterfaceHandle;
		m_FullscreenQuadVBAllocation.m_Size = msg.ElementsToAllocate;
		m_FullscreenQuadVBAllocation.m_Stride = msg.SizeVertexFormat;
		m_FullscreenQuadVertexBuffer = (CGLVertexBufferObject*)msg.m_InterfacePointer;
	}
	else
		m_FullscreenQuadVertexBuffer = NULL;

	if (!m_FullscreenQuadVertexBuffer)
		return;

	CGLRenderer_VertexRHW *pVertices = NULL;
	m_FullscreenQuadVertexBuffer->Lock( m_FullscreenQuadVBAllocation.m_Offset, m_FullscreenQuadVBAllocation.m_Size, (void**)&pVertices, 0 );
	if (pVertices)
	{
		int v=0;
		for (int y=0; y < iGridY; y++)
		{
			for (int x=0; x <= iGridX; x++, v+=2)
			{
				pVertices[v+0].color = 0xFFFFFFFF;
				pVertices[v+0].tu = (float)x / (float)iGridX;
				pVertices[v+0].tv = (float)y / (float)iGridY;
				pVertices[v+0].pos[0] = pVertices[v+0].tu;
				pVertices[v+0].pos[1] = pVertices[v+0].tv;
				pVertices[v+0].pos[2] = 0.0f;

				pVertices[v+1].color = 0xFFFFFFFF;
				pVertices[v+1].tu = (float)x / (float)iGridX;
				pVertices[v+1].tv = (float)(y+1) / (float)iGridY;
				pVertices[v+1].pos[0] = pVertices[v+1].tu;
				pVertices[v+1].pos[1] = pVertices[v+1].tv;
				pVertices[v+1].pos[2] = 0.0f;
			}
		}
	}
	m_FullscreenQuadVertexBuffer->Unlock();
}
