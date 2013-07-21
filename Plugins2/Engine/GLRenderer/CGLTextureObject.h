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

#ifndef CGLTextureObject_H
#define CGLTextureObject_H

#include "ITextureObject.h"

namespace ElementalEngine
{

class CGLTextureObject : public CObjectTemplate<ITextureObject >
{	
private:
	/// \brief The Default Constructor for CMeshObject
	/// \param parentName = name of parent in hierarchy
	/// \param name = IHashString * name of this object
	CGLTextureObject(IHashString *parentName, IHashString *name);

public:
	/// \brief	The Default Destructor for CMeshObject
	~CGLTextureObject();

	/// \brief	Create the a instance of IObject
	/// \return	IComponent pointer
	static IComponent *Create(int nArgs, va_list argptr);

	/// \brief	The Initialization for the Object
	/// \param	IManager* pManager: Parent Manager
	virtual void Init();
	/// \brief	The DeInit Function for the Object
	virtual void DeInit();

	/// \brief Loads a texture up
	/// \param file = the filename, relative texture pathNAME
	virtual bool LoadFromFile(  const TCHAR * file );

	/// \brief either reads or writes the entity information to a file
	virtual void Serialize(IArchive &ar){};

	virtual bool IsDataLoaded() const { return (m_Texture != 0); }
	virtual bool ReadyData();

	GLuint GetRenderTargetSurface();
	void ReleaseRenderTargetSurface();

public:
	//Interface Functions:
	virtual IHashString *GetTextureName();
	virtual void* Read( int level, bool bForceUpdateRead );
	virtual bool Write( void *p, int level ,IHashString * informat  );
    virtual void * Lock( INT * pitch, int level );
	virtual void Unlock( int level );
	virtual int GetNumLevels();

	virtual unsigned int GetReferenceCount();
	virtual unsigned int IncrementRefCount(){ m_RefCount++; return m_RefCount; }
	virtual unsigned int DecrementRefCount(){ m_RefCount--; return m_RefCount;}
	
	virtual UINT GetHeight();
	virtual UINT GetWidth();
	virtual UINT GetColorDepth();

	virtual bool MakeBlankTexture( UINT sizex, UINT sizey, UINT colordepth, IHashString * format, UINT mips);
	virtual bool MakeRenderTarget( UINT sizex, UINT sizey, UINT colordepth, DWORD type = RENDER_TARGET_COLOR_BUFFER, bool bAutoGenMipMaps=false );

	virtual void SetTextureName(IHashString *newName);
	virtual void GenerateMips();
	virtual UINT GetLevelCount();
	virtual bool IsRenderTarget();
	//for GL adapters:
	void * GetAPITexture()
	{
		ReadyData();
		return (void*)m_Texture;
	}
	virtual TEXTURETYPE GetTextureType()
	{
		return EE_TEXTURE2D;
	}

private:
	bool LoadHDRFile( TCHAR * file );
    GLuint  m_Texture;
	GLuint m_RenderTargetSurface;
	unsigned int    m_RefCount;
	CHashString     m_Filename;
	void *m_pLocalBuffer;
	int m_iLocalBufferSize;
	UINT m_Height;
	UINT m_Width;
    UINT m_Levels;
	DWORD m_ColorDepth;
    GLenum m_InternalFormat;
	bool m_bRenderTarget;
	bool m_Compressed;
    UINT m_CompressedSize;
	bool m_AutoGenMips;

	IRenderer * m_Renderer;
};

} //namespace EE

#endif