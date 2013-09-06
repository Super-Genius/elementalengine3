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

#include "stdafx.h"
#include "EnumMap.h"
#include "CHashString.hpp"


const EnumMap<const TCHAR *> glErrorStrings = EnumMap<const TCHAR *>
    ( GL_NO_ERROR, _T("No Error\n"))
    ( GL_INVALID_ENUM, _T("GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument.\n"))
    ( GL_INVALID_VALUE, _T("GL_INVALID_VALUE: A numeric argument is out of range.\n"))
    ( GL_INVALID_OPERATION, _T("The specified operation is not allowed in the current state.\n"))
    ( GL_INVALID_FRAMEBUFFER_OPERATION, _T("The framebuffer object is not complete.\n"))
    ( GL_OUT_OF_MEMORY, _T("There is not enough memory left to execute the command.\n"))
    ( GL_STACK_UNDERFLOW, _T("An attempt has been made to perform an operation that would cause an internal stack to underflow.\n"))
    ( GL_STACK_OVERFLOW, _T(")An attempt has been made to perform an operation that would cause an internal stack to overflow.\n"))
;

GLenum CheckAndLogGLError()
{
    GLenum retVal = glGetError();
    
    if (retVal != GL_NO_ERROR)
    {
        // grab an error code to a string for openGL
        EnumMap<const TCHAR *>::const_iterator emIter = glErrorStrings.find(retVal);
        if (emIter != glErrorStrings.end())
        {
            EngineGetToolBox()->Log(LOGERROR, _T("OpenGL Error %s"), emIter->second);
        }
        else
        {
            EngineGetToolBox()->Log(LOGERROR, _T("Unknown OpenGL Error 0x%x"), retVal);
            
        }
    }
    
    return retVal;
}

GLenum EEGLFormatFromColorBits( UINT &colorbits, bool bTiled )
{
	GLenum retval;
	switch(colorbits)
	{
		case 32:
			{
				retval = GL_RGBA8;
				break;
			}
		case 24:
			{
				colorbits = 32;
				retval = GL_RGB8;
				break;
			}
		case 16:
			{
				retval = GL_RGB5;
				break;
			}
		case 8:
			{
				//8 bit alpha, we can ask for 8 bit luminance too..again needs more support work
				//retval = GL_A8; doesn't seem widely supported?
				retval = GL_ALPHA8;
				break;
			}
		default:
			{
				retval = GL_RGBA8;
				break;
			}
	};
	return retval;
}

DWORD EEGLColorBitsFromFormat( GLenum format )
{
	switch(format)
	{
		case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
		case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
		case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
		case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
		case GL_RGBA8:
		case GL_RGB8:
			return 32;

		case GL_ALPHA8:
		case GL_LUMINANCE8:
			return 8;
		default:
			// if this assert is hit, add the format to the switch with the correct
			// parameters for the texture object
			assert( 0 );
			EngineGetToolBox()->Log( LOGERROR, _T("EEGLGetColorBitsFromFormat: Unknown format.\n") );
			return 0;
	}
}

bool EEGLIsCompressedFormat( GLenum format )
{
	switch(format)
	{
		case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
		case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
		case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
		case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
			return true;
		default:
			return false;
	}
}

