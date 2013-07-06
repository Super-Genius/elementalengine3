///============================================================================
/// \note   Elemental Engine
///         Copyright (c)  2005-2008 Signature Devices, Inc.
///
///         This code is redistributable under the terms of the EE License.
///
///         This code is distributed without warranty or implied warranty of
///			merchantability or fitness for a particular purpose. See the 
///			EE License for more details.
///
///         You should have received a copy of the EE License along with this
///			code; If not, write to Signature Devices, Inc.,
///			3200 Bridge Parkway Suite 102, Redwood City, CA 94086 USA.
///============================================================================

#include "stdafx.h"
#include "EnumMap.h"

#ifdef WIN32
D3DFORMAT EEDX9FormatFromColorBits( UINT &colorbits, bool bTiled )
{
	D3DFORMAT retval;
	switch(colorbits)
	{
		case 32:
			{
				retval = D3DFMT_A8R8G8B8;
				break;
			}
		case 24:
			{
				colorbits = 32;
				retval = D3DFMT_X8R8G8B8;
				break;
			}
		case 16:
			{
				retval = D3DFMT_R5G6B5;
				break;
			}
		case 8:
			{
				//8 bit alpha, we can ask for 8 bit luminance too..again needs more support work
				//retval = D3DFMT_A8; doesn't seem widely supported?
				retval = D3DFMT_L8;
				break;
			}
		default:
			{
				retval = D3DFMT_A8R8G8B8;
				break;
			}
	};
	return retval;
}
#endif//WIN32

#ifdef WIN32
DWORD EEDX9ColorBitsFromFormat( D3DFORMAT format )
{
	switch(format)
	{
		case D3DFMT_DXT5:
		case D3DFMT_DXT3:
		case D3DFMT_DXT1:
		case D3DFMT_A8R8G8B8:
		case D3DFMT_X8R8G8B8:
			return 32;

		case D3DFMT_A8:
		case D3DFMT_L8:
			return 8;
		default:
			// if this assert is hit, add the format to the switch with the correct
			// parameters for the texture object
			assert( 0 );
			EngineGetToolBox()->Log( LOGERROR, _T("EEDX9GetColorBitsFromFormat: Unknown format.\n") );
			return 0;
	}
}
#endif//WIN32

#ifdef WIN32
bool EEDX9IsCompressedFormat( D3DFORMAT format )
{
	switch(format)
	{
		case D3DFMT_DXT5:
		case D3DFMT_DXT3:
		case D3DFMT_DXT1:
			return true;
		default:
			return false;
	}
}
#endif//WIN32

