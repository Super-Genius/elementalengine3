///============================================================================
/// \note   Elemental Engine
///         Copyright (c)  2013 Social Systems Technology, Inc.
///
///         This code is redistributable under the terms of the EE License.
///
///         This code is distributed without warranty or implied warranty of
///			merchantability or fitness for a particular purpose. See the 
///			EE License for more details.
///
///         You should have received a copy of the EE License along with this
///			code; If not, write to Social Systems Technology, Inc,
///			.
///============================================================================

#include "Stdafx.h"
#include "CGLCubeTextureObject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

REGISTER_COMPONENT(CGLCubeTextureObject);

///
/// Constructor / Destructor
///

CGLCubeTextureObject::CGLCubeTextureObject(IHashString *parentName, IHashString *name) : 
	OBJECTTEMPLATE( CTextureManager, CGLCubeTextureObject, ICubeTextureObject, parentName, name )
{
	memset( m_pLocalBuffer, 0, sizeof(m_pLocalBuffer) );
	m_iLocalBufferSize = 0;
	m_Texture = NULL;

	DeInit();
	
	// grab the render interface
	GETRENDERERINTERFACEPARAMS renderInterface;
	static DWORD msgHash_GetRendererInterface = CHashString(_T("GetRendererInterface")).GetUniqueID();
	EngineGetToolBox()->SendMessage(msgHash_GetRendererInterface, 
						sizeof(GETRENDERERINTERFACEPARAMS), 
						&renderInterface, 
						NULL, 
						NULL);
	m_Renderer = renderInterface.m_RendererInterface;
}

CGLCubeTextureObject::~CGLCubeTextureObject()
{
	DeInit();
}

/// \brief The Initialization for the Object
/// \return pointer to an IComponent
IComponent *CGLCubeTextureObject::Create(int nArgs, va_list argptr)
{
	IHashString *name;
	IHashString *parentName;
	IObject *self;

	name = va_arg(argptr, IHashString *);
	parentName = va_arg(argptr, IHashString *);
	// create the object however you must create it
	try
	{
		// try and create an instance
		self = (IObject*)new CGLCubeTextureObject(parentName, name);
	}
	catch(...)
	{
		return NULL;
	}

	return self;
}

/// \brief Loads a texture
/// \param file = the filename
bool CGLCubeTextureObject::LoadFromFile( const TCHAR * file )
{
	DeInit();

	StdString szFilePath(file);
	szFilePath.MakeSafeFileName();

	TEXTUREOBJECTPARAMS tparm;
	//Find the texture in the manager first
	IToolBox * toolbox = EngineGetToolBox();
	if(!toolbox)
	{ 
		return false;
	}
    
	if( !m_Renderer )
	{
		return false;
	}
    
	CHashString tempname( file );
	tparm.Name = &tempname;
	static DWORD msgHash_GetTexture = CHashString(_T("GetTexture")).GetUniqueID();
	DWORD msg = toolbox->SendMessage(msgHash_GetTexture, sizeof( tparm ), &tparm, GetManagerName(), NULL );
	if( msg != MSG_HANDLED )
	{
		return false;
	}
    
	//there's already a texture with the same name. Try to cast it to same type so we can clone it
	if( tparm.TextureObjectInterface )
	{
		CGLCubeTextureObject * ctex = dynamic_cast< CGLCubeTextureObject * >( tparm.TextureObjectInterface);
		if( ctex )
		{
			//yep we can clone it
			m_Filename = file;
			m_Texture = ctex->m_Texture;
			m_bRenderTarget = false;
			return true;
		}else
		{
			//we're using 2 different API textures, with the same name, not good idea, return false
			return false;
		}
	}

#if 0
	//create new texture
	//error for non power of two textures if debug
	D3DXIMAGE_INFO srcInfo;
	LPDIRECT3DCUBETEXTURE9 temptex;
	//TODO: more control over texture creation?
	void *pFileData = NULL;
	UINT iFileDataSize = 0;
	if (!LoadFileIntoBuffer( szFilePath, pFileData, iFileDataSize, true ))
	{
		m_ToolBox->Log(LOGERROR, _T("Texture Object: %s failed to load\n"), szFilePath.c_str() );
		return false;
	}
	D3DFORMAT requestedFormat = EEGLRequestedInternalFormatFromFileInMemory( pFileData, iFileDataSize );
    
	HRESULT hr = D3DXCreateCubeTextureFromFileInMemoryEx( pDevice,
									pFileData, 
									iFileDataSize, 
									D3DX_DEFAULT, //size
									D3DX_DEFAULT, //number of mips
									0,	//usage - 0 unless for render targets
									requestedFormat,
									D3DPOOL_MANAGED,
									D3DX_DEFAULT,	//regular filter
									D3DX_DEFAULT,	//mip filter
									0, //color key, opaque black default
									&srcInfo,	//src info
									NULL, //palette
									&temptex );
	SAFE_DELETE_ARRAY( pFileData );
	if( hr != D3D_OK )
	{
		m_ToolBox->Log(LOGERROR, _T("Texture Object: %s failed to load\n"), szFilePath.c_str() );
		return false;
	}
	if( temptex )
	{
		m_Texture = temptex;
		m_Filename = file;
		m_bRenderTarget = false;		
		D3DSURFACE_DESC tempDesc;
		temptex->GetLevelDesc(0, &tempDesc);
		m_Size  = tempDesc.Width;
		m_ColorDepth = tempDesc.Format;
		m_iLocalBufferSize = 0;
		return true;
	}
#endif
	return false;
	
}

