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

#ifndef CGLIndexBuffer_H
#define CGLIndexBuffer_H

#include "IIndexBuffer.h"

namespace ElementalEngine
{


class CGLIndexBuffer : public IIndexBuffer
{
public:	
	~CGLIndexBuffer();
	CGLIndexBuffer();
	virtual bool Resize( UINT numElements );
	virtual UINT GetSize();
	virtual bool Lock( UINT offset, UINT size, void ** pData );
	virtual bool Unlock();
	
private:
	void DeInit();

	UINT m_Size;
	IRenderer * m_Renderer;
    GLuint  m_IndexBufferID;
};

}//namespace EE
#endif //#ifndef CGLIndexBuffer_H