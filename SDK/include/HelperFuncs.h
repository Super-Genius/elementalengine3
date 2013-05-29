///============================================================================
/// \file	HelperFuncs.h
/// \brief  Helper functions for easier direct calls
/// \date   05/27/2013
/// \author Kenneth Hurley
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
///			code;
///============================================================================ 
#ifndef _HELPER_FUNCS_H_
#define _HELPER_FUNCS_H_

extern "C" {
	
	/// Get a Rendering Interface
	/// returns IRenderer that is the loaded rendering engine or NULL if none loaded
	IRenderer *GetRendererInterface();

	/// Create a texture
	/// param: Format = HashString containing texture type, i.e. "DXT1", "A8R8G8B8", etc.
	/// param: sizeX = width of texture
	/// param: sizeY = height of texture
	/// param: bitDepth = # bits per pixel (could derive from Format??)
	/// param: bRenderTargetTexture = RENDER_TARGET_TYPE if a render target or RENDER_TARGET_NONE
	/// param: numMips = number of mip maps to create
	/// param: bAutoGenMipMaps = auto generate mips when creating texture
	/// param: Name = unique name to create it under
	/// returns: IBaseTextureObject of created texture or NULL
	IBaseTextureObject *CreateTexture(IHashString *Format, UINT sizeX, UINT sizeY, UINT bitDepth, RENDER_TARGET_TYPE bRenderTargetTexture = RENDER_TARGET_NONE,
		UINT numMips = 0, bool bAutoGenMipMaps = false, IHashString *Name = NULL);

	/// Load a texture from a file
	/// param: filename - const TCHAR pointer to filename
	/// returns: IBaseTextureObject of loaded texture or NULL for error
	IBaseTextureObject *LoadTexture(const TCHAR *filename);

}


#endif