const EnumMap<D3DFORMAT> textureTypes = EnumMap<D3DFORMAT>
	( CHashString(_T("DXT1")).GetUniqueID(), D3DFMT_DXT1 )
	( CHashString(_T("DXT2")).GetUniqueID(), D3DFMT_DXT2 )
	( CHashString(_T("DXT3")).GetUniqueID(), D3DFMT_DXT3 )
	( CHashString(_T("DXT4")).GetUniqueID(), D3DFMT_DXT4 )
	( CHashString(_T("DXT5")).GetUniqueID(), D3DFMT_DXT5 )
	( CHashString(_T("R8G8B8")).GetUniqueID(), D3DFMT_A32B32G32R32F )
	( CHashString(_T("A8R8G8B8")).GetUniqueID(), D3DFMT_A8R8G8B8 )
	( CHashString(_T("X8R8G8B8")).GetUniqueID(), D3DFMT_X8R8G8B8 )
	( CHashString(_T("R5G6B5")).GetUniqueID(), D3DFMT_R5G6B5 )
	( CHashString(_T("X1R5G5B5")).GetUniqueID(), D3DFMT_X1R5G5B5 )
	( CHashString(_T("A1R5G5B5")).GetUniqueID(), D3DFMT_A1R5G5B5 )
	( CHashString(_T("A4R4G4B4")).GetUniqueID(), D3DFMT_A4R4G4B4 )
	( CHashString(_T("R3G3B2")).GetUniqueID(), D3DFMT_R3G3B2 )
	( CHashString(_T("A8")).GetUniqueID(), D3DFMT_A8 )
	( CHashString(_T("A8R3G3B2")).GetUniqueID(), D3DFMT_A8R3G3B2 )
	( CHashString(_T("X4R4G4B4")).GetUniqueID(), D3DFMT_X4R4G4B4 )
	( CHashString(_T("A2B10G10R10")).GetUniqueID(), D3DFMT_A2B10G10R10 )
	( CHashString(_T("A8B8G8R8")).GetUniqueID(), D3DFMT_A8B8G8R8 )
	( CHashString(_T("X8B8G8R8")).GetUniqueID(), D3DFMT_X8B8G8R8 )
	( CHashString(_T("G16R16")).GetUniqueID(), D3DFMT_G16R16 )
	( CHashString(_T("A2R10G10B10")).GetUniqueID(), D3DFMT_A2R10G10B10 )
	( CHashString(_T("A16B16G16R16")).GetUniqueID(), D3DFMT_A16B16G16R16 )
	( CHashString(_T("A8P8")).GetUniqueID(), D3DFMT_A8P8 )
	( CHashString(_T("P8")).GetUniqueID(), D3DFMT_P8 )
	( CHashString(_T("L8")).GetUniqueID(), D3DFMT_L8 )
	( CHashString(_T("A8L8")).GetUniqueID(), D3DFMT_A8L8 )
	( CHashString(_T("A4L4")).GetUniqueID(), D3DFMT_A4L4 )
	( CHashString(_T("V8U8")).GetUniqueID(), D3DFMT_V8U8 )
	( CHashString(_T("L6V5U5")).GetUniqueID(), D3DFMT_L6V5U5 )
	( CHashString(_T("X8L8V8U8")).GetUniqueID(), D3DFMT_X8L8V8U8 )
	( CHashString(_T("Q8W8V8U8")).GetUniqueID(), D3DFMT_Q8W8V8U8 )
	( CHashString(_T("V16U16")).GetUniqueID(), D3DFMT_V16U16 )
	( CHashString(_T("A2W10V10U10")).GetUniqueID(), D3DFMT_A2W10V10U10 )
	( CHashString(_T("UYVY")).GetUniqueID(), D3DFMT_UYVY )
	( CHashString(_T("R8G8_B8G8")).GetUniqueID(), D3DFMT_R8G8_B8G8 )
	( CHashString(_T("YUY2")).GetUniqueID(), D3DFMT_YUY2 )
	( CHashString(_T("D16_LOCKABLE")).GetUniqueID(), D3DFMT_D16_LOCKABLE )
	( CHashString(_T("D32")).GetUniqueID(), D3DFMT_D32 )
	( CHashString(_T("D15S1")).GetUniqueID(), D3DFMT_D15S1 )
	( CHashString(_T("D24S8")).GetUniqueID(), D3DFMT_D24S8 )
	( CHashString(_T("D24X8")).GetUniqueID(), D3DFMT_D24X8 )
	( CHashString(_T("D24X4S4")).GetUniqueID(), D3DFMT_D24X4S4 )
	( CHashString(_T("D16")).GetUniqueID(), D3DFMT_D16 )
	( CHashString(_T("D32F_LOCKABLE")).GetUniqueID(), D3DFMT_D32F_LOCKABLE )
	( CHashString(_T("D24FS8")).GetUniqueID(), D3DFMT_D24FS8 )
	( CHashString(_T("D32_LOCKABLE")).GetUniqueID(), D3DFMT_D32_LOCKABLE )
	( CHashString(_T("S8_LOCKABLE")).GetUniqueID(), D3DFMT_S8_LOCKABLE )
	( CHashString(_T("L16")).GetUniqueID(), D3DFMT_L16 )
	( CHashString(_T("Q16W16V16U16")).GetUniqueID(), D3DFMT_Q16W16V16U16 )
	( CHashString(_T("MULTI2_ARGB8")).GetUniqueID(), D3DFMT_MULTI2_ARGB8 )
	( CHashString(_T("R16F")).GetUniqueID(), D3DFMT_R16F )
	( CHashString(_T("G16R16F")).GetUniqueID(), D3DFMT_G16R16F )
	( CHashString(_T("A16B16G16R16F")).GetUniqueID(), D3DFMT_A16B16G16R16F )
	( CHashString(_T("R32F")).GetUniqueID(), D3DFMT_R32F )
	( CHashString(_T("G32R32F")).GetUniqueID(), D3DFMT_G32R32F )
	( CHashString(_T("A32B32G32R32F")).GetUniqueID(), D3DFMT_A32B32G32R32F )
	( CHashString(_T("CxV8U8")).GetUniqueID(), D3DFMT_CxV8U8 )
	( CHashString(_T("A1")).GetUniqueID(), D3DFMT_A1 )
	( CHashString(_T("A2B10G10R10_XR_BIAS")).GetUniqueID(), D3DFMT_A2B10G10R10_XR_BIAS )
 ;