UINT CGLCubeTextureObject::GetHeight()
{
	return m_Size;
}

UINT CGLCubeTextureObject::GetWidth()
{
	return m_Size;
}

UINT CGLCubeTextureObject::GetColorDepth()
{
	return m_ColorDepth;
}

bool CGLCubeTextureObject::IsRenderTarget()
{
	return m_bRenderTarget;
}

void* CGLCubeTextureObject::Read( EE_CUBEMAPFACE faceindex )
{
	if( m_Texture )
	{
		int iTextureSize = (GetColorDepth() / 8) * GetWidth() * GetHeight();
		// delete the old buffer if our current one is the wrong size, means all buffers are invalid
		if (iTextureSize != m_iLocalBufferSize)
		{
			for( int i = 0; i < NUM_CUBE_FACES; i++ )
			{
				if( m_pLocalBuffer[ i ] )
				{
					delete (unsigned char *)m_pLocalBuffer[ i ];
					m_pLocalBuffer[ i ] = NULL;
				}
			}
			m_iLocalBufferSize = 0;
		}
		// allocate a new buffer if we don't have one
		if (!m_pLocalBuffer[ faceindex ])
		{
			m_pLocalBuffer[ faceindex ] = new unsigned char[iTextureSize];
			m_iLocalBufferSize = iTextureSize;
		}
#if 0
		//lock the texture and read the data
		D3DLOCKED_RECT  lockrect;
		D3DCUBEMAP_FACES face = GetD3DCubeFaceEnum( faceindex );
		HRESULT hr = m_Texture->LockRect( face, 0, &lockrect, NULL, D3DLOCK_READONLY );
		if( hr == D3D_OK )
		{
			UINT pixelsize = (GetColorDepth()/8);
			BYTE * srcbits = (BYTE*)lockrect.pBits;
			BYTE * destbits = (BYTE*)m_pLocalBuffer[ faceindex ];
			//write the texture to the buffer
			for( UINT i = 0; i < m_Size; i++ )
			{
                memcpy( destbits, srcbits, m_Size*pixelsize );
				//move by pitch
				srcbits += lockrect.Pitch;
				destbits += m_Size*pixelsize;
			}
		}
		m_Texture->UnlockRect( face, 0 );
		return m_pLocalBuffer;
#endif
	}
	return NULL;
}