const EnumMap<GLenum> textureTypes = EnumMap<GLenum>
    ( CHashString(_T("DXT1")).GetUniqueID(), GL_COMPRESSED_RGB_S3TC_DXT1_EXT)
	( CHashString(_T("DXT2")).GetUniqueID(), GL_COMPRESSED_RGBA_S3TC_DXT3_EXT)
	( CHashString(_T("DXT3")).GetUniqueID(), GL_COMPRESSED_RGBA_S3TC_DXT3_EXT)
	( CHashString(_T("DXT4")).GetUniqueID(), GL_COMPRESSED_RGBA_S3TC_DXT5_EXT)
	( CHashString(_T("DXT5")).GetUniqueID(), GL_COMPRESSED_RGBA_S3TC_DXT5_EXT)
	( CHashString(_T("R8G8B8")).GetUniqueID(), GL_RGB8 )
	( CHashString(_T("A8R8G8B8")).GetUniqueID(), GL_RGBA8 )
	( CHashString(_T("X8R8G8B8")).GetUniqueID(), GL_RGB8 )
	( CHashString(_T("R5G6B5")).GetUniqueID(), GL_RGB5 )
	( CHashString(_T("X1R5G5B5")).GetUniqueID(), GL_RGB5 )
	( CHashString(_T("A1R5G5B5")).GetUniqueID(), GL_RGB5_A1 )
	( CHashString(_T("A4R4G4B4")).GetUniqueID(), GL_RGBA4 )
	( CHashString(_T("R3G3B2")).GetUniqueID(), GL_R3_G3_B2 )
	( CHashString(_T("A8")).GetUniqueID(), GL_ALPHA8 )
	( CHashString(_T("X4R4G4B4")).GetUniqueID(), GL_RGB4 )
	( CHashString(_T("A8B8G8R8")).GetUniqueID(), GL_ABGR_EXT )
	( CHashString(_T("X8B8G8R8")).GetUniqueID(), GL_BGR )
    ( CHashString(_T("B8G8R8A8")).GetUniqueID(), GL_BGRA )
	( CHashString(_T("G16R16")).GetUniqueID(), GL_RG16 )
	( CHashString(_T("A2R10G10B10")).GetUniqueID(), GL_RGB10_A2 )
	( CHashString(_T("A16B16G16R16")).GetUniqueID(), GL_RGBA16 )
	( CHashString(_T("L8")).GetUniqueID(), GL_LUMINANCE8 )
	( CHashString(_T("A8L8")).GetUniqueID(), GL_LUMINANCE8_ALPHA8 )
	( CHashString(_T("A4L4")).GetUniqueID(), GL_LUMINANCE4_ALPHA4 )
	( CHashString(_T("D16_LOCKABLE")).GetUniqueID(), GL_DEPTH_COMPONENT16 )
	( CHashString(_T("D32")).GetUniqueID(), GL_DEPTH_COMPONENT32 )
	( CHashString(_T("D24S8")).GetUniqueID(), GL_DEPTH24_STENCIL8_EXT )
	( CHashString(_T("D24X8")).GetUniqueID(), GL_DEPTH_COMPONENT24 )
	( CHashString(_T("D16")).GetUniqueID(), GL_DEPTH_COMPONENT16 )
	( CHashString(_T("D32F_LOCKABLE")).GetUniqueID(), GL_DEPTH_COMPONENT32F )
	( CHashString(_T("D24FS8")).GetUniqueID(), GL_DEPTH32F_STENCIL8 )
	( CHashString(_T("D32_LOCKABLE")).GetUniqueID(), GL_DEPTH_COMPONENT32 )
	( CHashString(_T("L16")).GetUniqueID(), GL_LUMINANCE16)
    ( CHashString(_T("L16F")).GetUniqueID(), GL_LUMINANCE16F_ARB)
	( CHashString(_T("R16F")).GetUniqueID(), GL_R16F )
	( CHashString(_T("G16R16F")).GetUniqueID(), GL_RG16F )
	( CHashString(_T("A16B16G16R16F")).GetUniqueID(), GL_RGBA16F_ARB )
	( CHashString(_T("R32F")).GetUniqueID(), GL_R32F )
	( CHashString(_T("G32R32F")).GetUniqueID(), GL_RG32F )
	( CHashString(_T("A32B32G32R32F")).GetUniqueID(), GL_RGBA32F_ARB )

 ; 

GLenum EEGLFormatFromString( IHashString *hszFormat )
{
	GLenum format = 0;
	EnumMap<GLenum>::const_iterator ehsmIter = textureTypes.find(hszFormat->GetUniqueID());
	if (ehsmIter != textureTypes.end())
	{
		format = ehsmIter->second;
	}
	return format;
}

UINT EEGLFormatPitch( GLenum format, UINT width, UINT bitdepth )
{
	if ((format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) || (format == GL_COMPRESSED_RGB_S3TC_DXT1_EXT))
	{
		return (( width / 4 )* 8);
	}
	else if ((format == GL_COMPRESSED_RGBA_S3TC_DXT3_EXT) || (format == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT))
	{
		return (( width / 4 )* 16);
	}
	else
    {
		return width*(bitdepth/8);
    }
}

UINT EEGLFormatSize( GLenum format, UINT width, UINT height, UINT bitdepth)
{
	if ((format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) || (format == GL_COMPRESSED_RGB_S3TC_DXT1_EXT))
	{
		return (((width + 3)/ 4) * ((height + 3)/4) *  8);
	}
	else if ((format == GL_COMPRESSED_RGBA_S3TC_DXT3_EXT) || (format == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT))
	{
		return (((width + 3)/ 4) * ((height + 3)/4) *  16);
	}
	else
    {
		return width*height*(bitdepth/8);
    }
}