D3DFORMAT EEDX9FormatFromString( IHashString *hszFormat )
{
	D3DFORMAT format = D3DFMT_UNKNOWN;
	EnumMap<D3DFORMAT>::const_iterator ehsmIter = textureTypes.find(hszFormat->GetUniqueID());
	if (ehsmIter != textureTypes.end())
	{
		format = ehsmIter->second;
	}
	return format;
}

UINT EEDX9FormatPitch( D3DFORMAT format, UINT width, UINT bitdepth )
{
	if( format == D3DFMT_DXT1 )
	{
		return (( width / 4 )* 8);
	}
	else if( format == D3DFMT_DXT3 || format == D3DFMT_DXT5 )
	{
		return (( width / 4 )* 16);
	}
	else
		return width*(bitdepth/8);
}

#ifdef WIN32
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

	if( SrcFormat == D3DFMT_UNKNOWN )
	{
		SrcFormat = EEDX9FormatFromColorBits( SrcColorDepth );
	}
	UINT SrcPitch = EEDX9FormatPitch( SrcFormat, DestDesc.Width, SrcColorDepth );

	RECT surfsize;
	surfsize.top = 0;
	surfsize.bottom = DestDesc.Height;
	surfsize.left = 0;
	surfsize.right = DestDesc.Width;
	hr = D3DXLoadSurfaceFromMemory( DestSurface, NULL, NULL, pSrcMemory, SrcFormat,SrcPitch, NULL, &surfsize, D3DX_FILTER_NONE, 0 );
	SAFE_RELEASE( DestSurface );
	return D3D_OK;
}
#endif//WIN32

#ifdef WIN32
D3DFORMAT EEDX9RequestedInternalFormatFromFileInMemory( LPVOID pFileData, UINT iFileDataSize )
{
	return D3DFMT_UNKNOWN;
}
#endif//WIN32

#ifdef WIN32
/*
HRESULT EEDX9Clear( LPDIRECT3DDEVICE9 pDevice, bool clearFlags, bool clearTarget )
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

HRESULT EEDX9Present( LPDIRECT3DDEVICE9 pDevice, CDX9RenderContext *pDX9Context, UINT ClearColor[4] )
{
	HRESULT result;
	if( pDX9Context )
	{
		result = pDevice->Present( NULL, NULL, pDX9Context->m_hWnd, NULL );	
	}
	else
	{
		result = pDevice->Present( NULL, NULL, NULL, NULL );	
	}
	return result;
}
#endif//WIN32

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

BOOL LoadFileIntoBuffer( const TCHAR* pFileName, LPVOID& pFileData, UINT& iFileDataSize, bool bBinary )
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
