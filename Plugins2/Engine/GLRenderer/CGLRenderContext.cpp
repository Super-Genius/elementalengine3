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
#include "CGLRenderContext.h"
#include "CHashString.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//GL Render Context
CGLRenderContext::CGLRenderContext()
{
	m_ToolBox = EngineGetToolBox();
	m_hWnd = NULL;
	m_ClearColor[0] = 0;
	m_ClearColor[1] = 150;
	m_ClearColor[2] = 150;
	m_ClearColor[3] = 150;
}

void CGLRenderContext::DestroyContext()
{
//	DWORD retval = 0;
//	static DWORD msgHash_DeleteRenderContext = CHashString(_T("DeleteRenderContext")).GetUniqueID();
//	retval = m_ToolBox->SendMessage(msgHash_DeleteRenderContext, sizeof(UINT), this);
//	if (retval != MSG_HANDLED)
//		m_ToolBox->Log( LOGERROR, _T("%s(%i): Failed to DeleteRenderContext.\n"), __FILE__, __LINE__ );

}

void CGLRenderContext::CreateContext( HWND window, const int width, const int height,
									 const int depthbits, const int colorbits )
{
	memset( m_CurrentRenderState, -1, sizeof(m_CurrentRenderState) );
	memset( m_CurrentTextureStageState, -1, sizeof(m_CurrentTextureStageState) );
	memset( m_CurrentSamplerState, -1, sizeof(m_CurrentSamplerState) );
	m_Colorbits = colorbits;
	m_Depthbits = depthbits;
	m_Height = height;
	m_Width = width;
	m_hWnd = window;		
}

IHashString * CGLRenderContext::GetComponentType() 
{
    static CHashString CRCCT(_T("CGLRenderContext"));
	return &CRCCT;
}

bool CGLRenderContext::IsKindOf(IHashString *compType)
{
	return (compType->GetUniqueID() == GetComponentType()->GetUniqueID());
}

void CGLRenderContext::SetBGColor( UINT r, UINT g, UINT b, UINT a )
{
	m_ClearColor[1] = r;
	m_ClearColor[2] = g;
	m_ClearColor[3] = b;
	m_ClearColor[0] = a;
}
	
void CGLRenderContext::GetBGColor( UINT *r, UINT *g, UINT *b, UINT *a )
{
	*r = m_ClearColor[1];
	*g = m_ClearColor[2];
	*b = m_ClearColor[3];
	*a = m_ClearColor[0];
}