/*
HRESULT EED3DXLoadSurfaceFromMemory( LPDIRECT3DDEVICE9 pDevice, LPDIRECT3DTEXTURE9 pDestTexture, UINT DestLevel, LPCVOID pSrcMemory, D3DFORMAT SrcFormat, UINT SrcColorDepth )
{
	HRESULT hr;
	LPDIRECT3DSURFACE9 DestSurface = NULL;
	D3DSURFACE_DESC DestDesc;		

	pDestTexture->GetSurfaceLevel( DestLevel, &DestSurface );		
	hr = DestSurface->GetDesc( &DestDesc );
	if( hr != D3D_OK )
	{
		SAFE_RELEASE( DestSurface );
		return hr;
	}

	if( SrcFormat == GL_UNKNOWN )
	{
		SrcFormat = EEGLFormatFromColorBits( SrcColorDepth );
	}
	UINT SrcPitch = EEGLFormatPitch( SrcFormat, DestDesc.Width, SrcColorDepth );

	RECT surfsize;
	surfsize.top = 0;
	surfsize.bottom = DestDesc.Height;
	surfsize.left = 0;
	surfsize.right = DestDesc.Width;
	hr = D3DXLoadSurfaceFromMemory( DestSurface, NULL, NULL, pSrcMemory, SrcFormat,SrcPitch, NULL, &surfsize, D3DX_FILTER_NONE, 0 );
	SAFE_RELEASE( DestSurface );
	return D3D_OK;
}
*/

GLenum EEGLRequestedInternalFormatFromFileInMemory( void *pFileData, UINT iFileDataSize )
{
	return 0;
}

/*
HRESULT EEGLClear( LPDIRECT3DDEVICE9 pDevice, bool clearFlags, bool clearTarget )
{
	DWORD clearFlags = 0;
	if( clearDepth )clearFlags  |= D3DCLEAR_ZBUFFER;
	if( clearTarget )clearFlags |= D3DCLEAR_TARGET;
	// Clear the screen
	SetColorMask(true, true, true, true);
	HRESULT hr = pDevice->Clear( 0, NULL, clearFlags, 
				D3DCOLOR_ARGB((int)m_ClearColor[0], (int)m_ClearColor[1], (int)m_ClearColor[2], (int)m_ClearColor[3]),
				1.0f, 0 );
	assert( SUCCEEDED(hr) );
}

 */
HRESULT EEGLPresent(CGLRenderContext *pGLContext, UINT ClearColor[4] )
{
#if 0
	HRESULT result;
	if( pGLContext )
	{
		result = pDevice->Present( NULL, NULL, pGLContext->m_hWnd, NULL );	
	}
	else
	{
		result = pDevice->Present( NULL, NULL, NULL, NULL );	
	}
	return result;
#endif
    return 0;
}


bool GetStringFromFile( StdString &szFilepath, StdString &szOutput )
{
	CHashString streamType(_T("File"));
	CREATEARCHIVE ca;
	ca.mode = STREAM_MODE_READ;
	ca.streamData = (TCHAR*)szFilepath.c_str();
	ca.streamType = &streamType;
	static DWORD msgHash_CreateArchive = CHashString(_T("CreateArchive")).GetUniqueID();
	if (EngineGetToolBox()->SendMessage(msgHash_CreateArchive, sizeof(ca), &ca) != MSG_HANDLED)
		return false;

	ca.archive->Read( szOutput );
	ca.archive->Close();
	return true;
}

BOOL LoadFileIntoBuffer( const TCHAR* pFileName, void*& pFileData, UINT& iFileDataSize, bool bBinary )
{
	CREATESTREAM cs;
	cs.mode = STREAM_MODE_READ | (bBinary ? STREAM_MODE_BINARY : 0);
	cs.streamData = (void*)pFileName;
	cs.streamSize = 0;
	static DWORD msgHash_CreateStream_File = CHashString(_T("CreateStream_File")).GetUniqueID();

	DWORD retval = EngineGetToolBox()->SendMessage(msgHash_CreateStream_File, sizeof(CREATESTREAM), &cs);
	if ((retval != MSG_HANDLED) || (cs.openStream == NULL))
	{
		EngineGetToolBox()->Log( LOGERROR, _T("Error loading file into buffer: %s\n"), pFileName );
		return false;
	}
	
	cs.openStream->SeekTo(0, SEEK_END);
	iFileDataSize = cs.openStream->Tell();
	pFileData = new BYTE[iFileDataSize];
	cs.openStream->SeekTo(0,SEEK_SET);
	cs.openStream->Read( pFileData, iFileDataSize );
	cs.openStream->Close();
	delete cs.openStream;

	return true;
}
