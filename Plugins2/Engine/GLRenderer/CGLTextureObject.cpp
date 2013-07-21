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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

REGISTER_COMPONENT_AS(CGLTextureObject, CTextureObject);

///
/// Constructor / Destructor
///

CGLTextureObject::CGLTextureObject(IHashString *parentName, IHashString *name) : 
	OBJECTTEMPLATE( CTextureManager, CGLTextureObject, ITextureObject, parentName, name )
{
	m_pLocalBuffer = NULL;
	m_iLocalBufferSize = 0;
	m_Texture = NULL;
	m_RenderTargetSurface = NULL;
	m_Compressed = false;

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
	m_AutoGenMips = false;
}

CGLTextureObject::~CGLTextureObject()
{
	DeInit();
}

int CGLTextureObject::GetNumLevels()
{
	ReadyData();

	if( m_Texture )
	{
		return 1;   //m_Texture->GetLevelCount();
	}
	return 0;
}

/// \brief The Initialization for the Object
/// \return pointer to an IComponent
IComponent *CGLTextureObject::Create(int nArgs, va_list argptr)
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
		self = (IObject*)new CGLTextureObject(parentName, name);
	}
	catch(...)
	{
		return NULL;
	}

	return self;
}

/// \brief Loads a texture
/// \param file = the filename
bool CGLTextureObject::LoadFromFile( const TCHAR * file )
{
    // no internal loading, let the texture loader do it.
    return false;
}

bool CGLTextureObject::LoadHDRFile( TCHAR * file )
{

	return false;

}
bool CGLTextureObject::MakeRenderTarget( UINT sizex, UINT sizey, UINT colordepth, DWORD type, bool bAutoGenMipMaps )
{
#if 0
	DeInit();

	LPDIRECT3DDEVICE9 pDevice;
	if( !m_Renderer )
	{
		return false;
	}
	pDevice = (LPDIRECT3DDEVICE9)m_Renderer->GetAPIDevice();
	if( !pDevice )
	{
		return false;
	}
	D3DFORMAT format = D3DFMT_A8R8G8B8;
	DWORD usage = D3DUSAGE_RENDERTARGET;
	UINT numMips = 1;
	if (bAutoGenMipMaps)
		numMips = 0; // generate mips
	if( type == RENDER_TARGET_DEPTH_LUMINANCE )
	{
		//TODO: make use of d3dcaps
		/*format = D3DFMT_L16;//this doesn't work on some cards@#@#@!!! including 6600. hate minimal compliance D3D cards
		format = D3DFMT_D16;
		format = D3DFMT_X1R5G5B5;//this doesn't work on some cards@#@#@!!!
		format = D3DFMT_R32F;*/
		//format = D3DFMT_G16R16F;
		format = D3DFMT_A16B16G16R16F;
		//usage = D3DUSAGE_DEPTHSTENCIL;
		usage |= D3DUSAGE_AUTOGENMIPMAP;
	}
	else if( type == RENDER_TARGET_COMPRESSED_FLOATINGPOINT )
	{
		format = D3DFMT_A16B16G16R16F;
		usage |= D3DUSAGE_AUTOGENMIPMAP;
	}
	else if( type == RENDER_TARGET_FLOATINGPOINT )
	{
		format = D3DFMT_A32B32G32R32F;
	}
	else if( type == RENDER_TARGET_DEPTH_SURFACE )
	{
		format = D3DFMT_D24X8;
		usage = D3DUSAGE_DEPTHSTENCIL;
	}
	else if( type == RENDER_TARGET_TWO_CHANNEL_FP )
	{
		format = D3DFMT_G32R32F;
	}
	else if( type == RENDER_TARGET_ONE_CHANNEL_FP )
	{
		format = D3DFMT_R32F;
		usage |= D3DUSAGE_AUTOGENMIPMAP;
	}
	else
	{		
		format = EEGLFormatFromColorBits( colordepth );

		usage |= D3DUSAGE_AUTOGENMIPMAP;
	}
	//create new texture	
	LPDIRECT3DTEXTURE9 temptex = NULL;
	//TODO: more control over texture creation?
	if(FAILED(pDevice->CreateTexture( sizex, //width
									 sizey, //height
									numMips, //number of mips
									usage ,	//usage - 0 unless for render targets
									format,
									D3DPOOL_DEFAULT, 									
									&temptex, NULL )))
		{
//			DebugBreak();
			return false;
		}
		
	if( !temptex )
	{
		return false;
	}
	m_Texture = temptex;
	m_Height = sizey;
	m_Width = sizex;
	m_ColorDepth = colordepth;
	m_bRenderTarget = true;
	m_AutoGenMips = bAutoGenMipMaps;
#endif
	return true;
}

