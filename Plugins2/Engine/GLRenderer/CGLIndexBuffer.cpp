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
#include "CGLIndexBuffer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CGLIndexBuffer::~CGLIndexBuffer()
{
	DeInit();
}

CGLIndexBuffer::CGLIndexBuffer()
{
	m_Size = 0;
	m_IndexBufferID = 0;
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

bool CGLIndexBuffer::Resize( UINT numElements )
{
	if( !m_Renderer )
	{
		return false;
	}

    // get rid of old buffers
	DeInit();

    
    // assume failure
    bool retVal = false;
    
    // generate a IBO id
    glGenBuffersARB(1, &m_IndexBufferID);
    if (CheckAndLogGLError() == GL_NO_ERROR)
    {
        // bind the ID to a TYPE
        glBindBufferARB(GL_INDEX_ARRAY, m_IndexBufferID);
        if (CheckAndLogGLError() == GL_NO_ERROR)
        {
            // reserve space in the IBO, 16-bit index only
            glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, numElements * sizeof(GLuint), NULL, GL_DYNAMIC_DRAW_ARB);
            if (CheckAndLogGLError() == GL_NO_ERROR)
            {
                m_Size = numElements;
                retVal = true;
            }
            else
            {
                // some OpenGL error must have happened
                glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
                glDeleteBuffersARB(1, &m_IndexBufferID);
            }
        }
        else
        {
            glDeleteBuffersARB(1, &m_IndexBufferID);
        }
    }
    
    return retVal;
    
};

UINT CGLIndexBuffer::GetSize()
{
	return m_Size;
}

void CGLIndexBuffer::DeInit()
{
	if (m_IndexBufferID != 0)
	{
        glDeleteBuffersARB(1, &m_IndexBufferID);
        CheckAndLogGLError();
        m_IndexBufferID = 0;
		m_Size = 0;
	}
}

bool CGLIndexBuffer::Lock( UINT offset, UINT size, void ** pData)
{
    // assume an error will occur
    bool retVal = false;
    if (m_IndexBufferID != 0)
	{
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_IndexBufferID);
        if (CheckAndLogGLError() == GL_NO_ERROR)
        {
            // tell openGL we want a new buffer to fill in.
            glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_Size * sizeof(GLuint), NULL, GL_DYNAMIC_DRAW_ARB);
            if (CheckAndLogGLError() == GL_NO_ERROR)
            {                
                *pData = glMapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);
                retVal = (CheckAndLogGLError() == GL_NO_ERROR);
            }
            
        }
	}
    
	return retVal;
}

bool CGLIndexBuffer::Unlock()
{
    // assume error will occur
    bool retVal = false;
	if (m_IndexBufferID != 0)
	{
        glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_IndexBufferID);
        if (CheckAndLogGLError() == GL_NO_ERROR)
        {
            glUnmapBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB);
            retVal = (CheckAndLogGLError() == GL_NO_ERROR);
        }
	}
	return retVal;
}
