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

#include "CGLTextureObject.h"

GLenum EEGLFormatFromColorBits( UINT &colorbits, bool bTiled=false );
DWORD EEGLColorBitsFromFormat( GLenum format );
bool EEGLIsCompressedFormat( GLenum format );
GLenum EEGLFormatFromString( IHashString *hszFormat );
UINT EEGLFormatPitch( GLenum format, UINT width, UINT bitdepth );
GLenum EEGLRequestedInternalFormatFromFileInMemory( void *pFileData, UINT iFileDataSize );
HRESULT EEGLPresent(CGLRenderContext *pGLContext, UINT ClearColor[4] );

GLenum CheckAndLogGLError();