bool CGLTextureObject::MakeBlankTexture( UINT sizex, UINT sizey, UINT colordepth, IHashString * hformat, UINT mips )
{
	DeInit();

	if( !m_Renderer )
	{
		return false;
	}

	m_Levels = mips;
    
    glGenTextures(1, &m_Texture);
    if ((CheckAndLogGLError() != GL_NO_ERROR) || (m_Texture == 0))
    {
        m_ToolBox->Log(LOGFATALERROR, _T("Probably no OpenGL context setup as glGenTextures returned 0\n"));
        return false;
    }

    m_InternalFormat = GL_RGBA;
	if( hformat )
	{
		m_InternalFormat = EEGLFormatFromString( hformat );
		if( m_InternalFormat == 0 )
		{
			m_InternalFormat = GL_RGBA;
		}			
		m_Compressed = EEGLIsCompressedFormat( m_InternalFormat );
        m_CompressedSize = EEGLFormatSize(m_InternalFormat, sizex, sizey, colordepth);
	}
	else
		m_InternalFormat = EEGLFormatFromColorBits( colordepth );

    glEnable(GL_TEXTURE_2D);
    if (CheckAndLogGLError() != GL_NO_ERROR)
    {
        return false;
    }
    
    glBindTexture(GL_TEXTURE_2D, m_Texture);
    if (CheckAndLogGLError() != GL_NO_ERROR)
    {
        return false;
    }

    if (mips > 1)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }

    if (CheckAndLogGLError() != GL_NO_ERROR)
    {
        return false;
    }
    
#if 0
    if (m_Compressed)
    {
        glCompressedTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, sizex, sizey, 0, m_CompressedSize, NULL);
    }
    else
    {
        // allocate blank texture
        glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, sizex, sizey, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }
    
    if (CheckAndLogGLError() != GL_NO_ERROR)
    {
        glBindTexture(GL_TEXTURE_2D, 0);
        return false;
    }
#endif

    glBindTexture(GL_TEXTURE_2D, 0);
	m_Height = sizey;
	m_Width = sizex;
	m_ColorDepth = colordepth;
	m_bRenderTarget = false;

	return true;
}

UINT CGLTextureObject::GetHeight()
{
	ReadyData();

	return m_Height;
}

UINT CGLTextureObject::GetWidth()
{
	ReadyData();

	return m_Width;
}

UINT CGLTextureObject::GetColorDepth()
{
	ReadyData();

	return (UINT)m_ColorDepth;
}

bool CGLTextureObject::IsRenderTarget()
{
	return m_bRenderTarget;
}

