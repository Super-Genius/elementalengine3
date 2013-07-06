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

#ifndef CGLVertexBufferObject_H
#define CGLVertexBufferObject_H

#include "IVertexBufferObject.h"

namespace ElementalEngine
{
class CGLVertexBufferObject : public IVertexBufferObject
{
public:
	CGLVertexBufferObject();
	~CGLVertexBufferObject();
	virtual bool Initialize( CHANNELDESCRIPTORLIST &channels, UINT numElements, bool bdynamic, UINT streammode );
	virtual void GetDescription( VertexBufferDescriptor &Desc );
	virtual bool Lock( UINT offset, UINT size, void ** pData, DWORD flags );
	virtual bool Unlock();	

	//GL renderer specific
	//LPDIRECT3DVERTEXBUFFER9 GetGLInterface(){
	//	return m_VBInterface;
	//}
	virtual UINT GetStride()
	{
		return m_Stride;
	}

	virtual UINT GetSize()
	{
		return m_NumVertices;
	}

	//GL renderer specific:
	//LPDIRECT3DVERTEXDECLARATION9 GetVertexDeclaration()
	//{
	//	return m_VertexDeclaration;
	//}
	//GL renderer specific:
	DWORD GetFVF()
	{
		return m_FVF;
	}
private:
	//Sets the FVF and vdecl
	//void SetVertexDescription( CHANNELDESCRIPTORLIST &channels, LPDIRECT3DDEVICE9 pDevice, UINT streamode );
	void DeInit();

	UINT					m_NumVertices;
	UINT					m_Stride;
	ElementalEngine::CHANNELDESCRIPTORLIST	m_VertexDescription;
    GLuint                  m_VBOID;
	bool					m_bDynamic;
	IRenderer *				m_Renderer;

	// 	For GL
	DWORD					m_FVF;//if necessary
	GLuint                  m_VertexDeclaration;

};
}

#endif