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
#include "IMeshObject.h"
#include "CGLVertexBufferObject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CGLVertexBufferObject::CGLVertexBufferObject()
{
	m_NumVertices = 0;
	m_VertexDeclaration= NULL;
	m_Stride = 0;
	m_FVF = 0;

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

CGLVertexBufferObject::~CGLVertexBufferObject()
{
	DeInit();
}

void CGLVertexBufferObject::DeInit()
{
	//check if vertex buffer was created before
	m_NumVertices = 0;
    if (m_VBOID != 0)
    {
        glDeleteBuffersARB(1, &m_VBOID);
        m_VBOID = 0;
    }
	m_FVF = 0;
	m_VertexDescription.clear();
	
}

/// \brief Initializees a Vertex buffer
/// \param channels = the vertex description
/// \param numElements = number of vertices to allocate
/// \param bdynamic = dynamic vertex buffer or not? 
/// \return bool = true if successful, false if creation failed. 
bool CGLVertexBufferObject::Initialize( CHANNELDESCRIPTORLIST &channels, UINT numElements, bool bdynamic, UINT streammode )
{
	int totalstride;
	if( !m_Renderer )
	{
		return false;
	}
	
	//deinit if it was previously initialized
	DeInit();

	//Calculate requested stride:
	totalstride = 0;
	//calculate strides
	for( int i = 0; i < (int)channels.size(); i++ )
	{
		ChannelDesc * curStream = &channels[ i ];
		m_VertexDescription.push_back( *curStream );//add to internal format descriptor
		totalstride += curStream->Stride;
	}

	GLenum flags;
	if( bdynamic )
	{
		flags = GL_DYNAMIC_DRAW_ARB;
	}else
	{
		flags = GL_STATIC_DRAW_ARB;
	}
    
    bool retVal = false;
    // generate a VBO id
    glGenBuffersARB(1, &m_VBOID);
    if (CheckAndLogGLError() == GL_NO_ERROR)
    {
        // bind the ID to a TYPE
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_VBOID);
        if (CheckAndLogGLError() == GL_NO_ERROR)
        {
            // reserve space in the VBO
            glBufferDataARB(GL_ARRAY_BUFFER_ARB, numElements * m_Stride, NULL, flags);
            if (CheckAndLogGLError() == GL_NO_ERROR)
            {
                m_Stride = totalstride;
                m_NumVertices = numElements;
                m_bDynamic = bdynamic;
                retVal = true;
            }
            else
            {
                glDeleteBuffersARB(1, &m_VBOID);
            }
        }
        else
        {
            glDeleteBuffersARB(1, &m_VBOID);
        }
    }
    
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
    return retVal;
    
}

void CGLVertexBufferObject::GetDescription( VertexBufferDescriptor &Desc )
{
	if( Desc.outChannelDesc )
	{
		//fill with channel desc
		int size = m_VertexDescription.size();
		for( int i = 0; i < size;i++ )
		{
			Desc.outChannelDesc->push_back( m_VertexDescription[ i ] );
		}
	}
	Desc.outnumElements = m_NumVertices;
}

bool CGLVertexBufferObject::Lock( UINT offset, UINT size, void ** pData, DWORD flags )
{
    // assume error unless everything works.
    bool retVal = false;
    if (m_VBOID != 0)
	{
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_VBOID);
        if (CheckAndLogGLError() == GL_NO_ERROR)
        {
            if ((m_bDynamic && (flags & VBO_LOCK_DISCARD)) || (flags & VBO_LOCK_NOOVERWRITE))
            {
                // tell openGL we want a new buffer to fill in.
                glBufferDataARB(GL_ARRAY_BUFFER_ARB, m_NumVertices * m_Stride, NULL, GL_DYNAMIC_DRAW_ARB);
                if (CheckAndLogGLError() == GL_NO_ERROR)
                {
                    GLenum glFlags = GL_WRITE_ONLY_ARB;
                    if (flags & VBO_FLAGS_READ_ONLY)
                        glFlags = GL_READ_ONLY_ARB;
                    else if (flags & VBO_FLAGS_READ_WRITE)
                        glFlags = GL_READ_WRITE_ARB;
                    
                    *pData = glMapBufferARB(GL_ARRAY_BUFFER_ARB, glFlags);
                    retVal = (CheckAndLogGLError() == GL_NO_ERROR);
                }
            }

        }
	}
    
	return retVal;
}

bool CGLVertexBufferObject::Unlock()
{
    // assume error will happen, I know, I know that is negative.
    bool retVal = false;
	if (m_VBOID != 0)
	{
        glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_VBOID);
        if (CheckAndLogGLError() == GL_NO_ERROR)
        {
            glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
            retVal = (CheckAndLogGLError() == GL_NO_ERROR);
        }
	}
	return retVal;
}

/*
void CGLVertexBufferObject::SetVertexDescription( CHANNELDESCRIPTORLIST &channels, LPDIRECT3DDEVICE9 pDevice, UINT streamode )
{
	
	CGLRenderer * GLRenderer = (CGLRenderer*)m_Renderer;
	m_VertexDeclaration = GLRenderer->GetVertexDescription( channels, streamode );
};
*/