void* CGLTextureObject::Read( int level, bool bForceUpdateRead )
{
#if 0
	ReadyData();

	//level currently ignored, read cannot read mipmaps
	if( m_Texture )
	{
		LPDIRECT3DDEVICE9 pDevice;
		if( !m_Renderer )
		{
			return NULL;
		}
		pDevice = (LPDIRECT3DDEVICE9)m_Renderer->GetAPIDevice();
		if( !pDevice )
		{
			return NULL;
		}

		int iTextureSize = (GetColorDepth() / 8) * GetWidth() * GetHeight();
		// delete the old buffer if our current one is the wrong size
		if (m_pLocalBuffer && (iTextureSize != m_iLocalBufferSize))
		{
			delete[] m_pLocalBuffer;
			m_pLocalBuffer = NULL;
			m_iLocalBufferSize = 0;
		}
		// allocate a new buffer if we don't have one
		if (!m_pLocalBuffer)
		{
			m_pLocalBuffer = new unsigned char[iTextureSize];
			m_iLocalBufferSize = iTextureSize;
		}
		else
		{
			// return current buffer if we aren't forcing a read
			if (!bForceUpdateRead)
				return m_pLocalBuffer;
		}
		//check what kind of a surface it is, we have to get the surface if it's a rendertarget!
		if( !m_bRenderTarget )
		{
			if( m_Compressed )
			{
				LPDIRECT3DSURFACE9 Src = NULL;
				m_Texture->GetSurfaceLevel( 0, &Src );				
				//convert format to argb first
				LPDIRECT3DSURFACE9 tempSurf = NULL;
				HRESULT hr = pDevice->CreateOffscreenPlainSurface( GetWidth(), GetHeight(),
									D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &tempSurf, NULL );
				if( hr != D3D_OK ||
					tempSurf == NULL ||
					Src == NULL )
				{
					SAFE_RELEASE( Src );
					SAFE_RELEASE( tempSurf );
					return NULL;
				}

				hr = D3DXLoadSurfaceFromSurface( tempSurf, NULL, NULL, Src, NULL, NULL, D3DX_DEFAULT, 0 );			
				if( hr != D3D_OK )
				{
					return NULL;
				}

				//lock the texture and read the data
				D3DLOCKED_RECT  lockrect;
				hr = tempSurf->LockRect( &lockrect, NULL, D3DLOCK_READONLY );
				if( hr == D3D_OK )
				{
					UINT pixelsize = (GetColorDepth()/8);
					BYTE * srcbits = (BYTE*)lockrect.pBits;
					BYTE * destbits = (BYTE*)m_pLocalBuffer;
					//write the texture to the buffer
					for( UINT i = 0; i < m_Height; i++ )
					{
						memcpy( destbits, srcbits, m_Width*pixelsize );
						//move by pitch
						srcbits  +=  lockrect.Pitch;
						destbits += m_Width*pixelsize;
					}
				}
				tempSurf->UnlockRect();
				SAFE_RELEASE( tempSurf );
				SAFE_RELEASE( Src );
				return m_pLocalBuffer;
			}
			else
			{
				//lock the texture and read the data
				D3DLOCKED_RECT  lockrect;
				HRESULT hr = m_Texture->LockRect( 0, &lockrect, NULL, D3DLOCK_READONLY );
				if( hr == D3D_OK )
				{
					UINT pixelsize = (GetColorDepth()/8);
					BYTE * srcbits = (BYTE*)lockrect.pBits;
					BYTE * destbits = (BYTE*)m_pLocalBuffer;
					//write the texture to the buffer
					for( UINT i = 0; i < m_Height; i++ )
					{
						memcpy( destbits, srcbits, m_Width*pixelsize );
						//move by pitch
						srcbits  +=  lockrect.Pitch;
						destbits += m_Width*pixelsize;
					}
				}
				m_Texture->UnlockRect( 0 );
				return m_pLocalBuffer;
			}
		}else   //we are render target, need surface
		{			
			LPDIRECT3DSURFACE9 RenderSurf = NULL;
			LPDIRECT3DSURFACE9 OffSurf = NULL;
			m_Texture->GetSurfaceLevel( 0, &RenderSurf );
			
			if( RenderSurf )
			{
				D3DSURFACE_DESC tDesc;
				m_Texture->GetLevelDesc(0, &tDesc );
				if( FAILED(pDevice->CreateOffscreenPlainSurface( tDesc.Width,
													tDesc.Height,
													tDesc.Format,
													D3DPOOL_SYSTEMMEM,													
													&OffSurf,
													NULL )))
				{
					RenderSurf->Release();
					return NULL;
				}
				if( FAILED( pDevice->GetRenderTargetData( RenderSurf, OffSurf ) ))
				{
					RenderSurf->Release();
					OffSurf->Release();
					return NULL;
				}
				//No need for rendertarget surface anymore
				RenderSurf->Release();
				//lock the texture and read the data
				D3DLOCKED_RECT  lockrect;
				HRESULT hr = OffSurf->LockRect( &lockrect, NULL, D3DLOCK_READONLY );
				if( hr == D3D_OK )
				{
					UINT pixelsize = (GetColorDepth()/8);
					BYTE * srcbits = (BYTE*)lockrect.pBits;
					BYTE * destbits = (BYTE*)m_pLocalBuffer;
					//write the texture to the buffer
					for( UINT i = 0; i < m_Height; i++ )
					{
						memcpy( destbits, srcbits, m_Width*pixelsize );
						//move by pitch
						srcbits  +=  lockrect.Pitch;
						destbits += m_Width*pixelsize;
					}
				}
				OffSurf->UnlockRect();
				OffSurf->Release();
			}
			return m_pLocalBuffer;

		}
	}
#endif
	return NULL;
}

bool CGLTextureObject::Write( void *p, int level, IHashString * informat )
{
	ReadyData();

	if (!m_Renderer)
		return false;

	if (m_Texture && (p || m_pLocalBuffer))
	{
		void *pData = NULL;
		if (p && m_pLocalBuffer) // if we have a local copy and we are given data to write
		{
			// then copy the given data to our localbuffer before using our local buffer
			memcpy( m_pLocalBuffer, p, m_iLocalBufferSize );
			pData = m_pLocalBuffer;
		}
		else if (p) // else if we are given data but don't have a local copy
		{
			// then just write with the data given
			pData = p;
		}
		else // else we weren't given any data
		{
			// so just write with our local copy (which we logically must have if we reached this line)
			pData = m_pLocalBuffer;
		}
        
		GLenum SrcFormat = EEGLFormatFromString( informat );
        
        glEnable(GL_TEXTURE_2D);
        if (CheckAndLogGLError() != GL_NO_ERROR)
		{
			m_ToolBox->Log( LOGERROR, _T("Could not copy surface for write.\n"));
			return false;
		}
        
        // bind the texture before loading
        glBindTexture(GL_TEXTURE_2D, m_Texture);
        
        if (CheckAndLogGLError() != GL_NO_ERROR)
		{
			m_ToolBox->Log( LOGERROR, _T("Could not copy surface for write.\n"));
			return false;
		}
        
        // calculate mip width/height
        UINT levelWidth = m_Width;
        UINT levelHeight = m_Height;
        for (int i=level; i != 0; i--)
        {
            levelWidth >>= 1;
            if (levelWidth == 0)
            {
                levelWidth = 1;
            }
            levelHeight >>= 1;
            if (levelHeight == 0)
            {
                levelHeight = 1;
            }
        }
        
        if (m_Compressed)
        {
            UINT compressedSize = EEGLFormatSize(SrcFormat, levelWidth, levelHeight, (UINT)m_ColorDepth);
            glCompressedTexImage2D(GL_TEXTURE_2D,  level,  SrcFormat,  levelWidth, levelHeight, 0, compressedSize, pData);
        }
        else
        {
            glTexImage2D(GL_TEXTURE_2D, level, SrcFormat, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pData);
        }
        
        if (CheckAndLogGLError() != GL_NO_ERROR)
		{
			m_ToolBox->Log( LOGERROR, _T("Could not copy surface for write.\n"));
			return false;
		}	
        return true;
	}
    
	m_ToolBox->Log( LOGERROR, _T("Error writing to texture.\n"));				
	return false;
}