static GLenum GL_CUBEMAP_MAP[CUBEMAP_FACE_NEGATIVE_Z + 1] = { GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
                                                           GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
                                                           GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

GLenum CGLCubeTextureObject::GetGLCubeFaceEnum( EE_CUBEMAPFACE faceindex )
{
    if ((faceindex >= 0) && (faceindex <= 6))
    {
        return GL_CUBEMAP_MAP[faceindex];
    }
    
    EngineGetToolBox()->Log(LOGERROR, _T("Cubemap face index out of bounds. Using Cubemap Positive X as default."));
    
    return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
}

bool CGLCubeTextureObject::Write( EE_CUBEMAPFACE faceindex, void *p )
{
#if 0
	//UploadCubemapFace
	if (m_Texture && p)
	{
		void *pData =p;
		//lock the texture and write the data
		D3DLOCKED_RECT  lockrect;
		D3DCUBEMAP_FACES face = GetD3DCubeFaceEnum( faceindex );
		HRESULT hr = m_Texture->LockRect( face, 0, &lockrect, NULL, 0 );
		if( hr == D3D_OK )
		{
			UINT pixelsize = (GetColorDepth()/8);
			BYTE * destbits = (BYTE*)lockrect.pBits;
			BYTE * srcbits = (BYTE*)pData;
			//write the texture to the buffer
			for( UINT i = 0; i < m_Size; i++ )
			{
                memcpy( destbits, srcbits, m_Size*pixelsize );
				//move by pitch
				destbits += lockrect.Pitch;
				srcbits += m_Size*pixelsize;
			}
		}
		m_Texture->UnlockRect( face, 0 );
		//Update Mip maps here
		return true;
	}
#endif
	return false;
}

void * CGLCubeTextureObject::Lock( EE_CUBEMAPFACE faceindex )
{
#if 0
	if( m_Texture )
	{
		D3DLOCKED_RECT  lockrect;
		D3DCUBEMAP_FACES face = GetD3DCubeFaceEnum( faceindex );
		HRESULT hr = m_Texture->LockRect( face, 0, &lockrect, NULL, 0 );
		if( hr == D3D_OK )
		{
			//should return pitch as well, but that's only DX? Or Use read and write
			return lockrect.pBits;
		}
	}
#endif
	return NULL;
}

void CGLCubeTextureObject::Unlock( EE_CUBEMAPFACE faceindex )
{
#if 0
	if( m_Texture )
	{
		D3DCUBEMAP_FACES face = GetD3DCubeFaceEnum( faceindex );
		m_Texture->UnlockRect( face, 0 );
	}
#endif
}

///
/// Init / Update / DeInit
///
void CGLCubeTextureObject::Init()
{
	m_Filename = "";
	m_RefCount = 1;
	m_Texture = NULL;

	memset( m_pLocalBuffer, 0, sizeof(m_pLocalBuffer) );
	m_Size = 0;
	m_ColorDepth = 0;
	m_iLocalBufferSize = 0;
	m_bRenderTarget = false;
}

void CGLCubeTextureObject::DeInit()
{
#if 0
	if (m_Texture)
	{
		m_Texture->Release();
		m_Texture = NULL;
	}
#endif
    
	//remove all the faces
	for( int i = 0; i < NUM_CUBE_FACES; i++ )
	{
		if( m_pLocalBuffer[ i ] )
		{
			delete (unsigned char *)m_pLocalBuffer[ i ];
			m_pLocalBuffer[ i ] = NULL;
		}
	}
	m_Filename = "";
	m_RefCount = 1;
	m_Size = 0;
	m_ColorDepth = 0;
	m_iLocalBufferSize = 0;
	m_bRenderTarget = false;
}

/// \brief Update the object
/// \return true or false
bool CGLCubeTextureObject::Update()
{
	return true;
}

unsigned int CGLCubeTextureObject::GetReferenceCount()
{
	return m_RefCount;
}

IHashString *CGLCubeTextureObject::GetTextureName()
{
	return &m_Filename;

}

void CGLCubeTextureObject::SetTextureName(IHashString *name)
{
	m_Filename = name;

}

bool CGLCubeTextureObject::InitializeCubemap( const UINT squaresize, const UINT colordepth_in, bool bRenderTarget )
{
	DeInit();

	UINT colordepth = colordepth_in;
	if( !m_Renderer )
	{
		return false;
	}

	UINT size = squaresize*squaresize* ( colordepth  / 8 );
    GLuint temptex = 0;
#if 0
	D3DFORMAT format = D3DFMT_A8R8G8B8;
	UINT numMips = 0;
	DWORD usage = 0;
	if( bRenderTarget )
	{
		usage = D3DUSAGE_RENDERTARGET;
		numMips = 1;
	}
	format = EEGLFormatFromColorBits( colordepth );
	//create new texture	
	LPDIRECT3DCUBETEXTURE9 temptex;
	//TODO: more control over texture creation?
	if(FAILED(pDevice->CreateCubeTexture( squaresize, //width and height
									numMips, //number of mips
									usage,	//usage - 0 unless for render targets
									format,
									D3DPOOL_MANAGED, //TODO: managed vs. Unmanaged! unmanaged you can't lock!								
									&temptex,
									NULL)))
		{
			return false;
		}
		
	if( !temptex )
	{
		return false;
	}
#endif
	m_Size = squaresize;
	m_ColorDepth = colordepth;
	m_iLocalBufferSize = size;
	m_Texture = temptex;
	m_bRenderTarget = bRenderTarget;
	return true;
}
	