void CGLTextureObject::GenerateMips()
{
	ReadyData();

	if( m_Texture && m_AutoGenMips )
	{
		//This won't work with autogenerate mipmaps off
		//m_Texture->AddDirtyRect(NULL);
		//m_Texture->GenerateMipSubLevels();
	}
}

UINT CGLTextureObject::GetLevelCount()
{
	if (m_Texture != 0)
    {
        return m_Levels;
    }
	else
		return 0;
}

void * CGLTextureObject::Lock( INT * pitch, int level )
{
#if 0
	ReadyData();

	if( m_Texture )
	{
		D3DLOCKED_RECT  lockrect;
		HRESULT hr = m_Texture->LockRect( level, &lockrect, NULL, 0 );
		if( hr == D3D_OK )
		{
			//should return pitch as well, but that's only DX? Or Use read and write
			if( pitch ){
				*pitch = lockrect.Pitch;
			}
			return lockrect.pBits;
		}
	}
#endif
	return NULL;
}

void CGLTextureObject::Unlock( int level )
{
	ReadyData();

	if( m_Texture )
	{
		//m_Texture->UnlockRect( level );
	}
}

///
/// Init / Update / DeInit
///
void CGLTextureObject::Init()
{
	m_Filename = "";
	m_RefCount = 1;
	m_Texture = 0;
	
    m_InternalFormat = 0;
	m_Height = 0;
	m_Width = 0;
    m_Levels = 0;
	m_ColorDepth = 0;
	m_bRenderTarget = false;
}

void CGLTextureObject::DeInit()
{	
	if (m_Texture)
	{
        glDeleteTextures(1, &m_Texture);
        m_Texture = 0;
	}

	if (m_pLocalBuffer)
	{
		delete (unsigned char *)m_pLocalBuffer;
		m_pLocalBuffer = NULL;
	}

	m_Filename = "";
	m_RefCount = 1;
	m_pLocalBuffer = NULL;
	m_iLocalBufferSize = 0;
	m_Texture = 0;
    m_InternalFormat = 0;

	m_Height = 0;
	m_Width = 0;
    m_Levels = 0;
	m_ColorDepth = 0;
	m_bRenderTarget = false;
}

unsigned int CGLTextureObject::GetReferenceCount()
{
	return m_RefCount;
}

IHashString *CGLTextureObject::GetTextureName()
{
	return &m_Filename;

}

void CGLTextureObject::SetTextureName(IHashString *name)
{
	m_Filename = name;

}

bool CGLTextureObject::ReadyData()
{
	// if our data is loaded, we are ready
	if (IsDataLoaded())
		return true;

	// try to get the data loaded
	LOADFILEEXTPARAMS lfep;
	lfep.fileName = const_cast<TCHAR*>((const TCHAR*)GetName()->GetString());
	lfep.bInternalLoad = true;
	static DWORD msgHash_LoadFileByExtension = CHashString(_T("LoadFileByExtension")).GetUniqueID();
	m_ToolBox->SendMessage(msgHash_LoadFileByExtension, sizeof(LOADFILEEXTPARAMS), &lfep);				

	// return whether our data was loaded
	return IsDataLoaded();
}

GLuint CGLTextureObject::GetRenderTargetSurface()
{
	if (!m_RenderTargetSurface && m_Texture)
	{
        return 0;
		//m_Texture->GetSurfaceLevel( 0, &m_RenderTargetSurface );
	}

	return m_RenderTargetSurface;
}

void CGLTextureObject::ReleaseRenderTargetSurface()
{
    if (m_RenderTargetSurface != 0)
    {
        glDeleteTextures(1, &m_RenderTargetSurface);
    }
}
